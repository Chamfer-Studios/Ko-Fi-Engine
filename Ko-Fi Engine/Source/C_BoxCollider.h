#ifndef __C_BOX_COLLIDER_H__
#define __C_BOX_COLLIDER_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"
#include "MathGeoLib/Math/float3.h"
class C_BoxCollider : public Component
{
public:
	//constructors
	C_BoxCollider(GameObject* parent);
	~C_BoxCollider();

	//Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override; //OnGui

	//Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;
private:
	reactphysics3d::BoxShape* boxShape = nullptr;
	reactphysics3d::Collider* collider = nullptr;
	float3 scaleFactor = float3(1, 1, 1);
	std::string currentFilter = "Set Filter";

};
#endif // !__C_BOX_COLLIDER_H__

