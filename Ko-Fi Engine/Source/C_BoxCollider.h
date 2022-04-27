#ifndef __C_BOX_COLLIDER_H__
#define __C_BOX_COLLIDER_H__
#include "Component.h"
#include "reactphysics3d/reactphysics3d.h"
#include "MathGeoLib/Math/float3.h"
class C_BoxCollider : public Component
{
public:
	//constructors
	C_BoxCollider(GameObject* parent);
	~C_BoxCollider();

	//Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override; //OnGui

	//Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;

	void UpdateFilter();
	void UpdateScaleFactor();
	void UpdateIsTrigger();
	void UpdateCenter();

	// Getters & Setters
	inline std::string GetFilter() const { return filter; };
	inline void SetFilter(const std::string newFilter) { filter = newFilter; };

	inline bool GetIsTrigger() const { return isTrigger; }
	inline void SetIsTrigger(const bool newIsTrigger) { isTrigger = newIsTrigger; }

	inline float3 GetScaleFactor() const { return scaleFactor; }
	inline void SetScaleFactor(const float3 newScaleFactor) { scaleFactor = newScaleFactor; }
	inline void SetScaleFactor(const float scaleX, const float scaleY, const float scaleZ) { scaleFactor = float3(scaleX, scaleY, scaleZ); }

	inline float3 GetCenter() const { return center; }
	inline void SetCenter(const float3 newCenter) { center = newCenter; }
	inline void SetCenter(const float centerX, const float centerY, const float centerZ) { center = float3(centerX, centerY, centerZ); }

private:
	reactphysics3d::BoxShape* boxShape = nullptr;
	reactphysics3d::Collider* collider = nullptr;

	// Variables serialized
	std::string filter = "";
	bool isTrigger = false;
	float3 scaleFactor = float3(1, 1, 1);
	float3 center = { 0,0,0 };
	float3 size = { 1,1,1 };
};
#endif // !__C_BOX_COLLIDER_H__

