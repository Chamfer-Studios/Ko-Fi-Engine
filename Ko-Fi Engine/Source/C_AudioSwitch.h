#ifndef __C_AUDIO_SWITCH_H__
#define __C_AUDIO_SWITCH_H__

#include "C_Audio.h"
#include "R_Track.h"

using Json = nlohmann::json;

class GameObject;

class TrackInstance
{
public:

    TrackInstance();
    TrackInstance(float volume, bool mute, bool playOnStart, bool loop, const char* path = nullptr);
    ~TrackInstance();

    //void Delete();

    inline void SetTrack(R_Track* track) { this->track = track; }

    inline bool GetPlayOnStart() const { return playOnStart; }
    inline bool GetMute() const { return mute; }
    inline bool GetLoop() const { return loop; }
    inline float GetVolume() const { return volume; }

    inline bool IsTrackLoaded() { return (track->channels != 0); }

    //inline bool GetBypass() const { return bypass; }

    //inline float GetPan() const { return pan; }

    //inline float GetTranspose() const { return transpose; }

public:
    R_Track* track;

    bool play = false;

    bool playOnStart = false, loop = false, mute = false;

    float volume = 100.0f, offset = 0.0f;

    //float pan = 0.0f, transpose = 0.0f;
    //bool bypass = false;

    //bool knobReminder1 = false, knobReminder2 = false;
};

class C_AudioSwitch : public C_Audio
{
public:
    C_AudioSwitch(GameObject* parent);
    C_AudioSwitch(GameObject* parent, int totalTracks, int nextSwitchTrack,float fadeTime);
    ~C_AudioSwitch();

    bool Start() override;
    bool Update(float dt) override;
    bool InspectorDraw(PanelChooser* chooser) override;

    void UpdatePlayState();

    void SwitchFade(float fadeSeconds);

    void StopAllTracks();

    void DisablePlayOnStart();

private:
    int totalTracks = 1;

    bool switching = false;
    int nextSwitchTrack = 1;

    float fadeTime = 2.0f;
    float switchTime = 0.0f;
    float pauseDifference = 0.0f;

    TrackInstance* oldTrack = nullptr;
    TrackInstance* newTrack = nullptr;
    TrackInstance* playingTrack = nullptr;

    std::vector<TrackInstance*> tracks;
};

#endif // !__C_AUDIO_SWITCH_H__