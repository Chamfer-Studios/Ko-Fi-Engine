#include "C_RigidBody2.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_Transform.h"


C_RigidBody2::C_RigidBody2(GameObject* parent) : Component(parent)
{

}

C_RigidBody2::~C_RigidBody2()
{
}

bool C_RigidBody2::Start()
{
	

	return true;
}

bool C_RigidBody2::Update(float dt)
{

	return true;
}

bool C_RigidBody2::CleanUp()
{
	return true;
}

bool C_RigidBody2::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("RigidBody2"))
	{
		
	}
	return true;
}

void C_RigidBody2::Save(Json& json) const
{
}

void C_RigidBody2::Load(Json& json)
{
}
