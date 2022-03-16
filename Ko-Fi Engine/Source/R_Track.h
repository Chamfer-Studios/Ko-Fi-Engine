#ifndef __R_TRACK_H__
#define __R_TRACK_H__

#include "al.h"
#include "alc.h"
#include "efx.h"
#include "efx-creative.h"

#include <dr_wav.h>
#include <dr_mp3.h>
#include <dr_flac.h>

#include <iostream>
#include <vector>
#include <string>

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

class R_Track
{
public:
	R_Track()
	{
		channels = 0;
		sampleRate = 44100;
		bits = 16;
		duration = 0.0f;
		format = AudioFormat::NONE;
		buffer = 0;
		source = 0;

		totalPCMFrameCountWav = 0;
		totalPCMFrameCountMp3 = 0;
		totalPCMFrameCountFlac = 0;
	}

	~R_Track()
	{
		alec(alDeleteSources(1, &source));
		alec(alDeleteSources(1, &buffer));
	}

	inline const char* GetTrackPath() const { return path.c_str(); }
	inline const char* GetTrackName() const { return name.c_str(); }

    // Methods
	uint64_t GetTotalSamples() const
	{
		uint64_t ret = 0;

		switch (format)
		{
		case AudioFormat::WAV: ret = totalPCMFrameCountWav * channels; break;
		case AudioFormat::MP3: ret = totalPCMFrameCountMp3 * channels; break;
		case AudioFormat::FLAC: ret = totalPCMFrameCountFlac * channels; break;
		}
		return ret;
	}

	void SetPCMFrameCount(drwav_uint64 value)
	{
		switch (format)
		{
		case AudioFormat::WAV: totalPCMFrameCountWav = value; break;
		case AudioFormat::MP3: totalPCMFrameCountMp3 = (drmp3_uint64)value; break;
		case AudioFormat::FLAC: totalPCMFrameCountFlac = (drflac_uint64)value; break;
		}
	}

public:
	unsigned int channels = 0;
	unsigned int sampleRate = 44100;
	unsigned int bits = 16;
	float duration;

	AudioFormat format;

	ALuint buffer;
	ALuint source;

	std::vector<uint16_t> pcmData;

	std::string path;
	std::string name;

private:
	drwav_uint64 totalPCMFrameCountWav = 0;
	drmp3_uint64 totalPCMFrameCountMp3 = 0;
	drflac_uint64 totalPCMFrameCountFlac = 0;
};

#endif // !__R_TRACK_H__