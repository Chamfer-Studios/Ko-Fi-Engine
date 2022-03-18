#include "C_AudioSource.h"
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

C_AudioSource::C_AudioSource(GameObject* parent) : C_Audio(parent)
{
    type = ComponentType::AUDIO_SOURCE;

    track = nullptr;
}

C_AudioSource::~C_AudioSource()
{
    StopAudio(track->source);

    RELEASE(track);
}

bool C_AudioSource::Start()
{
    if (track != nullptr)
    {
        StopAudio(track->source);

        if (track->playOnStart)
            PlayAudio(track->source);
    }

    return true;
}

bool C_AudioSource::Update(float dt)
{
    bool ret = true;

    if (track != nullptr)
    {
        if (track->playOnStart && owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PLAYING)
        {
            PlayAudio(track->source);
            track->playOnStart = false;
        }

        if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PAUSED)
        {
            PauseAudio(track->source);
            return true;
        }

        ResumeAudio(track->source);
    }

    if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::STOPPED)
        return true;

    return true;
}

bool C_AudioSource::InspectorDraw(PanelChooser* chooser)
{
    bool ret = true;

    if (ImGui::CollapsingHeader("Audio Source"))
    {
        if (chooser->IsReadyToClose("Add Track"))
        {
            if (chooser->OnChooserClosed() != nullptr)
            {
                std::string path = chooser->OnChooserClosed();

                if (!path.empty())
                {
                    if (track != nullptr)
                        RELEASE(track);

                    track = new R_Track();
                    Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
                    track->source = CreateAudioSource(track->buffer, false);

                    track->SetVolume(track->volume);
                    //SetPanning(pan);
                    //SetTranspose(transpose);
                }
            }
        }

        if (track != nullptr)
        {
            bool mono = track->channels == 1;

            if (ImGui::Button("Change Track"))
            {
                chooser->OpenPanel("Add Track", "wav");
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete Track"))
            {
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

            ImGui::Separator();

            ImGui::SameLine();
            ImGui::Spacing();
            ImGui::Text("Options");

            if (ImGui::Checkbox("Mute", &track->mute))
                track->SetVolume(track->volume);

            ImGui::Checkbox("Play on start", &track->playOnStart);

            if (ImGui::Checkbox("Loop", &track->loop))
                track->SetLoop(track->loop);

            ImGui::Spacing();

            std::string action;
            track->play ? action = "Stop" : action = "Play";
            if (ImGui::Button(action.c_str()))
            {
                float time = track->duration * track->offset;
                track->play ? StopAudio(track->source) : PlayAudio(track->source, time);
            }

            ImGui::SliderFloat("Offset", &track->offset, 0.0f, 1.0f);
        }
        else
        {
            if (ImGui::Button("Add Track"))
            {
                chooser->OpenPanel("Add Track", "wav");
            }
        }
    }

    if (track != nullptr)
        UpdatePlayState();

    return ret;
}

void C_AudioSource::UpdatePlayState()
{
    ALint sourceState;
    alGetSourcei(track->source, AL_SOURCE_STATE, &sourceState);
    (sourceState == AL_PLAYING) ? track->play = true : track->play = false;
}