#include "C_AudioSwitch.h"

#include "External/imgui/imgui_internal.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

#include "Globals.h"
#include "Log.h"
#include "M_SceneManager.h"
#include "GameObject.h"
#include "Effects.h"
#include "Importer.h"
#include "PanelChooser.h"
#include "Engine.h"
#include "M_Editor.h"

#include "MathGeoLib/Math/MathFunc.h"

C_AudioSwitch::C_AudioSwitch(GameObject* parent) : C_Audio(parent)
{
	type = ComponentType::AUDIO_SWITCH;

    totalTracks = 0;

    switching = false;
    nextSwitchTrack = 0;

    fadeTime = 2.0f;
    switchTime = 0.0f;
    pauseDifference = 0.0f;

    trackIdInEdit = -1;
    openEditor = false;

    offsetSync = false;
    oldOffset = 0.0f;
    editorOffset = 0.0f;

    oldTrack = nullptr;
    newTrack = nullptr;
    playingTrack = nullptr;
}

C_AudioSwitch::~C_AudioSwitch()
{
    for (R_Track* index : tracks)
    {
        if (index->IsTrackLoaded())
            StopAudio(index->source);

        RELEASE(index);
    }

    tracks.clear();
    tracks.shrink_to_fit();
}

bool C_AudioSwitch::Start()
{


    return true;
}

bool C_AudioSwitch::OnPlay()
{
    for (R_Track* index : tracks)
    {
        if (index->IsTrackLoaded())
        {
            StopAudio(index->source);
            if (index->GetPlayOnStart())
            {
                playingTrack = index;

                PlayAudio(index->source, index->offset);
            }
        }
    }
    switching = false;
    switchTime = 0.0f;
    return true;
}

bool C_AudioSwitch::Update(float dt)
{
    bool ret = true;

    if (switching)
        SwitchFade(fadeTime);

    for (R_Track* index : tracks)
    {
        if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PAUSED)
        {
            PauseAudio(index->source);
            if (switching)
                pauseDifference = owner->GetEngine()->GetEngineConfig()->startupTime.ReadSec() - switchTime;

            continue;
        }
        //ResumeAudio(index->source);
    }

    UpdatePlayState();

    return ret;
}

