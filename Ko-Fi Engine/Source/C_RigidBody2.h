#ifndef __C_RIGIDBODY2_H__
#define __C_RIGIDBODY2_H__
#include "Component.h"
//struct RigidBody;
class C_RigidBody2 : public Component
{
public:
	//constructors
	C_RigidBody2(GameObject* parent);
	~C_RigidBody2();

	//Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;//OnGUi

	//Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;

private:
	//RigidBody* rigidBody = nullptr;

};

#endif // !__C_RIGIDBODY_H__
