#include "C_AudioSource.h"
#include "R_Track.h"
#include "SceneManager.h"
#include "Globals.h"
#include "GameObject.h"

#include "MathGeoLib/Math/MathFunc.h"

C_AudioSource::C_AudioSource(GameObject* parent) : C_Audio(parent)
{
    type = ComponentType::AUDIO_SOURCE;

    track = nullptr;

    volume = 100.0f;
    SetVolume(volume);

    pan = 0.0f;
    SetPanning(pan);

    transpose = 0.0f;
    SetTranspose(transpose);

    loop = false;
    SetLoop(loop);

    offset = 0.0f;

    play = false;
    playOnStart = true;

    mute = false;
    //bypass = false;
}

C_AudioSource::C_AudioSource(GameObject* parent, float volume, float pan, float transpose, bool mute, bool playOnStart, bool loop, bool bypass) : C_Audio(parent)
{
    this->volume = volume;
    SetVolume(volume);

    this->pan = pan;
    SetPanning(pan);

    this->transpose = transpose;
    SetTranspose(transpose);

    this->loop = loop;
    SetLoop(loop);

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

    if (playOnStart)
        PlayAudio(track->source);

    return true;
}

bool C_AudioSource::Update(float dt)
{
    bool ret = true;

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

    }

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