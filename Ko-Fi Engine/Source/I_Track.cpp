#include "I_Track.h"

#include "Engine.h"
#include "M_FileSystem.h"

#include "R_Track.h"

#include "al.h"
#include "alc.h"
#include "alext.h"
#include "efx.h"
#include "efx-creative.h"
#include "efx-presets.h"

#include "dr_wav.h"
#include "dr_mp3.h"
#include "dr_flac.h"

#include "Log.h"

#include <filesystem>

I_Track::I_Track(KoFiEngine* engine) : engine(engine)
{
}

I_Track::~I_Track()
{
}

bool I_Track::Import(const char* path, R_Track* track)
{
    bool ret = true;

    track->path = path;
    track->name = engine->GetFileSystem()->GetNameFromPath(path);

    std::filesystem::path filePath = path;
    if (filePath.extension() == ".mp3" || filePath.extension() == ".MP3")
    {
        ret = LoadMP3(path, track);
    }
    else if (filePath.extension() == ".wav" || filePath.extension() == ".WAV")
    {
        ret = LoadWav(path, track);
    }
    else if (filePath.extension() == ".flac" || filePath.extension() == ".FLAC")
    {
        ret = LoadFlac(path, track);
    }
    return ret;
}

bool I_Track::LoadMP3(const char* path, R_Track* track)
{
    track->format = AudioFormat::MP3;

    drmp3_uint64 totalPCMFrameCount = 0;
    drmp3_config mp3Config;

    drmp3_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(path, &mp3Config, &totalPCMFrameCount, nullptr);
    track->SetPCMFrameCount(totalPCMFrameCount);
    track->channels = mp3Config.channels;
    track->sampleRate = mp3Config.sampleRate;

    if (pSampleData == NULL)
    {
        KOFI_ERROR(": Failed to load audio file.");
        return false;
    }

    track->bits = 16;

    if (track->GetTotalSamples() > drmp3_uint64(std::numeric_limits<size_t>::max()))
    {
        KOFI_ERROR(": Too much data in file for 32bit addressed vector.");
        return false;
    }

    track->pcmData.resize(size_t(track->GetTotalSamples()));
    std::memcpy(track->pcmData.data(), pSampleData, track->pcmData.size() * 2);
    drmp3_free(pSampleData, nullptr);

    ALuint buffer;
    alec(alGenBuffers(1, &buffer));
    alec(alBufferData(buffer, track->channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track->pcmData.data(), track->pcmData.size() * 2 /*two bytes per sample*/, track->sampleRate));

    ALint bufferSize;
    alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
    track->duration = bufferSize / (track->sampleRate * track->channels * 2); //(2 bytes == 16 Bits)

    track->buffer = buffer;

    return true;
}

bool I_Track::LoadWav(const char* path, R_Track* track)
{
    track->format = AudioFormat::WAV;

    drwav_uint64 totalPCMFrameCount = 0;

    drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(path, &track->channels, &track->sampleRate, &totalPCMFrameCount, nullptr);
    track->SetPCMFrameCount(totalPCMFrameCount);

    if (pSampleData == NULL)
    {
        KOFI_ERROR(": Failed to load audio file.");
        return false;
    }

    track->bits = 16;

    if (track->GetTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
    {
        KOFI_ERROR(": Too much data in file for 32bit addressed vector.");
        return false;
    }

    track->pcmData.resize(size_t(track->GetTotalSamples()));
    std::memcpy(track->pcmData.data(), pSampleData, track->pcmData.size() * /*twobytes_in_s15*/2);
    drwav_free(pSampleData, nullptr);

    ALuint buffer;
    alec(alGenBuffers(1, &buffer));
    alec(alBufferData(buffer, track->channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track->pcmData.data(), track->pcmData.size() * 2 /*two bytes per sample*/, track->sampleRate));

    ALint bufferSize;
    alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
    track->duration = (float)bufferSize / (track->sampleRate * track->channels * 2); //(2 bytes == 16 Bits)

    track->buffer = buffer;

    return true;
}

bool I_Track::LoadFlac(const char* path, R_Track* track)
{
    track->format = AudioFormat::FLAC;

    drwav_uint64 totalPCMFrameCount = 0;

    drflac_int16* pSampleData = drflac_open_file_and_read_pcm_frames_s16(path, &track->channels, &track->sampleRate, &totalPCMFrameCount, nullptr);
    track->SetPCMFrameCount(totalPCMFrameCount);

    if (pSampleData == NULL)
    {
        KOFI_ERROR(": Failed to load audio file.");
        return false;
    }

    track->bits = 16;

    if (track->GetTotalSamples() > drflac_uint64(std::numeric_limits<size_t>::max()))
    {
        KOFI_ERROR(": Too much data in file for 32bit addressed vector.");
        return false;
    }

    track->pcmData.resize(size_t(track->GetTotalSamples()));
    std::memcpy(track->pcmData.data(), pSampleData, track->pcmData.size() * /*twobytes_in_s15*/2);
    drflac_free(pSampleData, nullptr);

    ALuint buffer;
    alec(alGenBuffers(1, &buffer));
    alec(alBufferData(buffer, track->channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track->pcmData.data(), track->pcmData.size() * 2 /*two bytes per sample*/, track->sampleRate));

    ALint bufferSize;
    alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
    track->duration = bufferSize / (track->sampleRate * track->channels * 2); //(2 bytes == 16 Bits)

    track->buffer = buffer;

    return true;
}