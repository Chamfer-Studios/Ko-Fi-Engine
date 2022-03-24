#ifndef __C_COLLIDER_H__
#define __C_COLLIDER_H__

#include "Component.h"
#include "Physics.h"
#include "MathGeoLib/Math/float3.h"

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
	bool PostUpdate(float dt) override;
	bool UpdateCollider(); // Called whenever a change is produced in collider shape or state

	void CreateCollider(ColliderShape collType);

	void DrawCollider();

	// Serialization 
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// On inspector draw
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Getters & setters
	inline const ColliderShape GetColliderShape() { return colliderShape; }
	inline const const char* GetColliderShapeString() { return ColliderShapeToString(colliderShape); }
	inline void SetColliderShape(const ColliderShape colliderShape) { if (colliderShape == this->colliderShape) return; this->colliderShape = colliderShape; hasUpdated = true; } // We will have to delete actual shape and re-do again a new one
	
	inline const CollisionLayer GetCollisionLayer() { return collisionLayer; }
	inline const const char* GetCollisionLayerString() { return CollisionLayerToString(collisionLayer); }
	inline void SetCollisionLayer(const CollisionLayer collisionLayer) { if (collisionLayer == this->collisionLayer) return; this->collisionLayer = collisionLayer; hasUpdated = true; }
	
	inline const bool IsTrigger() { return isTrigger; }
	inline void SetIsTrigger(const bool isTrigger) { this->isTrigger = isTrigger; hasUpdated = true; }
	
	inline const float3 GetBoxCollSize() { return boxCollSize; }
	inline void SetCollSize(const float3 size) { boxCollSize = size; hasUpdated = true; }
	inline void SetCollSize(const float x, const float y, const float z) { boxCollSize = float3(x, y, z); hasUpdated = true; }

	inline const float3 GetCenterPosition() { return centerPosition; }
	inline void SetCenterPosition(const float3 newCenterPos) { centerPosition = newCenterPos; hasUpdated = true; }
	inline void SetCenterPosition(const float x, const float y, const float z) { centerPosition = float3(x, y, z); hasUpdated = true; }

	inline const bool GetDrawCollider() { return drawCollider; }
	inline void SetDrawCollider(const bool newDrawCollider) { drawCollider = newDrawCollider; }

private:
	// Private methods
	const char* ColliderShapeToString(const ColliderShape collType);
	const char* CollisionLayerToString(const CollisionLayer collLayer);

	// Collider attributes
	bool hasUpdated = false;
	physx::PxShape* shape = nullptr;

	bool enabled = true;
	bool isTrigger = false;

	ColliderShape colliderShape = ColliderShape::NONE;
	int colliderShapeInt = 0;

	CollisionLayer collisionLayer = CollisionLayer::DEFAULT;
	int collisionLayerInt = 0;

	float3 boxCollSize = float3(5, 5, 5); // Box collider size
	float3 centerPosition = float3(0, 0, 0); // Center position of the collider

	bool drawCollider = false;
};

#endif // !__C_COLLIDER_H__
