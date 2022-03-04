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

	inline void SetActive(bool state) { isActive = state; }
	inline void SetLinearVelocity(float3 lV) { linearVelocity = lV;  }

private:
	bool isActive;
	float4x4 parentTransform;

	float mass;
	float density;

	float3 linearVelocity;
	bool modifiedLinVel;
	float3 angularVelocity;
	bool modifiedAngVel;

	bool addForce;
	float3 force;
	bool modifiedForce;

};