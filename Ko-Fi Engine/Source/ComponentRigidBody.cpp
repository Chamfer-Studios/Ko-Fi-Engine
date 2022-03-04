#include "ComponentRigidBody.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "MathGeoLib/Math/TransformOps.h"

ComponentRigidBody::ComponentRigidBody(GameObject* parent) : Component(parent)
{
	isActive = true;
	isStatic = false;
	isDynamic = false;

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
	
	if (isDynamic)
	{
		parentTransform = owner->GetTransform()->GetGlobalTransform();
		if (isActive)
		{
			//Check forces and apply them to acceleration
			float3 currentAcceleration = force / mass;

			//apply acceleration to linear velocity
			linearVelocity += currentAcceleration / dt;

			//Check velocity and apply it to position
			float3 currentLV = linearVelocity / dt;

			//y axis cap
			if (currentLV.y != 0.0f)
			{
				currentLV.y = 0;
			}

			parentTransform.Translate(currentLV);

			//impement angular movement
			//floate3 currentAV = angularVelocity / dt;
			//translate to rotation angles
			//euler to quat
			// 
			//apply rotation
			//parentTransform.Rotate(Quat rotation);
		}
	}
	else
	{

	}

	return ret;
}

bool ComponentRigidBody::CleanUp()
{
	bool ret = true;

	return ret;
}