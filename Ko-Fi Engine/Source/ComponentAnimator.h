#ifndef __COMPONENT_ANIMATOR_H__
#define __COMPONENT_ANIMATOR_H__

#include <vector>
#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"

class GameObject;

struct AnimationClip {
	int whatever;
};

class ComponentAnimator : public Component
{
public:
	ComponentAnimator(GameObject* parent);
	~ComponentAnimator();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
	bool InspectorDraw(PanelChooser* chooser);

	void Reset();

	void Save();

	bool playing;

	//R_Animation rAnim;
};

#endif // __COMPONENT_ANIMATOR_H__
