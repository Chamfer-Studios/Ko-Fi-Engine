#ifndef __COMPONENT_RIGID_BODY_H__
#define __COMPONENT_RIGID_BODY_H__

#include "Globals.h"
#include "Engine.h"

#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"

#include "SceneManager.h"

#include "PxPhysicsAPI.h"
#include "Physics.h"

#include "MathGeoLib/Math/Quat.h"
#include "MathGeoLib/Math/float3.h"

class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(GameObject* parent);
	~ComponentRigidBody();

	bool Update(float dt) override;

	// Called whenever a rigid body attribute is changed
	void UpdatePhysicsValues(); 

	inline void	AddForce(physx::PxVec3 force, physx::PxForceMode::Enum forceMode) { if (dynamicBody) dynamicBody->addForce(force, forceMode); }
	inline void	AddTorque(physx::PxVec3 force, physx::PxForceMode::Enum forceMode) { if (dynamicBody) dynamicBody->addTorque(force, forceMode); }

	inline const bool IsSleeping() { return dynamicBody->isSleeping(); }

	// Serialization 
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// On inspector draw
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Getters & setters
	inline const physx::PxRigidActor* GetRigidBody() { if (isKinematic) return staticBody; else return dynamicBody; }



	inline const float3 GetLinearVelocity() { return linearVel; }
	inline void SetLinearVelocity(const float3 newLinearVel) { linearVel = newLinearVel; hasUpdated = true; }
	inline const float3 GetAngularVelocity() { return angularVel; }
	inline void SetAngularVelocity(const float3 newAngularVel) { angularVel = newAngularVel; hasUpdated = true; }
	inline const float GetLinearDamping() { return linearDamping; }
	inline void SetLinearDamping(const float newLinearDamping) { linearDamping = newLinearDamping; hasUpdated = true; }
	inline const float GetAngularDamping() { return angularDamping; }
	inline void SetAngularDamping(const float newAngularDamping) { angularDamping = newAngularDamping; hasUpdated = true; }

	// Rigid body modificable attributes getters & setters
	inline const float GetMass() { return mass; }
	inline void SetMass(const float newMass) { mass = newMass; hasUpdated = true; }
	inline const float GetDensity() { return density; }
	inline void SetDensity(const float newDensity) { density = newDensity; hasUpdated = true; }

	inline const bool GetUseGravity() { return useGravity; }
	inline void SetUseGravity(const bool newGravity) { useGravity = newGravity; hasUpdated = true; }
	inline bool IsStatic() const { return !isKinematic; }
	inline bool IsKinematic() const { return isKinematic; }
	void SetStatic();
	void SetDynamic();

	inline void FreezePositionX(bool freeze) { freezePositionX = freeze; hasUpdated = true; }
	inline void FreezePositionY(bool freeze) { freezePositionY = freeze; hasUpdated = true; }
	inline void FreezePositionZ(bool freeze) { freezePositionZ = freeze; hasUpdated = true; }
	inline void FreezeRotationX(bool freeze) { freezeRotationX = freeze; hasUpdated = true; }
	inline void FreezeRotationY(bool freeze) { freezeRotationY = freeze; hasUpdated = true; }
	inline void FreezeRotationZ(bool freeze) { freezeRotationZ = freeze; hasUpdated = true; }

private:
	physx::PxRigidDynamic* dynamicBody = nullptr;
	physx::PxRigidStatic* staticBody = nullptr;

	bool hasUpdated = false; // This bool serves as a: has the object moved? then update it

	float3 linearVel = float3::zero;
	float3 angularVel = float3::zero;
	float linearDamping = 0.0f;
	float angularDamping = 0.0f;

	// Rigid body modificable attributes
	float mass = 5.0f;
	float density = 1.0f;

	bool useGravity = true;
	bool isKinematic = true;

	bool freezePositionX = false;
	bool freezePositionY = false;
	bool freezePositionZ = false;
	bool freezeRotationX = false;
	bool freezeRotationY = false;
	bool freezeRotationZ = false;
};


#endif // !__COMPONENT_RIGID_BODY_H__

