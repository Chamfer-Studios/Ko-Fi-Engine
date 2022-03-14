#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Geometry/Plane.h"

class GameObject;
class ComponentTransform;
using Json = nlohmann::json;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent);
	~ComponentCamera();

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void LookAt(const float3& point);
	void CalculateViewMatrix();
	void RecalculateProjection();
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)
	// At the moment, all components are serialized (saved and loaded) in the same place (SceneManager),
	// but we will consider to have functions in each of them in the future for better code organization.
	//void OnSave(JSONWriter& writer) const override;
	//void OnLoad(const JSONReader& reader) override;

	void DrawFrustum() const;
	bool ClipsWithBBox(const AABB& refBox) const;
	void FrustumCulling();
	void ResetFrustumCulling();
	void Save(Json& json) const override;
	void Load(Json& json) override;

	float3 right, up, front, position, reference, rotation;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float aspectRatio = 1.f;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 100.f;
	float cameraSensitivity = .5f;
	float cameraSpeed = 60.f;
	bool projectionIsDirty = true;

	// Debug bools
	bool drawFrustum = true;
	bool frustumCulling = false;

private:
	float lastDeltaX = 0.f, lastDeltaY = 0.f;

	ComponentTransform* componentTransform = nullptr;
};

#endif // !__COMPONENT_CAMERA_H__