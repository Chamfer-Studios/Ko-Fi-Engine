#ifndef __P_COLLISION_H__
#define __P_COLLISION_H__
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

struct CollisionPoints {
	float3 a; //Furthest point of A into B
	float3 b; //Furthest point of B into A
	float3 normal; //b-a normalized
	float depth; //length of b-a
	bool hasCollision;
};
struct Transform {
	float3 position;
	float3 scale;
	Quat rotation;
};
struct Collider {
	virtual CollisionPoints TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform) const = 0;

	virtual CollisionPoints TestCollision(
	const Transform* transform,
	const BoxCollider* boxCollider,
	const Transform* sphereTransform) const = 0;

	//virtual CollisionPoints TestCollision(
	//	const Transform* transform,
	//	const SphereCollider* sphere,
	//	const Transform* sphereTransform) const = 0;

	//virtual CollisionPoints TestCollision(
	//	const Transform* transform,
	//	const PlaneCollider* plane,
	//	const Transform* planeTransform) const = 0;
};

struct BoxCollider : Collider
{
	float3 min, max,centerPoint;
	bool Intersects(BoxCollider* a, BoxCollider* b)
	{
		return (a->min.x <= b->max.x && a->max.x >= b->min.x) &&
			(a->min.y <= b->max.y && a->max.y >= b->min.y) &&
			(a->min.z <= b->max.z && a->max.z >= b->min.z);
	}
};

//struct SphereCollider : Collider
//{
//	float3 center;
//	float radius;
//	CollisionPoints TestCollision(
//		const Transform* transform,
//		const Collider* collider,
//		const Transform* colliderTransform) const override
//	{
//		return collider->TestCollision(colliderTransform, this, transform);
//	}
//
//	CollisionPoints TestCollision(
//		const Transform* transform,
//		const SphereCollider* sphere,
//		const Transform* sphereTransform) const override
//	{
//		return algo::FindSphereSphereCollisionPoints(
//			this, transform, sphere, sphereTransform);
//	}
//
//	CollisionPoints TestCollision(
//		const Transform* transform,
//		const PlaneCollider* plane,
//		const Transform* planeTransform) const override
//	{
//		return algo::FindSpherePlaneCollisionPoints(
//			this, transform, plane, planeTransform);
//	}
//};
////struct PlaneCollider
////	: Collider
////{
////	float3 plane;
////	float distance;
////
////	CollisionPoints TestCollision(
////		const Transform* transform,
////		const Collider* collider,
////		const Transform* colliderTransform) const override
////	{
////		return collider->TestCollision(colliderTransform, this, transform);
////	}
////
////	CollisionPoints TestCollision(
////		const Transform* transform,
////		const SphereCollider* sphere,
////		const Transform* sphereTransform) const override
////	{
////		// reuse sphere code
////		CollisionPoints points = sphere->TestCollision(sphereTransform, this, transform);
////
////		float3 T = points.a; // You could have an algo Plane v Sphere to do the swap
////		points.a = points.b;
////		points.b = T;
////
////		points.normal = -points.normal;
////
////		return points;
////	}
////
////	CollisionPoints TestCollision(
////		const Transform* transform,
////		const PlaneCollider* plane,
////		const Transform* planeTransform) const override
////	{
////		return {}; // No plane v plane
////	}
////};
////
//namespace algo {
//	//CollisionPoints FindSphereSphereCollisionPoints(
//	//	const SphereCollider* a, const Transform* ta,
//	//	const SphereCollider* b, const Transform* tb);
//
//
//	//CollisionPoints FindSpherePlaneCollisionPoints(
//	//	const SphereCollider* a, const Transform* ta,
//	//	const PlaneCollider* b, const Transform* tb);
// 
//	/*CollisionPoints FindBoxBoxCollisionPoints(
//		const BoxCollider* a, const Transform* ta,
//		const BoxCollider* b, const Transform* tb);*/
//}
struct Collision {
	RigidBody* objA;
	RigidBody* objB;
	CollisionPoints points;
};
#endif // !__P_COLLISION_H__

