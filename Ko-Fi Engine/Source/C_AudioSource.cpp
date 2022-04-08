#include "C_AudioSource.h"

#include "External/imgui/imgui_internal.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

#include "Globals.h"
#include "ImGuiAppLog.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "Effects.h"
#include "Importer.h"
#include "PanelChooser.h"

C_AudioSource::C_AudioSource(GameObject* parent) : C_Audio(parent)
{
    type = ComponentType::AUDIO_SOURCE;

    track = nullptr;

    openEditor = false;
    editorOffset = 0.0f;
}

C_AudioSource::~C_AudioSource()
{
    if(track != nullptr)
        StopAudio(track->source);

    RELEASE(track);
}

bool C_AudioSource::Start()
{

    return true;
}

bool C_AudioSource::OnPlay()
{
    if (track != nullptr)
    {
        StopAudio(track->source);

        if (track->playOnStart)
        {
            PlayAudio(track->source, track->offset);
        }
    }
    return true;
}

bool C_AudioSource::Update(float dt)
{
    bool ret = true;

    if (track != nullptr)
    {
        if (owner->GetEngine()->GetSceneManager()->GetGameState() == GameState::PAUSED)
        {
            PauseAudio(track->source);
            return true;
        }
        //ResumeAudio(track->source);
    }

    if (track != nullptr && track->IsTrackLoaded())
        UpdatePlayState();

    return ret;
}

bool C_AudioSource::InspectorDraw(PanelChooser* chooser)
{
    bool ret = true;

    if (openEditor && track != nullptr)
        DrawEditor();

    if (ImGui::CollapsingHeader("Audio Source"))
    {
        DrawDeleteButton(owner, this);

        if (chooser->IsReadyToClose("Add Track"))
        {
            if (chooser->OnChooserClosed() != nullptr)
            {
                std::string path = chooser->OnChooserClosed();

                if (!path.empty())
                {
                    if (track != nullptr)
                    {
                        openEditor = false;
                        editorOffset = 0.0f;
                        StopAudio(track->source);
                        RELEASE(track);
                    }

                    track = new R_Track();
                    Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
                    track->source = CreateAudioSource(track->buffer, false);

                    track->SetVolume();
                    track->SetPanning();
                    track->SetTranspose();
                }
            }
        }

        if (track != nullptr)
        {
            bool mono = track->channels == 1;

            ImGui::Spacing();
            if (ImGui::Button("Change Track"))
            {
                chooser->OpenPanel("Add Track", "wav", { "wav" });
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Track"))
            {
                openEditor = false;
                editorOffset = 0.0f;
                StopAudio(track->source);
                RELEASE(track);
                return ret;
            }
            ImGui::Spacing();

            ImGui::Text("Track Name: %s", track->GetTrackName());
            ImGui::Spacing();
            ImGui::Text("Sample Rate: %d kHz (%d-Bits)", track->sampleRate, track->bits);
            ImGui::Spacing();
            ImGui::Text("Channels: %d", track->channels);
            if (!mono)
            {
                ImGui::SameLine();
                ImGui::Text("(No Pan)");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            std::string action;
            track->play ? action = "Stop" : action = "Play";
            if (ImGui::Button(action.c_str()))
            {
                track->play ? StopAudio(track->source) : PlayAudio(track->source, track->offset);
            }
            ImGui::SameLine();
            if (ImGui::Button("Edit"))
            {
                openEditor = !openEditor;
            }
            ImGui::SameLine();
            ImGui::SliderFloat("Offset", &track->offset, 0.0f, track->duration);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Options");

            if (ImGui::Checkbox("Mute", &track->mute))
                track->SetVolume();

            ImGui::Checkbox("Play on start", &track->playOnStart);

            if (ImGui::Checkbox("Loop", &track->loop))
                track->SetLoop(track->loop);

            if (ImGui::Checkbox("Bypass SFX", &track->bypass))
            {
                for (uint i = 0; i < track->effects.size(); ++i)
                    track->effects[i]->ToggleBypass(!track->bypass);
            }

            ImGui::Spacing();
        }
        else
        {
            ImGui::Spacing();
            if (ImGui::Button("Add Track"))
            {
                chooser->OpenPanel("Add Track", "wav", { "wav" });
            }
            ImGui::Spacing();
        }
    }
    else
        DrawDeleteButton(owner, this);

    return ret;
}

void C_AudioSource::Save(Json& json) const
{
    json["type"] = "audio_source";

    if (track != nullptr)
    {
        json["state"] = true;
        json["track_path"] = track->GetTrackPath();
        json["mute"] = track->GetMute();
        json["play_on_start"] = track->GetPlayOnStart();
        json["loop"] = track->GetLoop();
        json["bypass"] = track->GetBypass();
        json["volume"] = track->GetVolume();
        json["pan"] = track->GetPan();
        json["transpose"] = track->GetTranspose();
        json["offset"] = track->GetOffset();
    }
    else
        json["state"] = false;
}

void C_AudioSource::Load(Json& json)
{
    openEditor = false;
    if (track != nullptr)
    {
        editorOffset = 0.0f;
        StopAudio(track->source);
        RELEASE(track);
    }

    bool state = json.at("state");
    if (state)
    {
        std::string path = json.at("track_path");
        track = new R_Track();
        Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
        track->source = CreateAudioSource(track->buffer, false);

        track->SetMute(json.at("mute"));
        track->SetPlayOnStart(json.at("play_on_start"));
        track->SetLoop(json.at("loop"));
        track->SetBypass(json.at("bypass"));
        track->SetVolume(json.at("volume"));
        track->SetPanning(json.at("pan"));
        track->SetTranspose(json.at("transpose"));
        track->SetOffset(json.at("offset"));
    }
}

void C_AudioSource::UpdatePlayState()
{
    ALint sourceState;
    alGetSourcei(track->source, AL_SOURCE_STATE, &sourceState);
    (sourceState == AL_PLAYING) ? track->play = true : track->play = false;
}

void C_AudioSource::DrawEditor()
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
                ImGui::Text("Track: %s", track->name.c_str());
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
                    float time = track->duration * editorOffset;
                    track->play ? StopAudio(track->source) : PlayAudio(track->source, time);
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
        StopAudio(track->source);
}

void C_AudioSource::PlayTrack()
{
    if (track != nullptr)
    {
        float time = track->duration * track->offset;
        PlayAudio(track->source, time);
    }
}

void C_AudioSource::ResumeTrack()
{
    if (track != nullptr)
    {
        ResumeAudio(track->source);
    }
}

void C_AudioSource::StopTrack()
{
    if (track != nullptr)
    {
        StopAudio(track->source);
    }
}

void C_AudioSource::PauseTrack()
{
    if (track != nullptr)
    {
        PauseAudio(track->source);
    }
}