bool C_AudioSwitch::InspectorDraw(PanelChooser* chooser)
{
    bool ret = true;

    if (openEditor && trackIdInEdit != -1)
        DrawEditor(tracks[trackIdInEdit]);

    if (ImGui::CollapsingHeader("Audio Switch"))
    {
        DrawDeleteButton(owner, this);

        if (chooser->IsReadyToClose("Add Track"))
        {
            if (chooser->OnChooserClosed() != nullptr)
            {
                std::string path = chooser->OnChooserClosed();

                if (!path.empty())
                {
                    R_Track* track = new R_Track();
                    Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
                    track->source = CreateAudioSource(track->buffer, false);

                    track->SetVolume();
                    track->SetLoop(track->loop);
                    track->SetPanning();
                    track->SetTranspose();

                    tracks.push_back(track);
                    ++totalTracks;
                }
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("Add Track") && totalTracks < 9)
        {
            chooser->OpenPanel("Add Track", "wav", { "wav" });
        }
        // STOP TRACK
        if (IsAnyTrackPlaying())
        {
            if (totalTracks < 9)
                ImGui::SameLine();

            if (ImGui::Button("Stop Track"))
            {
                StopAllTracks();
                playingTrack = nullptr;
            }
        }
        ImGui::Spacing();


        if (totalTracks <= 1)
            ImGui::BeginDisabled();
        ImGui::Separator();
        ImGui::Spacing();

        // SWITCH BUTTON
        if (ImGui::Button("Switch To") && !switching)
        {
            SwitchTrack(nextSwitchTrack);
        }
        ImGui::SameLine();
        ImGui::DragInt("##Switch", &nextSwitchTrack, 0.1f, 0, tracks.size() - 1, "Track %d");

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Fade Time "); ImGui::SameLine(73);
        ImGui::SameLine(87);
        ImGui::DragFloat("##FadeTime", &fadeTime, 0.1f, 0.1f, 10.0f, "%.2f sec");
        ImGui::Checkbox("Offset Sync", &offsetSync);

        ImGui::Spacing();
        if (totalTracks <= 1)
            ImGui::EndDisabled();

        if (totalTracks > 0)
        {
            // TRACK LIST
            for (uint i = 0; i < tracks.size(); ++i)
            {
                ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);


                R_Track* index = tracks[i];
                if (index == nullptr)
                {
                    CONSOLE_LOG("[ERROR] Fatal Error iterating tracks. Equals nullptr.");
                    return ret;
                }

                ImGui::Separator();
                ImGui::Spacing();
                if (ImGui::Button("Delete"))
                {
                    openEditor = false;
                    trackIdInEdit = -1;

                    totalTracks--;

                    if (nextSwitchTrack > i && nextSwitchTrack != 0)
                        nextSwitchTrack--;

                    StopAudio(index->source);
                    tracks.erase(tracks.begin() + i);
                    RELEASE(index);

                    ImGui::PopID();
                    ImGui::SameLine();
                    continue;
                }

                ImGui::SameLine();

                std::string name = "Track " + std::to_string(i) + ": " + index->name.c_str();
                if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    if (index->IsTrackLoaded()) // If track is loaded
                    {
                        bool mono = index->channels == 1;
                        ImGui::Text("Sample Rate: %d kHz (%d-Bits)", index->sampleRate, index->bits);
                        ImGui::Text("Channels: %d", index->channels);
                        if (!mono)
                        {
                            ImGui::SameLine();
                            ImGui::Text("(No Pan)");
                        }
                        ImGui::Spacing();

                        if (ImGui::Button("Edit"))
                        {
                            if (i == trackIdInEdit)
                            {
                                openEditor = false;
                                trackIdInEdit = -1;
                            }
                            else if (i != trackIdInEdit)
                            {
                                openEditor = true;
                                trackIdInEdit = i;
                            }
                        }
                        ImGui::SameLine();

                        ImGui::SliderFloat("Offset", &index->offset, 0.0f, index->duration);

                        ImGui::Spacing();
                        ImGui::Text("Options");

                        if (ImGui::Checkbox("Mute", &index->mute))
                            index->SetVolume();

                        if (ImGui::Checkbox("Play on start", &index->playOnStart) && index->playOnStart)
                        {
                            DisablePlayOnStart(index);
                        }

                        if (ImGui::Checkbox("Loop", &index->loop))
                            index->SetLoop(index->loop);

                        if (ImGui::Checkbox("Bypass SFX", &index->bypass))
                        {
                            for (uint i = 0; i < index->effects.size(); ++i)
                                index->effects[i]->ToggleBypass(!index->bypass);
                        }
                    }
                }
                ImGui::Spacing();
                ImGui::PopID();
            }
        }
    }
    else
        DrawDeleteButton(owner, this);

    return ret;
}

void C_AudioSwitch::Save(Json& json) const
{
    json["type"] = "audio_switch";

    json["total_tracks"] = totalTracks;
    json["fade_time"] = fadeTime;
    json["offset_sync"] = offsetSync;

    Json jsonTrack;
    for (R_Track* track : tracks)
    {
        jsonTrack["track_path"] = track->GetTrackPath();
        jsonTrack["mute"] = track->GetMute();
        jsonTrack["play_on_start"] = track->GetPlayOnStart();
        jsonTrack["loop"] = track->GetLoop();
        jsonTrack["bypass"] = track->GetBypass();
        jsonTrack["volume"] = track->GetVolume();
        jsonTrack["pan"] = track->GetPan();
        jsonTrack["transpose"] = track->GetTranspose();
        jsonTrack["offset"] = track->GetOffset();

        json["tracks"].push_back(jsonTrack);
    }
}

