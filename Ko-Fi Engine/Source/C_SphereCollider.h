#ifndef __C_SPHERE_COLLIDER_H__
#define __C_SPHERE_COLLIDER_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"

class C_SphereCollider : public Component
{
public:
	//constructors
	C_SphereCollider(GameObject* parent);
	~C_SphereCollider();

	//Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override; //OnGui

	//Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;
private:
	reactphysics3d::SphereShape* sphereShape = nullptr;
	reactphysics3d::Collider* collider = nullptr;
	float scaleFactor = 1.0f;
	std::string currentFilter = "Set Filter";

};
#endif // !__C_SPHERE_COLLIDER_H__

