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

TrackInstance::TrackInstance()
{
    track = nullptr;

    play = false;
    playOnStart = false;
    loop = false;
    mute = false;
    volume = 100.0f;
    offset = 0.0f;
}

TrackInstance::TrackInstance(float volume, bool mute, bool playOnStart, bool loop, const char* path)
{
    track = nullptr;

    this->volume = volume;
    this->mute = mute;
    this->playOnStart = playOnStart;
    this->loop = loop;

    if (path != nullptr)
        this->track->path = path;
}

TrackInstance::~TrackInstance()
{
    RELEASE(track);
}

//void TrackInstance::Delete()
//{
//
//}

//////////////////////////////////////////////////////////////////
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
            StopAudio(tracks[i]->track->source);

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
            StopAudio(tracks[i]->track->source);
            if (tracks[i]->GetPlayOnStart())
            {
                playingTrack = tracks[i];
                PlayAudio(tracks[i]->track->source);
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
            PauseAudio(tracks[i]->track->source);
            //if (switching) pauseDifference = gameTimer->RealReadSec() - switchTime;
            continue;
        }
        ResumeAudio(tracks[i]->track->source);
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
            alGetSourcei(tracks[i]->track->source, AL_SOURCE_STATE, &sourceState);
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
            PlayAudio(newTrack->track->source);
            alSourcef(newTrack->track->source, AL_GAIN, 0.0f);
        }

        if (owner->GetEngine()->GetSceneManager()->GetState() == RuntimeState::PAUSED)
        {
           //switchTime = gameTimer->RealReadSec() - pauseDifference;
            return;
        }

        //float t = gameTimer->RealReadSec() - switchTime;
        //float d = fadeSeconds;
        //if (t > d)
        //{
        //    switching = false; // Stop source
        //    StopAudio(oldTrack->track.source);
        //    SetVolume(oldTrack->volume, oldTrack);
        //    SetVolume(newTrack->volume, newTrack);
        //    playingTrack = newTrack;
        //    oldTrack = nullptr;
        //    newTrack = nullptr;
        //    pauseDifference = 0;
        //}
        //else
        //{
        //    float volume = oldTrack->volume;
        //    volume = Pow(volume, 2.5f) / 1000.0f;
        //    if (volume > 99.0f) volume = 100.0f;
        //    volume = (volume / 100) * (1.0f - t / d);
        //    alSourcef(oldTrack->track.source, AL_GAIN, volume);

        //    float nvolume = newTrack->volume;
        //    nvolume = Pow(nvolume, 2.5f) / 1000.0f;
        //    if (nvolume > 99.0f) nvolume = 100.0f;
        //    nvolume = (nvolume / 100) * (t / d);
        //    alSourcef(newTrack->track.source, AL_GAIN, nvolume);
        //}
    }
}

void C_AudioSwitch::StopAllTracks()
{
    for (uint i = 0; i < tracks.size(); ++i)
    {
        if (tracks[i]->IsTrackLoaded())
            StopAudio(tracks[i]->track->source);
    }
}

void C_AudioSwitch::DisablePlayOnStart()
{

}