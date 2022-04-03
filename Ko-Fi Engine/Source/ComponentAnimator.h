#ifndef __COMPONENT_ANIMATOR_H__
#define __COMPONENT_ANIMATOR_H__

#include <vector>
#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"
#include <map>

class GameObject;
class R_Animation;
class AnimatorClip;

class ComponentAnimator : public Component
{
public:
	ComponentAnimator(GameObject* parent);
	~ComponentAnimator();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool InspectorDraw(PanelChooser* chooser);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void Reset();

	bool CreateClip(const AnimatorClip& clip);
	bool CreateDefaultClip(AnimatorClip* clip);

	void SetAnim(R_Animation* anim);
	AnimatorClip* GetSelectedClip();

	void SetSelectedClip(std::string name);

private:
	bool playing;

	R_Animation* animation;

	std::map<std::string, AnimatorClip> clips;
	AnimatorClip* selectedClip;
};

#endif // __COMPONENT_ANIMATOR_H__
