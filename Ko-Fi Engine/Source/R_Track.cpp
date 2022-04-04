#include "R_Track.h"
#include "Effects.h"

#include "MathGeoLib/Math/MathFunc.h"

R_Track::R_Track()
{
	volume = 100.0f;

	offset = 0.0f;

	loop = false;
	play = false;
	playOnStart = false;
	mute = false;

	pan = 0.0f;
	transpose = 0.0f;
	bypass = false;
	knobReminder1 = false;
	knobReminder2 = false;

	currEffect = 0;
	totalEffects = 6;

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
	CleanUp();
}

void R_Track::CleanUp()
{
	path.clear();
	path.shrink_to_fit();

	name.clear();
	name.shrink_to_fit();

	for (unsigned int i = 0; i < effects.size(); ++i)
	{
		effects[i]->Disconnect(source);
		delete effects[i];
	}
	effects.clear();
	effects.shrink_to_fit();

	fxTracker.clear();
	fxTracker.shrink_to_fit();

	alSourcei(source, AL_BUFFER, 0);
	alec(alDeleteSources(1, &source));
	alec(alDeleteBuffers(1, &buffer));
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
	loop = active;
	alSourcei(source, AL_LOOPING, active);
}

// Range [0 - 100]
void R_Track::SetVolume()
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

void R_Track::SetPanning()
{
    alSource3f(source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
}
void R_Track::SetPanning(float pan)
{
	alSource3f(source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
}

void R_Track::SetTranspose()
{
    transpose = exp(0.0577623f * transpose);

    if (transpose > 4.0f)
        transpose = 4.0f;

    if (transpose < 0.25f)
        transpose = 0.25f;

    if (transpose > 0.98f && transpose < 1.02f)
        transpose = 1.0f;

    alSourcef(source, AL_PITCH, transpose);
}
void R_Track::SetTranspose(float transpose)
{
	transpose = exp(0.0577623f * transpose);

	if (transpose > 4.0f)
		transpose = 4.0f;

	if (transpose < 0.25f)
		transpose = 0.25f;

	if (transpose > 0.98f && transpose < 1.02f)
		transpose = 1.0f;

	alSourcef(source, AL_PITCH, transpose);
}

int R_Track::GetEffectNameId(std::string eName) const
{
	if ("Reverb" == eName) return 1;
	else if ("Distortion" == eName) return 2;
	else if ("Flanger" == eName) return 3;
	else if ("Delay" == eName) return 4;
	else if ("Chorus" == eName) return 5;

	return -1;
}

const char* R_Track::ReturnWrittenName(std::string eName) const
{
	if ("Reverb" == eName) return "Reverb";
	else if ("Distortion" == eName) return "Distortion";
	else if ("Flanger" == eName) return "Flanger";
	else if ("Delay" == eName) return "Delay";
	else if ("Chorus" == eName) return "Chorus";

	return "";
}

Effect* R_Track::CreateEffect(int effect)
{
	Effect* e = nullptr;

	const char* eName = fxTracker[effect];

	if ("Reverb" == eName) e = new Reverb(source, bypass);
	else if ("Distortion" == eName) e = new Distortion(source, bypass);
	else if ("Flanger" == eName) e = new Flanger(source, bypass);
	else if ("Delay" == eName) e = new Delay(source, bypass);
	else if ("Chorus" == eName) e = new Chorus(source, bypass);

	return e;
}

void R_Track::RemoveEffect()
{
	for (unsigned int i = 0; i < effects.size(); ++i)
	{
		if (effects[i]->selected)
		{
			Effect* e = effects[i];
			if (fxTracker.size() == 1) fxTracker.push_back(ReturnWrittenName(e->GetName()));
			else
			{
				int delId = GetEffectNameId(e->GetName());
				bool inserted = false;
				for (unsigned int a = 1; a < fxTracker.size(); a++)
				{
					if (delId < GetEffectNameId(fxTracker[a]))
					{
						fxTracker.insert(fxTracker.begin() + a, ReturnWrittenName(e->GetName()));
						inserted = true;
						break;
					}
				}
				if (!inserted) fxTracker.push_back(ReturnWrittenName(e->GetName()));
			}
			effects[i]->Disconnect(source);
			delete effects[i];
			effects.erase(effects.begin() + i);
			break;
		}
	}
}