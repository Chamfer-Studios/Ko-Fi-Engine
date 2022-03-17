#ifndef __C_AUDIO_SOURCE_H__
#define __C_AUDIO_SOURCE_H__

#include "C_Audio.h"
#include "R_Track.h"

using Json = nlohmann::json;

class GameObject;

class C_AudioSource : public C_Audio
{
public:
	C_AudioSource(GameObject* parent);
	~C_AudioSource();

	bool Start() override;
	bool Update(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void UpdatePlayState();

private:
    R_Track* track;
};

#endif // !__C_AUDIO_SOURCE_H__