void C_AudioSwitch::Load(Json& json)
{
    openEditor = false;
    trackIdInEdit = -1;
    editorOffset = 0.0f;

    if (IsAnyTrackPlaying())
    {
        R_Track* trackToStop = GetPlayingTrack();
        StopAudio(trackToStop->source);
    }

    fadeTime = json.at("fade_time");
    offsetSync = json.at("offset_sync");

    if (totalTracks != 0 && json.at("total_tracks") > 0)
    {
        // TODO: Check already existing tracks & update them
        return;
    }
    else if (totalTracks == 0 && json.at("total_tracks") > 0)
    {
        totalTracks = json.at("total_tracks");

        for (const auto& index : json.at("tracks").items())
        {
            std::string path = index.value().at("track_path");

            R_Track* track = new R_Track();
            Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
            track->source = CreateAudioSource(track->buffer, false);

            track->SetMute(index.value().at("mute"));
            track->SetPlayOnStart(index.value().at("play_on_start"));
            track->SetLoop(index.value().at("loop"));
            track->SetBypass(index.value().at("bypass"));
            track->SetVolume(index.value().at("volume"));
            track->SetPanning(index.value().at("pan"));
            track->SetTranspose(index.value().at("transpose"));
            track->SetOffset(index.value().at("offset"));

            tracks.push_back(track);
        }
    }
}

void C_AudioSwitch::UpdatePlayState()
{
    for (R_Track* index : tracks)
    {
        if (index->IsTrackLoaded())
        {
            ALint sourceState;
            alGetSourcei(index->source, AL_SOURCE_STATE, &sourceState);
            (sourceState == AL_PLAYING) ? index->play = true : index->play = false;
        }
    }
}

void C_AudioSwitch::SwitchTrack(int newTrackIndex)
{
    if (newTrackIndex >= tracks.size())
        return;

    nextSwitchTrack = newTrackIndex;

    oldTrack = GetPlayingTrack();
    newTrack = tracks[nextSwitchTrack];

    if (offsetSync && oldTrack != nullptr)
        alGetSourcef(oldTrack->source, AL_SEC_OFFSET, &oldOffset);

    if (oldTrack != nullptr && !newTrack->play && newTrack->IsTrackLoaded())
    {
        switching = true;
        switchTime = owner->GetEngine()->GetEngineConfig()->startupTime.ReadSec();
    }
}

void C_AudioSwitch::PlayTrack(int trackIndex)
{
    if (tracks[trackIndex] != nullptr)
    {
        playingTrack = tracks[trackIndex];

        PlayAudio(tracks[trackIndex]->source, tracks[trackIndex]->offset);
    }
}

void C_AudioSwitch::ResumeTrack(int trackIndex)
{
    if (tracks[trackIndex] != nullptr)
        ResumeAudio(tracks[trackIndex]->source);
}

void C_AudioSwitch::StopTrack(int trackIndex)
{
    if (tracks[trackIndex] != nullptr)
        StopAudio(tracks[trackIndex]->source);
}

void C_AudioSwitch::PauseTrack(int trackIndex)
{
    if (tracks[trackIndex] != nullptr)
        PauseAudio(tracks[trackIndex]->source);
}

