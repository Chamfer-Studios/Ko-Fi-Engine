#ifndef __C_CAPSULE_COLLIDER_H__
#define __C_CAPSULE_COLLIDER_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"

class C_CapsuleCollider : public Component
{
public:
	//constructors
	C_CapsuleCollider(GameObject* parent);
	~C_CapsuleCollider();

	//Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override; //OnGui

	//Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;
private:
	reactphysics3d::CapsuleShape* capsuleShape = nullptr;
	reactphysics3d::Collider* collider = nullptr;

};
#endif // !__C_CAPSULE_COLLIDER_H__

