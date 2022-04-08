#ifndef __R_TRACK_H__
#define __R_TRACK_H__

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"
#include "Resource.h"

#include <dr_wav.h>
#include <dr_mp3.h>
#include <dr_flac.h>

#include <iostream>

class Effect;

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

enum class AudioFormat
{
	NONE,
	MP3,
	WAV,
	FLAC
};

class R_Track : public Resource
{
public:
	R_Track();
	~R_Track();

	uint64_t GetTotalSamples() const;
	void SetPCMFrameCount(drwav_uint64 value);

	inline bool IsTrackLoaded() { return channels != 0; }

	inline void SetTrackPath(const char* path) { this->path = path; }
	inline const char* GetTrackPath() const { return path.c_str(); }
	inline void SetTrackName(const char* name) { this->name = name; }
	inline const char* GetTrackName() const { return name.c_str(); }

	inline void SetPlayOnStart(bool playOnStart) { this->playOnStart = playOnStart; }
	inline bool GetPlayOnStart() const { return playOnStart; }

	inline void SetMute(bool mute) { this->mute = mute; }
	inline bool GetMute() const { return mute; }

	void SetLoop(bool active);
	inline bool GetLoop() const { return loop; }

	void SetVolume();
	void SetVolume(float volume);
	inline float GetVolume() const { return volume; }

	inline void SetBypass(bool bypass) { this->bypass = bypass; }
	inline bool GetBypass() const { return bypass; }

	void SetPanning();
	void SetPanning(float pan);
	inline float GetPan() const { return pan; }

	void SetTranspose();
	void SetTranspose(float transpose);
	inline float GetTranspose() const { return transpose; }

	inline void SetOffset(float offset) { this->offset = offset; }
	inline float GetOffset() const { return offset; }

	int GetEffectNameId(std::string eName) const;
	const char* ReturnWrittenName(std::string eName) const;

	Effect* CreateEffect(int effect);
	void RemoveEffect();

public:
	std::string path;
	std::string name;

	float volume = 100.0f, offset = 0.0f;

	bool play = false;

	bool playOnStart = false, loop = false, mute = false;

	bool knobReminder1 = false, knobReminder2 = false;
	float pan = 0.0f, transpose = 0.0f; 
	bool bypass = false;

	std::vector<Effect*> effects;
	int currEffect = 0;
	unsigned int totalEffects = 6;
	const char* fxNames[6] = { "-----", "Reverb", "Distortion", "Flanger", "Delay", "Chorus" };
	std::vector<const char*> fxTracker = { "-----", "Reverb", "Distortion", "Flanger", "Delay", "Chorus" };

	unsigned int channels = 0;
	unsigned int sampleRate = 44100;
	unsigned int bits = 16;
	float duration;

	AudioFormat format;

	ALuint buffer;
	ALuint source;

	std::vector<uint16_t> pcmData;

private:
	drwav_uint64 totalPCMFrameCountWav = 0;
	drmp3_uint64 totalPCMFrameCountMp3 = 0;
	drflac_uint64 totalPCMFrameCountFlac = 0;
};

#endif // !__R_TRACK_H__