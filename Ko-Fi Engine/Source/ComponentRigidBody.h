#ifndef __COMPONENT_RIGID_BODY_H__
#define __COMPONENT_RIGID_BODY_H__

#include "Globals.h"
#include "Defs.h"
#include "Engine.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"

#include "PxPhysicsAPI.h"
#include "Physics.h"

#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float3.h"

class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(GameObject* parent);
	~ComponentRigidBody();

	bool Start() override;
	bool Update() override;
	bool CleanUp() override;

	void ApplyPhysics();

	// Getters & setters
	inline bool IsStatic() const { return isStatic; }
	inline bool IsKinematic() const { return !isStatic; }
	inline const physx::PxRigidDynamic* const GetRigidBody() { return rigidBody; }
	void SetStatic();
	void SetDynamic();
private:
	physx::PxRigidDynamic* rigidBody = nullptr;

	bool isStatic = false;

	bool affectGravity = true;
	bool hasUpdated = false; // This bool serves as a, has the object moved? then update it

	float mass = 5.0f;
};


#endif // !__COMPONENT_RIGID_BODY_H__

