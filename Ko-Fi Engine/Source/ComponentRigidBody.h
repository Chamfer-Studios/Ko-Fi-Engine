#pragma once

#include "Component.h"

#include "Globals.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"

class GameObject;

class ComponentRigidBody : public Component
{
public:
	ComponentRigidBody(GameObject* parent);
	~ComponentRigidBody();

	bool Start();
	bool Update(float dt);
	bool CleanUp();

	//getters/setters
	inline bool GetActive() const { return isActive; }
	inline float3 GetLinearVelocity() const { return linearVelocity; }
	inline float3 GetAngularVelocity() const { return angularVelocity; }
	inline float GetMass() const { return mass; }
	inline bool IsDynamic() const { return isDynamic; }
	inline bool IsStatic() const { return isStatic; }

	inline void SetActive(bool state) { isActive = state; }
	inline void SetLinearVelocity(float3 lV) { linearVelocity = lV; }
	inline void SetAngularVelocity(float3 aV) { angularVelocity = aV; }
	inline void SetMass(float newMass) { mass = newMass; }
	inline void SetDynamic(bool dynamic) { isDynamic = dynamic; isStatic = !isDynamic; }
	inline void SetStatic(bool stat) { isStatic = stat; isDynamic = !isStatic; }

private:
	bool isActive;

	bool isDynamic;
	bool isStatic;
	float4x4 parentTransform;
	
	float mass;
	float density;

	float3 linearVelocity;

	float3 angularVelocity;

	bool addForce;
	float3 force;
};