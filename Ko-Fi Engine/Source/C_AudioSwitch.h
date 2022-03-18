#ifndef __C_AUDIO_SWITCH_H__
#define __C_AUDIO_SWITCH_H__

#include "C_Audio.h"
#include "R_Track.h"

using Json = nlohmann::json;

class GameObject;

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

    void SwitchTrack(int newTrackIndex);
    void SwitchFade(float fadeSeconds);

    bool IsAnyTrackPlaying() const;
    R_Track* GetPlayingTrack() const;

    void StopAllTracks();
    void DisablePlayOnStart();

    inline int GetTotalTracks() const { return totalTracks; }
    inline int GetNextSwitchTrack() const { return nextSwitchTrack; }
    inline float GetFadeTime() const { return fadeTime; }

private:
    int totalTracks = 0;

    bool switching = false;
    int nextSwitchTrack = 1;

    float fadeTime = 2.0f;
    float switchTime = 0.0f;
    float pauseDifference = 0.0f;

    R_Track* oldTrack = nullptr;
    R_Track* newTrack = nullptr;
    R_Track* playingTrack = nullptr;

    std::vector<R_Track*> tracks;

    std::string trackNaming[10] = { "Track 1", "Track 2", "Track 3", "Track 4", "Track 5", "Track 6", "Track 7", "Track 8", "Track 9", "Track 10" };
};

#endif // !__C_AUDIO_SWITCH_H__