void C_AudioSwitch::SwitchFade(float fadeSeconds)
{
    if (!newTrack->play)
    {
        PlayAudio(newTrack, oldOffset + newTrack->offset);
        alSourcef(newTrack->source, AL_GAIN, 0.0f);
    }

    if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PAUSED)
    {
        switchTime = owner->GetEngine()->GetEngineConfig()->startupTime.ReadSec() - pauseDifference;
        return;
    }

    float t = owner->GetEngine()->GetEngineConfig()->startupTime.ReadSec() - switchTime;
    float d = fadeSeconds;
    if (t > d)
    {
        switching = false; // Stop source
        StopAudio(oldTrack->source);
        oldTrack->SetVolume();

        newTrack->SetVolume();

        playingTrack = newTrack;

        oldTrack = nullptr;
        newTrack = nullptr;
        pauseDifference = 0;
        oldOffset = 0.0f;
    }
    else
    {
        float volume = oldTrack->volume;
        volume = Pow(volume, 2.5f) / 1000.0f;
        if (volume > 99.0f) volume = 100.0f;
        volume = (volume / 100) * (1.0f - t / d);
        alSourcef(oldTrack->source, AL_GAIN, volume);

        float nvolume = newTrack->volume;
        nvolume = Pow(nvolume, 2.5f) / 1000.0f;
        if (nvolume > 99.0f) nvolume = 100.0f;
        nvolume = (nvolume / 100) * (t / d);
        alSourcef(newTrack->source, AL_GAIN, nvolume);
    }
}

bool C_AudioSwitch::IsAnyTrackPlaying() const
{
    for (R_Track* index : tracks)
    {
        if (!index->IsTrackLoaded())
            continue;

        if (index->play)
            return true;
    }
    return false;
}

R_Track* C_AudioSwitch::GetPlayingTrack() const
{
    for (R_Track* index : tracks)
    {
        if (index->play)
            return index;
    }
    return nullptr;
}
int C_AudioSwitch::GetPlayingTrackID() const
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->play)
            return i;
    }
    return -1;
}

void C_AudioSwitch::StopAllTracks()
{
    for (R_Track* index : tracks)
    {
        if (index->IsTrackLoaded())
            StopAudio(index->source);
    }
}

void C_AudioSwitch::DisablePlayOnStart(R_Track* trackToChange)
{
    for (R_Track* index : tracks)
    {
        if (!index->IsTrackLoaded() || trackToChange == index)
            continue;

        index->playOnStart = false;
    }
}

