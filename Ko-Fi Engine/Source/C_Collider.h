#ifndef __C_COLLIDER_H__
#define __C_COLLIDER_H__

#include "GameObject.h"
#include "Component.h"
#include "ComponentRigidBody.h"

#include "Physics.h"

enum class ColliderType2
{
	BOX,
	SPHERE,
	CAPSULE,
	NONE
};

class ComponentCollider2 : public Component
{
public:
	ComponentCollider2(GameObject* parent, ColliderType2 collType);
	~ComponentCollider2();

	bool Update(float dt) override;
	bool CleanUp() override;

	void CreateCollider(ColliderType2 collType);

	// Serialization 
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// On inspector draw
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Getters & setters
	inline const ColliderType2 GetColliderType() { return colliderType; }
	inline void SetColliderType(const ColliderType2 colliderType) { this->colliderType = colliderType; } // We will have to delete actual shape and re-do again a new one
	inline const bool IsTrigger() { return isTrigger; }
	inline void SetIsTrigger(const bool isTrigger) { this->isTrigger = isTrigger; }

private:
	physx::PxShape* shape = nullptr;

	bool isTrigger = false;

	ColliderType2 colliderType = ColliderType2::NONE;

};

#endif // !__C_COLLIDER_H__
