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
    ~C_AudioSwitch();

    bool Start() override;
    bool Update(float dt) override;
    bool OnPlay() override;

    bool InspectorDraw(PanelChooser* chooser) override;

    void Save(Json& json) const override;
    void Load(Json& json) override;

    void UpdatePlayState();

    void SwitchTrack(int newTrackIndex);
    void PlayTrack(int trackIndex);
    void ResumeTrack(int trackIndex);
    void StopTrack(int trackIndex);
    void PauseTrack(int trackIndex);

    void SwitchFade(float fadeSeconds);

    bool IsAnyTrackPlaying() const;
    R_Track* GetPlayingTrack() const;
    int GetPlayingTrackID() const;

    void StopAllTracks();
    void DisablePlayOnStart(R_Track* trackToChange);

    inline int GetTotalTracks() const { return totalTracks; }
    inline int GetNextSwitchTrack() const { return nextSwitchTrack; }
    inline float GetFadeTime() const { return fadeTime; }

    void DrawEditor(R_Track* track);

private:
    int totalTracks = 0;

    bool switching = false;
    int nextSwitchTrack = 0;

    float fadeTime = 2.0f;
    float switchTime = 0.0f;
    float pauseDifference = 0.0f;

    int trackIdInEdit = -1;

    bool offsetSync = false;
    float oldOffset = 0.0f;

    R_Track* oldTrack = nullptr;
    R_Track* newTrack = nullptr;
    R_Track* playingTrack = nullptr;

    std::vector<R_Track*> tracks;
};

#endif // !__C_AUDIO_SWITCH_H__