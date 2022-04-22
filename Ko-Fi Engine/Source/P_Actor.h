#pragma once
#ifndef __P_ACTOR_H__
#define __P_ACTOR_H__
#include "MathGeoLib/Math/float3.h"
#include "P_Collision.h"
#include <functional>

struct CollisionActor {
public:
	inline void SetTransform(Transform* transform) { this->transform = transform; }
	//inline void SetCollider(Collider* collider) { this->collider = collider; }
	inline void SetIsTrigger(bool isTrigger) { this->isTrigger = isTrigger; }
	inline Transform* GetTransform() { return this->transform; }
	//inline Collider* GetCollider() { return this->collider; }
	inline bool GetIsTrigger() { return this->isTrigger; }
	inline bool GetIsDynamic() { return this->isDynamic; }
protected:
	Transform* transform = nullptr;
	//Collider* collider;
	bool isTrigger = false;
	bool isStatic = false;
	bool isDynamic = true;
	//std::function<void(Collision&, float)> onCollision;
};

struct RigidBody : CollisionActor
{
public:
	inline void SetGravity(float3 gravity) { this->gravity = gravity; }
	inline void SetForce(float3 force) { this->force = force; }
	inline void SetVelocity(float3 velocity) { this->velocity = velocity; }
	inline void SetMass(float mass) { this->mass = mass; }
	inline void SetTakesGravity(bool takesGravity) { this->takesGravity = takesGravity; }
	inline void SetStaticFriction(float staticFriction) { this->staticFriction = staticFriction; }
	inline void SetDynamicFriction(float dynamicFriction) { this->dynamicFriction = dynamicFriction; }
	inline void SetRestitution(float restitution) { this->restitution = restitution; }

	inline float3 GetGravity() { return this->gravity; }
	inline float3 GetForce() { return this->force; }
	inline float3 GetVelocity() { return this->velocity; }
	inline float GetMass() { return this->mass; }
	inline bool GetTakesGravity() { return this->takesGravity; }
	inline float GetStaticFriction() { return this->staticFriction; }
	inline float GetDynamicFriction() { return this->dynamicFriction; }
	inline float GetRestitution() { return this->restitution; }


private: 
	float3 gravity = float3(0,-1000.0f,0); //gravitational acceleration
	float3 force = float3::zero;
	float3 velocity = float3::zero;
	
	float mass = 1000.0f;
	bool takesGravity = true;

	float staticFriction = 0.0f;
	float dynamicFriction = 0.0f;
	float restitution = 0.0f;
};
#endif // !__P_ACTOR_H__

