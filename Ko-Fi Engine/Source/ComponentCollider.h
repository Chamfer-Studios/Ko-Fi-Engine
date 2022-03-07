#ifndef __COMPONENT_COLLIDER_H__
#define __COMPONENT_COLLIDER_H__

#include "Component.h"
#include "GameObject.h"
#include "CollisionDetector.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"
#include "Engine.h"

// See: https://github.com/MissclickStudios/Projecte3/commits/31099184ed0cfc0294897689afc08654caf98fde

class ComponentMesh;

enum class ColliderType
	{
		UNDEFINED,
		PLAYER,
		ENEMY,
		WALL,
		FLOOR
	};

class ComponentCollider : public Component
{
public:

	ComponentCollider(GameObject* parent, ColliderType collType = ColliderType::UNDEFINED);
	~ComponentCollider();

	bool Start() override; //get the owner and put it inside the list of collidableEntities
	bool Update(float dt) override;
	bool CleanUp() override;
	bool PostUpdate(float dt) override;
	 
	// ----- Editor settings ---------------
	bool InspectorDraw(PanelChooser* chooser);
	// -------------------------------------

	// ----- Serialization -----------------
	void Save(Json& json) const override;
	void Load(Json& json) override;
	// -------------------------------------

	// ----- Getters Setters ---------------
	const ColliderType GetColliderType() const { return collType; }
	bool IsTrigger() const { return isTrigger; }
	bool IsColliding() const { return isColliding; }
	GameObject* GetLastCollided() const { return lastObjectCollided; }

	void SetColliderType(ColliderType type);
	void SetTrigger(bool enable) { isTrigger = enable; }
	void SetColliding(bool collides) { isColliding = collides; }
	void SetNewCollision(GameObject* collidedGO) { isColliding = true; lastObjectCollided = collidedGO; }
	// -------------------------------------

	
private:
	bool isTrigger = false;
	bool isColliding = false;
	GameObject* lastObjectCollided = nullptr;

	ColliderType collType;
	int colliderType;
};


#endif
