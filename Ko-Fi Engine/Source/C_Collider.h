#ifndef __C_COLLIDER_H__
#define __C_COLLIDER_H__

#include "GameObject.h"
#include "Component.h"
#include "ComponentRigidBody.h"

#include "Physics.h"
#include "MathGeoLib/Math/float3.h"
#include "PxPhysicsAPI.h"

enum class ColliderShape
{
	NONE,
	BOX,
	SPHERE,
	CAPSULE
};

enum class CollisionLayer
{
	DEFAULT,
	PLAYER,
	ENEMY,
	BULLET,
	TERRAIN
};

class ComponentCollider2 : public Component
{
public:
	ComponentCollider2(GameObject* parent, ColliderShape collType);
	~ComponentCollider2();

	bool Update(float dt) override;
	bool UpdateCollider(); // Called whenever a change is produced in collider shape or state

	void CreateCollider(ColliderShape collType);

	// Serialization 
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// On inspector draw
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Getters & setters
	inline const ColliderShape GetColliderShape() { return colliderShape; }
	inline const const char* GetColliderShapeString() { return ColliderShapeToString(colliderShape); }
	inline void SetColliderShape(const ColliderShape colliderShape) { if (colliderShape == this->colliderShape) return; this->colliderShape = colliderShape; toUpdate = true; } // We will have to delete actual shape and re-do again a new one
	
	inline const CollisionLayer GetCollisionLayer() { return collisionLayer; }
	inline const const char* GetCollisionLayerString() { return CollisionLayerToString(collisionLayer); }
	inline void SetCollisionLayer(const CollisionLayer collisionLayer) { if (collisionLayer == this->collisionLayer) return; this->collisionLayer = collisionLayer; toUpdate = true; }
	
	inline const bool IsTrigger() { return isTrigger; }
	inline void SetIsTrigger(const bool isTrigger) { this->isTrigger = isTrigger; toUpdate = true; }
	
	inline const float3 GetCollSize() { return boxCollSize; }
	inline void SetCollSize(const float3 size) { boxCollSize = size; toUpdate = true; }
	inline void SetCollSize(const float x, const float y, const float z) { boxCollSize = float3(x, y, z); toUpdate = true; }

private:
	// Private methods
	const char* ColliderShapeToString(const ColliderShape collType);
	const char* CollisionLayerToString(const CollisionLayer collLayer);

	// Collider attributes
	bool toUpdate = false;
	physx::PxShape* shape = nullptr;

	bool enabled = true;
	bool isTrigger = false;

	ColliderShape colliderShape = ColliderShape::NONE;
	int colliderShapeInt = 0;

	CollisionLayer collisionLayer = CollisionLayer::DEFAULT;
	int collisionLayerInt = 0;

	float3 boxCollSize = float3(1, 1, 1);
};

#endif // !__C_COLLIDER_H__