void C_AudioSwitch::DrawEditor(R_Track* track)
{
    if (ImGui::Begin("Audio M_Editor", &openEditor))
    {
        bool mono = track->channels == 1;

        if (ImGui::BeginTable("Upper Table", 2))
        {
            float winWidth = ImGui::GetWindowWidth();
            float resultWidth = winWidth > 627 ? 350.0f : (350.0f - (627.0f - winWidth));
            resultWidth = winWidth < 408 ? 148 : resultWidth;
            float width = winWidth > 627 ? ((winWidth * 0.545f) - 350) : ((winWidth * ((54.4f + ((627.0f - winWidth) / 100)) / 100)) - resultWidth);

            ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, resultWidth); // Default to 100.0f
            ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            {
                ImGui::Text("Track %d: %s", trackIdInEdit, track->name.c_str());
                ImGui::Spacing();
                ImGui::Text("%d kHz | %d-Bits", track->sampleRate, track->bits);
                ImGui::Spacing();
                ImGui::Text("%d Channels", track->channels);
                if (!mono)
                {
                    ImGui::SameLine(); ImGui::Text(" (No Pan)");
                }
            }

            ImGui::TableSetColumnIndex(1);
            {
                char t1[] = { "Play" };
                char t2[] = { "Stop" };
                char* t = track->play ? t = t2 : t = t1;
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Dummy(ImVec2{ width, 0.0f }); ImGui::SameLine();
                if (ImGui::Button(t))
                {
                    //float time = track->duration * editorOffset;
                    bool p = false;

                    if (track->play)
                        StopAudio(track->source);
                    else
                        p = true;

                    if (p)
                    {
                        StopAllTracks();
                        PlayAudio(track->source, editorOffset);
                        playingTrack = track;
                    }
                    else
                    {
                        playingTrack = nullptr;
                    }

                }
            }
            ImGui::EndTable();
        }

        ImGui::Dummy(ImVec2{ 0.0f, 4.2f });

        if (ImGui::BeginTable("Middle Table", 2))
        {
            ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 180.0f); // Default to 100.0f
            ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            {
                // SLIDERS & KNOBS
                ImGui::Dummy(ImVec2{ 0.0f, 35.0f });
                ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
                if (track->mute) ImGui::BeginDisabled();
                ImGui::VSliderFloat("-0", ImVec2{ 15, 155 }, &track->volume, 0.0f, 100.0f, "");
                if (ImGui::IsItemDeactivatedAfterEdit()) track->SetVolume();
                if (track->mute) ImGui::EndDisabled();
                ImGui::SameLine(0.0f, 20.0f);
                if (ImGui::BeginTable("Column Table", 2))
                {
                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
                    ImGui::Dummy(ImVec2{ 0.0f, 6.0f });

                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
                    ImGui::Dummy(ImVec2{ 4.5f, 0.0f }); ImGui::SameLine();
                    if (ImGui::Knob("L / R", &track->pan, -0.5, 0.5, false, mono, 2.5f, &track->knobReminder2) && mono)
                        track->SetPanning();

                    ImGui::Dummy(ImVec2{ 0.0f, 10.0f });

                    ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
                    if (ImGui::Knob("Transpose", &track->transpose, -24.0, 24.0, false, true, -10, &track->knobReminder1))
                        track->SetTranspose();
                }
                ImGui::EndTable();
            }

            ImGui::TableSetColumnIndex(1);
            {
                // VOLUME GRAPHIC
                float a[10] = { 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f };
                float width = ImGui::GetWindowWidth();
                ImGui::PushItemWidth(width - 220);
                ImGui::SliderFloat("##offset", &editorOffset, 0.0f, track->duration);
                ImGui::PopItemWidth();
                ImGui::PlotHistogram("##volumegraph", a, 10, 100.0f, "", 0.0f, 10.0f, ImVec2(width - 220, 100));
                ImGui::PlotHistogram("##volumegraph", a, 10, 0.0f, "", 10.0f, 0.0f, ImVec2(width - 220, 100), 4, true);
            }
            ImGui::EndTable();

        }

        ImGui::Dummy(ImVec2{ 6.f, 0.0f }); ImGui::SameLine();

        if (ImGui::BeginTable("Bottom Table", 2))
        {
            ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 170.0f); // Default to 100.0f
            ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableNextRow();
            int selectId = -1;

            // EFFECT SELECTOR
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::Text("Effects:");
                if (track->currEffect != 0)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Add"))
                    {
                        track->effects.push_back(track->CreateEffect(track->currEffect));
                        track->fxTracker.erase(track->fxTracker.begin() + track->currEffect);
                        track->currEffect--;
                        track->totalEffects--;
                    }
                }

                if (!track->effects.empty())
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Delete")) track->RemoveEffect();
                }

                ImGui::Combo("##Effects", &track->currEffect, &track->fxTracker[0], track->fxTracker.size());

                bool select = false;
                bool clicked = false;
                for (unsigned int i = 0; i < track->effects.size(); i++)
                {
                    Effect* e = track->effects[i];

                    if (i % 2 != 0) ImGui::SameLine();
                    bool click = ImGui::Selectable(e->GetName(), &e->selected, ImGuiSelectableFlags_None, ImVec2{ 85.0f, 12.0f });

                    if (click && e->selected)
                    {
                        selectId = i;
                        clicked = true;
                    }
                }

                for (unsigned int i = 0; selectId != -1 && clicked && i < track->effects.size(); i++)
                {
                    if (i == selectId) continue;
                    track->effects[i]->selected = false;
                }
            }

            // PARAMETER TWICK
            ImGui::TableSetColumnIndex(1);
            {
                for (unsigned int i = 0; i < track->effects.size(); i++)
                {
                    Effect* e = track->effects[i];
                    if (e->selected)
                    {
                        e->Draw();
                        break;
                    }
                }
            }
            ImGui::EndTable();

        }
    }
    ImGui::End();

    if (!openEditor)
    {
        StopAllTracks();
        trackIdInEdit = -1;
    }
}