#ifndef __C_ANIMATOR__
#define __C_ANIMATOR__

#include <vector>
#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"

class GameObject;

class C_Animator : public Component
{
	C_Animator(GameObject* parent);
	~C_Animator();

	bool Start();
	bool Update(float dt);
	bool CleanUp();
};

#endif // __C_ANIMATOR__
