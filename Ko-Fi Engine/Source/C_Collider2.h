#ifndef __C_Collider2_H__
#define __C_Collider2_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"

class C_Collider2 : public Component
{
public:
	//constructors
	C_Collider2(GameObject* parent);
	~C_Collider2();

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

};
#endif // !__C_Collider2_H__

