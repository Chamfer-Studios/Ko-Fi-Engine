#ifndef __C_AUDIO_H__
#define __C_AUDIO_H__

#include "R_Track.h"

#include "al.h"
#include "alc.h"
#include "alext.h"

#include "Component.h"

#include <iostream>

//using Json = nlohmann::json;

#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if( error != AL_NO_ERROR)\
	{\
		std::cerr << "OpenAL Error: " << error << " with call for " << #message << std::endl;\
	}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

class GameObject;

class C_Audio : public Component
{
public:
    C_Audio(GameObject* parent) : Component(parent), editorOffset(0.0f), openEditor(false) {}
    ~C_Audio() {}

    virtual bool Start() { return true; }
    virtual bool Update(float dt) { return true; }
    virtual bool InspectorDraw(PanelChooser* chooser) { return true; }

    virtual void Save(Json& json) const {}
    virtual void Load(Json& json) {}

    ALuint CreateAudioSource(ALuint audioBuffer, bool spacial)
    {
        ALuint source;
        alec(alGenSources(1, &source));
        alec(alSourcef(source, AL_PITCH, 1.f));
        alec(alSourcef(source, AL_GAIN, 1.f));
        alec(alSourcei(source, AL_LOOPING, AL_FALSE));

        alec(alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED));

        // Velocitat que es reduirà el volum quan la pos > AL_MAX_DISTANCE
        alSourcef(source, AL_ROLLOFF_FACTOR, 10.0f);
        // AL_TRUE = pos relativa al listener | AL_FALSE = pos relativa a general coords (for spacial)
        alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

        alec(alSource3f, source, AL_POSITION, 0, 0, 0);
        alec(alSource3f, source, AL_VELOCITY, 0, 0, 0);

        alec(alSourcei(source, AL_BUFFER, audioBuffer));

        return source;
    }

    void PlayAudio(ALuint audioSource, float time = 0.0f)
    {
        ALint sourceState;
        alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

        if (sourceState == AL_STOPPED || sourceState == AL_INITIAL)
        {
            if (time != 0.0f)
                alSourcef(audioSource, AL_SEC_OFFSET, time);

            alec(alSourcePlay(audioSource));
        }
    }

    void PlayAudio(R_Track* track, float time = 0.0f)
    {
        ALint sourceState;
        alec(alGetSourcei(track->source, AL_SOURCE_STATE, &sourceState));

        if (time >= track->duration)
            time = 0.0f;

        if (sourceState == AL_STOPPED || sourceState == AL_INITIAL)
        {
            if (time != 0.0f)
                alSourcef(track->source, AL_SEC_OFFSET, time);

            alec(alSourcePlay(track->source));
        }
    }

    void ResumeAudio(ALuint audioSource)
    {
        ALint sourceState;
        alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

        if (sourceState == AL_PAUSED)
            alec(alSourcePlay(audioSource));
    }

    void StopAudio(ALuint audioSource)
    {
        ALint sourceState;
        alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

        if (sourceState == AL_PLAYING || sourceState == AL_PAUSED)
            alec(alSourceStop(audioSource));
    }

    void PauseAudio(ALuint audioSource)
    {
        ALint sourceState;
        alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

        if (sourceState == AL_PLAYING)
            alec(alSourcePause(audioSource));
    }

public:
    float editorOffset = 0.0f;
    bool openEditor = false;
};

#endif // !__C_AUDIO_H__