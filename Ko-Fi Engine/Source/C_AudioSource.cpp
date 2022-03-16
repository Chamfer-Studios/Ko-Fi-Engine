#include "C_AudioSource.h"
#include "R_Track.h"
#include "SceneManager.h"
#include "Globals.h"
#include "GameObject.h"
#include "ImGuiAppLog.h"
#include "PanelChooser.h"
#include "Importer.h"
#include "MathGeoLib/Math/MathFunc.h"

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

    volume = 100.0f;
    pan = 0.0f;
    transpose = 0.0f;

    loop = false;

    offset = 0.0f;

    play = false;
    playOnStart = true;

    mute = false;
    //bypass = false;
}

C_AudioSource::C_AudioSource(GameObject* parent, float volume, float pan, float transpose, bool mute, bool playOnStart, bool loop, bool bypass) : C_Audio(parent)
{
    track = nullptr;

    this->volume = volume;
    this->pan = pan;
    this->transpose = transpose;

    this->loop = loop;

    offset = 0.0f;

    play = false;
    playOnStart = true;

    mute = false;
    bypass = false;
}

C_AudioSource::~C_AudioSource()
{
    StopAudio(track->source);

    RELEASE(track);
}

bool C_AudioSource::Start()
{
    StopAudio(track->source);

    return true;
}

bool C_AudioSource::Update(float dt)
{
    bool ret = true;

    if (playOnStart && track != nullptr && owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PLAYING)
    {
        PlayAudio(track->source);
        playOnStart = false;
    }

    if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::STOPPED)
        return true;

    if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PAUSED)
    {
        PauseAudio(track->source);
        return true;
    }

    ResumeAudio(track->source);

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
                    track = new R_Track();
                    Importer::GetInstance()->trackImporter->Import(path.c_str(), track);
                    track->source = CreateAudioSource(track->buffer, false);

                    SetVolume(volume);
                    SetPanning(pan);
                    SetTranspose(transpose);
                }
            }
        }

        if (track != nullptr)
        {
            bool mono = track->channels == 1;

            ImGui::Text("Track Name: %s", track->name.c_str());
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

            if (ImGui::Checkbox("Mute", &mute))
                SetVolume(volume);

            ImGui::Checkbox("Play on start", &playOnStart);

            if (ImGui::Checkbox("Loop", &loop))
                SetLoop(loop);

            ImGui::Spacing();
            ImGui::Spacing();

            std::string action;
            play ? action = "Stop" : action = "Play";
            if (ImGui::Button(action.c_str()))
            {
                float time = track->duration * offset;
                play ? StopAudio(track->source) : PlayAudio(track->source, time);
            }

            ImGui::SliderFloat("Offset", &offset, 0.0f, 1.0f);
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

// Range [0 - 100]
void C_AudioSource::SetVolume(float volume)
{
    if (mute)
    {
        alSourcef(track->source, AL_GAIN, 0);
        return;
    }

    volume = Pow(volume, 2.5f) / 1000.0f;

    if (volume > 99.0f)
        volume = 100.0f;

    alSourcef(track->source, AL_GAIN, volume / 100);
}

void C_AudioSource::SetPanning(float pan)
{
    alSource3f(track->source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
}

void C_AudioSource::SetTranspose(float transpose)
{
    transpose = exp(0.0577623f * transpose);

    if (transpose > 4.0f)
        transpose = 4.0f;

    if (transpose < 0.25f)
        transpose = 0.25f;

    if (transpose > 0.98f && transpose < 1.02f)
        transpose = 1.0f;

    alSourcef(track->source, AL_PITCH, transpose);
}

void C_AudioSource::SetLoop(bool active)
{
    alSourcei(track->source, AL_LOOPING, active);
}

void C_AudioSource::UpdatePlayState()
{
    ALint sourceState;
    alGetSourcei(track->source, AL_SOURCE_STATE, &sourceState);
    (sourceState == AL_PLAYING) ? play = true : play = false;
}