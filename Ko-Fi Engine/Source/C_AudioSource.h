#ifndef __C_AUDIO_SOURCE_H__
#define __C_AUDIO_SOURCE_H__

#include "C_Audio.h"

using Json = nlohmann::json;

class GameObject;
class R_Track;

class C_AudioSource : public C_Audio
{
public:
	C_AudioSource(GameObject* parent);
	C_AudioSource(GameObject* parent, float volume, bool mute, bool playOnStart, bool loop);
	~C_AudioSource();

	bool Start() override;
	bool Update(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void UpdatePlayState();

	inline bool GetPlayOnStart() const { return playOnStart; }

	inline bool GetMute() const { return mute; }

	void SetLoop(bool active);
	inline bool GetLoop() const { return loop; }

	inline float GetVolume() const { return volume; }

	//inline bool GetBypass() const { return bypass; }

	//void SetPanning(float pan);
	//inline float GetPan() const { return pan; }

	//void SetTranspose(float transpose);
	//inline float GetTranspose() const { return transpose; }

private:
    void SetVolume(float volume);

private:
    R_Track* track;

    float volume = 100.0f, offset = 0.0f;

    bool play = false;

	bool playOnStart = true, loop = false, mute = false;
	
	//bool knobReminder1 = false, knobReminder2 = false;
	//float pan = 0.0f, transpose = 0.0f;
	//bool bypass = false;
};

#endif // !__C_AUDIO_SOURCE_H__