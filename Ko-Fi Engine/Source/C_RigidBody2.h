#ifndef __C_RIGIDBODY2_H__
#define __C_RIGIDBODY2_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"

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

	//Getter & Setters
	inline reactphysics3d::RigidBody* GetBody() { return this->body; }

private:
	//RigidBody* rigidBody = nullptr;
	reactphysics3d::RigidBody* body = nullptr;

};

#endif // !__C_RIGIDBODY_H__
