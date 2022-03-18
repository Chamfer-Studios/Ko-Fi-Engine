#include "C_AudioSwitch.h"
#include "SceneManager.h"
#include "Globals.h"
#include "GameObject.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "Importer.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

C_AudioSwitch::C_AudioSwitch(GameObject* parent) : C_Audio(parent)
{
	type = ComponentType::AUDIO_SWITCH;

    totalTracks = 0;

    switching = false;
    nextSwitchTrack = 1;

    fadeTime = 2.0f;
    switchTime = 0.0f;
    pauseDifference = 0.0f;

    oldTrack = nullptr;
    newTrack = nullptr;
    playingTrack = nullptr;
}

C_AudioSwitch::C_AudioSwitch(GameObject* parent, int totalTracks, int nextSwitchTrack, float fadeTime) : C_Audio(parent)
{
    type = ComponentType::AUDIO_SWITCH;

    this->totalTracks = totalTracks;
    this->nextSwitchTrack = nextSwitchTrack;
    this->fadeTime = fadeTime;

    switching = false;
    switchTime = 0.0f;
    pauseDifference = 0.0f;

    oldTrack = nullptr;
    newTrack = nullptr;
    playingTrack = nullptr;
}

C_AudioSwitch::~C_AudioSwitch()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->IsTrackLoaded())
            StopAudio(tracks[i]->source);

        RELEASE(tracks[i]);
    }

    tracks.clear();
    tracks.shrink_to_fit();
}

bool C_AudioSwitch::Start()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->IsTrackLoaded())
        {
            StopAudio(tracks[i]->source);
            if (tracks[i]->GetPlayOnStart())
            {
                playingTrack = tracks[i];
                PlayAudio(tracks[i]->source);
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

    SwitchFade(fadeTime);

    if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::STOPPED)
        return ret;

    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PAUSED)
        {
            PauseAudio(tracks[i]->source);
            if (switching)
                pauseDifference = owner->GetEngine()->GetSceneManager()->GetTotalGameTime() - switchTime;

            continue;
        }
        ResumeAudio(tracks[i]->source);
    }
    UpdatePlayState();
}

