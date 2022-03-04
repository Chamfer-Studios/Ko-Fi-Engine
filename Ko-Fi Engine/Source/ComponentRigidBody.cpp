#include "ComponentRigidBody.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "MathGeoLib/Math/TransformOps.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent) : Component(parent)
{
	isActive = false;
	modifiedLinVel = false;
	modifiedAngVel = false;
	modifiedForce = false;

	mass = 0;
	density = 0;
	linearVelocity = float3::zero;
	angularVelocity = float3::zero;
	force = float3::zero;
}

ComponentRigidBody::~ComponentRigidBody()
{
	
}

bool ComponentRigidBody::Start()
{
	bool ret = true;

	return ret;                                                                                                                                               
}

bool ComponentRigidBody::Update(float dt)
{
	bool ret = true;

	parentTransform = owner->GetTransform()->GetGlobalTransform();
	if (isActive)
	{
		//Check forces and apply them to velocity
		if (modifiedForce)
		{
			float3 currentForce = force / dt; 

			//apply force to linear velocity
			linearVelocity += currentForce;
		}
		//Check velocity and apply it to position
		if (modifiedLinVel)
		{
			float3 currentLV = linearVelocity / dt;

			//y axis cap
			if (currentLV.y != 0.0f)
			{
				currentLV.y = 0;
			}
			
			//apply velocity to position
			parentTransform.Translate(currentLV);
		}
		else if (modifiedAngVel)
		{
			float3 currentLV = linearVelocity / dt;

			//apply velocity to rotation
			//parentTransform.Rotate(Quat rotation);
		}

	}
	return ret;
}

bool ComponentRigidBody::CleanUp()
{
	bool ret = true;

	return ret;
}