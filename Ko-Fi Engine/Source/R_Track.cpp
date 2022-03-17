#include "R_Track.h"

#include "MathGeoLib/Math/MathFunc.h"

R_Track::R_Track()
{
	volume = 100.0f;

	offset = 0.0f;

	loop = false;
	play = false;
	playOnStart = true;
	mute = false;

	//pan = 0.0f;
	//transpose = 0.0f;
	//bypass = false;

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

R_Track::R_Track(float volume, bool mute, bool playOnStart, bool loop)
{
	this->volume = volume;
	this->mute = mute;
	this->playOnStart = playOnStart;
	this->loop = loop;

	play = false;

	offset = 0.0f;

	//this->pan = pan;
	//this->transpose = transpose;
	//this->bypass = bypass;

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

R_Track::~R_Track()
{
	alec(alDeleteSources(1, &source));
	alec(alDeleteSources(1, &buffer));
}

uint64_t R_Track::GetTotalSamples() const
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

void R_Track::SetPCMFrameCount(drwav_uint64 value)
{
	switch (format)
	{
	case AudioFormat::WAV: totalPCMFrameCountWav = value; break;
	case AudioFormat::MP3: totalPCMFrameCountMp3 = (drmp3_uint64)value; break;
	case AudioFormat::FLAC: totalPCMFrameCountFlac = (drflac_uint64)value; break;
	}
}

void R_Track::SetLoop(bool active)
{
	alSourcei(source, AL_LOOPING, active);
}

// Range [0 - 100]
void R_Track::SetVolume(float volume)
{
	if (mute)
	{
		alSourcef(source, AL_GAIN, 0.0f);
		return;
	}

	volume = Pow(volume, 2.5f) / 1000.0f;

	if (volume > 99.0f)
		volume = 100.0f;

	alSourcef(source, AL_GAIN, volume / 100.0f);
}

//void C_AudioSource::SetPanning(float pan)
//{
//    alSource3f(track->source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
//}

//void C_AudioSource::SetTranspose(float transpose)
//{
//    transpose = exp(0.0577623f * transpose);
//
//    if (transpose > 4.0f)
//        transpose = 4.0f;
//
//    if (transpose < 0.25f)
//        transpose = 0.25f;
//
//    if (transpose > 0.98f && transpose < 1.02f)
//        transpose = 1.0f;
//
//    alSourcef(track->source, AL_PITCH, transpose);
//}