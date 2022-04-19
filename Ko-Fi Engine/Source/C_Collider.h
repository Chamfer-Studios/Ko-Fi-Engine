#ifndef __C_COLLIDER_H__
#define __C_COLLIDER_H__

#include "Component.h"
#include "M_Physics.h"
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
	PROJECTILE,
	TERRAIN
};

class C_Transform;

class C_Collider : public Component
{
public:
	C_Collider(GameObject* parent, ColliderShape collType = ColliderShape::NONE);
	~C_Collider();

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool UpdateCollider(); // Called whenever a change is produced in collider shape or state

	void CreateCollider(ColliderShape collType);
	void CreateBoxCollider();

	void DrawCollider();
	void DrawBoxCollider();

	// Serialization 
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// On inspector draw
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Getters & setters
	inline const ColliderShape GetColliderShape() { return colliderShape; }
	inline const const char* GetColliderShapeString() { return ColliderShapeToString(colliderShape); }
	inline void SetColliderShape(const ColliderShape colliderShape) { if (colliderShape == this->colliderShape) return; this->colliderShape = colliderShape; hasUpdated = true; } // We will have to delete actual shape and re-do again a new one
	
	inline const std::string* GetFilter() const { return &filter; }
	inline void SetFilter(const std::string newFilter) { if (newFilter == filter) return; filter = newFilter; hasUpdated = true; }
	
	inline const bool IsTrigger() { return isTrigger; }
	inline void SetIsTrigger(const bool isTrigger) { this->isTrigger = isTrigger; hasUpdated = true; }
	
	inline const float3 GetBoxCollSize() { return boxCollSize; }
	inline void SetCollSize(const float3 size) { boxCollSize = size; hasUpdated = true; }
	inline void SetCollSize(const float x, const float y, const float z) { boxCollSize = float3(x, y, z); hasUpdated = true; }

	inline const float3 GetOffset() { return offset; }
	inline void SetOffset(const float3 offset) { this->offset = offset; hasUpdated = true; }
	inline void SetOffset(const float x, const float y, const float z) { offset = float3(x, y, z); hasUpdated = true; }

	inline const bool GetDrawCollider() { return drawCollider; }
	inline void SetDrawCollider(const bool newDrawCollider) { drawCollider = newDrawCollider; }

	inline void UpdateCollSizeFromAABB() { this->setFromAABB = true; this->hasUpdated = true; }

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

	std::string filter = "default";

	float3 boxCollSize = float3(50, 50, 50); // Box collider size
	float3 offset = float3::zero;
	bool setFromAABB = true;

	bool drawCollider = false;

	std::string* debugFilter;
	C_Transform* prevTransform;
};

#endif // !__C_COLLIDER_H__
