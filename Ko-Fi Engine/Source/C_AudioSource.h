#ifndef __C_AUDIO_SOURCE_H__
#define __C_AUDIO_SOURCE_H__

#include "C_Audio.h"

using Json = nlohmann::json;

class R_Track;

class C_AudioSource : public C_Audio
{
public:
	C_AudioSource(GameObject* parent);
	C_AudioSource(GameObject* parent, float volume, float pan, float transpose, bool mute, bool playOnStart, bool loop, bool bypass);
	~C_AudioSource();

	bool Start() override;
	bool Update(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	inline bool GetPlayOnStart() const { return playOnStart; }

	inline bool GetMute() const { return mute; }

	void SetLoop(bool active);
	inline bool GetLoop() const { return loop; }

	//bool GetBypass() const { return bypass; }

	inline float GetVolume() const { return volume; }

	void SetPanning(float pan);
	inline float GetPan() const { return pan; }

	void SetTranspose(float transpose);
	inline float GetTranspose() const { return transpose; }

	void UpdatePlayState();

private:
    void SetVolume(float volume);

private:
    R_Track* track;

    float volume = 100.0f, pan = 0.0f, transpose = 0.0f, offset = 0.0f;

    //bool knobReminder1 = false, knobReminder2 = false;
    bool play = false;

	bool playOnStart = true, loop = false, mute = false;
	//bool bypass = false;
};

#endif // !__C_AUDIO_SOURCE_H__