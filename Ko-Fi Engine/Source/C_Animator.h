#ifndef __C_ANIMATOR_H__
#define __C_ANIMATOR_H__

#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"

class GameObject;
class R_Animation;
class AnimatorClip;
class map;

class C_Animator : public Component
{
public:
	C_Animator(GameObject* parent);
	~C_Animator();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool InspectorDraw(PanelChooser* chooser);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void Reset();

	bool CreateClip(const AnimatorClip& clip);
	bool CreateDefaultClip(AnimatorClip* clip);
	bool ResetClip();

	void SetAnim(R_Animation* anim);
	void SetSelectedClip(std::string name);

	AnimatorClip* GetSelectedClip();
	bool IsCurrentClipPlaying();
	bool IsCurrentClipLooping();

private:
	R_Animation* animation = nullptr;

	std::map<std::string, AnimatorClip> clips;
	AnimatorClip* selectedClip = nullptr;
	AnimatorClip* clipToDelete = nullptr;

	bool createClipErrorMessage = false;
};

#endif // !__C_ANIMATOR_H__
