#include "C_RigidBody2.h"
#include "Engine.h"
#include "GameObject.h"
#include "M_Physics.h"
#include "C_Transform.h"
#include "P_World.h"
#include "P_Actor.h"
#include "P_Collision.h"

C_RigidBody2::C_RigidBody2(GameObject* parent) : Component(parent)
{
	this->rigidBody = new RigidBody();
}

C_RigidBody2::~C_RigidBody2()
{
}

bool C_RigidBody2::Start()
{
	this->rigidBody->SetTransform(new Transform());
	owner->GetTransform()->GetGlobalTransform().Decompose(
		rigidBody->GetTransform()->position,
		rigidBody->GetTransform()->rotation,
		rigidBody->GetTransform()->scale);
	owner->GetEngine()->GetPhysics()->world->AddRigidBody(this->rigidBody);

	return true;
}

bool C_RigidBody2::Update(float dt)
{
	owner->GetTransform()->SetPosition(rigidBody->GetTransform()->position);
	owner->GetTransform()->SetRotationQuat(rigidBody->GetTransform()->rotation);
	owner->GetTransform()->SetScale(rigidBody->GetTransform()->scale);
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
		float mass = rigidBody->GetMass();
		if (ImGui::DragFloat("mass##", &mass,1.0f,0.001f,5000.0f))
		{
			if(mass > 0)
			rigidBody->SetMass(mass);
		}
		float3 gravity = rigidBody->GetGravity();
		if (ImGui::DragFloat3("gravity##",&(gravity[0]),0.01f,1000.0f))
		{
			rigidBody->SetGravity(gravity);
		}

	}
	return true;
}

void C_RigidBody2::Save(Json& json) const
{
}

void C_RigidBody2::Load(Json& json)
{
}
