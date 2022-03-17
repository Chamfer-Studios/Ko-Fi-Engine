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

    totalTracks = 1;

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
            if (switching) pauseDifference = owner->GetEngine()->GetSceneManager()->GetTotalGameTime() - switchTime;
            continue;
        }
        ResumeAudio(tracks[i]->source);
    }
    UpdatePlayState();
}

bool C_AudioSwitch::InspectorDraw(PanelChooser* chooser)
{
    return true;
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

}