bool C_AudioSwitch::InspectorDraw(PanelChooser* chooser)
{
    bool ret = true;

    if (ImGui::CollapsingHeader("Audio Switch"))
    {
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

                    track->SetVolume(track->volume);
                    track->SetLoop(track->loop);
                    //SetPanning(pan);
                    //SetTranspose(transpose);
                    tracks.push_back(track);
                    ++totalTracks;
                }
            }
        }

        if (ImGui::BeginTable("SwitchAudioSourceComp", 2))
        {
            ImGui::TableSetupColumn("SAST10", ImGuiTableColumnFlags_WidthFixed, 2);
            ImGui::TableSetupColumn("SAST11");

            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TableSetColumnIndex(1);

            // SWITCH BUTTON
            if (ImGui::Button("Switch To") && !switching)
            {
                SwitchTrack(nextSwitchTrack - 1);
            }
            ImGui::SameLine();
            ImGui::DragInt("##Switch", &nextSwitchTrack, 1, 1, tracks.size(), "Track %d");

            ImGui::Spacing();

            ImGui::Text("Fade Time "); ImGui::SameLine(73);
            ImGui::SameLine(87);

            ImGui::DragFloat("##FadeTime", &fadeTime, 0.1f, 0.1f, 10.0f, "%.2f sec");

            ImGui::Spacing();
            ImGui::Spacing();

            // STOP TRACK
            if (IsAnyTrackPlaying())
            {
                if (ImGui::Button("Stop Track"))
                {
                    StopAllTracks();
                    playingTrack = nullptr;
                }

                ImGui::Spacing();
                ImGui::Spacing();
            }

            // TRACK LIST
            ImGui::PushItemWidth(80);
            if (ImGui::Button("Add Track"))
            {
                chooser->OpenPanel("Add Track", "wav");
            }
            ImGui::PopItemWidth();

            ImGui::Spacing();
            for (uint i = 0; i < tracks.size(); ++i)
            {
                ImGui::PushID(i + 50);

                R_Track* index = tracks[i];
                if (tracks.size() >= 1)
                {
                    if (ImGui::Button("X"))
                    {
                        totalTracks--;

                        if (nextSwitchTrack > 1)
                            nextSwitchTrack--;

                        StopAudio(index->source);
                        RELEASE(index);
                        tracks.erase(tracks.begin() + i);

                        ImGui::PopID();
                        ImGui::SameLine();
                        continue;
                    }
                    ImGui::SameLine();
                }

                std::string trackName = trackNaming[i];
                if (!index->IsTrackLoaded()) // If track isn't loaded
                {
                    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.7f, 0, 0, 0.2f });
                    trackName += " (no audio)";
                }

                if (ImGui::CollapsingHeader(trackName.c_str(), ImGuiTreeNodeFlags_Framed))
                {
                    if (ImGui::BeginTable("TrackInsights", 2))
                    {
                        ImGui::TableSetupColumn("SAST20", ImGuiTableColumnFlags_WidthFixed, 3);
                        ImGui::TableSetupColumn("SAST21");

                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::TableSetColumnIndex(1);

                        if (index->IsTrackLoaded()) // If track is loaded
                        {
                            ImGui::SameLine();
                            ImGui::Spacing();
                            ImGui::Text("Options");

                            if (ImGui::Checkbox("Mute", &index->mute))
                                tracks[i]->SetVolume(index->volume);

                            if (ImGui::Checkbox("Play on start", &index->playOnStart) && index->playOnStart)
                            {
                                DisablePlayOnStart();
                                index->playOnStart = true;
                            }

                            if (ImGui::Checkbox("Loop", &index->loop))
                                tracks[i]->SetLoop(index->loop);
                        }
                    }
                    ImGui::EndTable();
                }
                if (!index->IsTrackLoaded())
                    ImGui::PopStyleColor(); // False

                ImGui::PopID();
            }
        }
        ImGui::EndTable();

        UpdatePlayState();
    }

    return ret;
}

void C_AudioSwitch::UpdatePlayState()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->IsTrackLoaded())
        {
            ALint sourceState;
            alGetSourcei(tracks[i]->source, AL_SOURCE_STATE, &sourceState);
            (sourceState == AL_PLAYING) ? tracks[i]->play = true : tracks[i]->play = false;
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

    if (oldTrack != nullptr && !newTrack->play && newTrack->IsTrackLoaded())
    {
        switching = true;
        switchTime = owner->GetEngine()->GetSceneManager()->GetTotalGameTime();
    }
}

void C_AudioSwitch::SwitchFade(float fadeSeconds)
{
    if (switching)
    {
        if (!newTrack->play)
        {
            PlayAudio(newTrack->source);
            alSourcef(newTrack->source, AL_GAIN, 0.0f);
        }

        if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PAUSED)
        {
            switchTime = owner->GetEngine()->GetSceneManager()->GetTotalGameTime() - pauseDifference;
            return;
        }

        float t = owner->GetEngine()->GetSceneManager()->GetTotalGameTime() - switchTime;
        float d = fadeSeconds;
        if (t > d)
        {
            switching = false; // Stop source
            StopAudio(oldTrack->source);
            oldTrack->SetVolume(oldTrack->volume);

            newTrack->SetVolume(newTrack->volume);

            playingTrack = newTrack;

            oldTrack = nullptr;
            newTrack = nullptr;
            pauseDifference = 0;
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
}

bool C_AudioSwitch::IsAnyTrackPlaying() const
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (!tracks[i]->IsTrackLoaded())
            continue;

        return tracks[i]->play;
    }
    return false;
}

R_Track* C_AudioSwitch::GetPlayingTrack() const
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->play)
            return tracks[i];
    }
    return nullptr;
}

void C_AudioSwitch::StopAllTracks()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->IsTrackLoaded())
            StopAudio(tracks[i]->source);
    }
}

void C_AudioSwitch::DisablePlayOnStart()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (!tracks[i]->IsTrackLoaded())
            continue;

        tracks[i]->playOnStart = false;
    }
}