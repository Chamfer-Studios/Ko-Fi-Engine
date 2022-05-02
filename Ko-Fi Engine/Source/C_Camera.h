#ifndef __C_CAMERA_H__
#define __C_CAMERA_H__

#include "Component.h"

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Geometry/Plane.h"

class GameObject;
class C_Transform;
using Json = nlohmann::json;

class C_Camera : public Component
{
public:
	C_Camera(GameObject* gameObject, bool isEngineCamera = false);
	~C_Camera();

	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;

	void LookAt(const float3& point);
	void CalculateViewMatrix(bool ortho = false);
	void RecalculateProjection(bool ortho = false);
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	void DrawFrustum() const;
	bool ClipsWithBBox(const AABB& refBox) const;
	void FrustumCulling();
	void ResetFrustumCulling();
	void SetAspectRatio(const float& aspectRatio);
	float GetFarPlaneHeight() const;
	float GetFarPlaneWidth() const;
	void Save(Json& json) const override;
	void Load(Json& json) override;

	float3 right, up, front, position, reference;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float aspectRatio = 1.f;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 100.f;
	float cameraSensitivity = .5f;
	float cameraSpeed = 60.f;
	bool projectionIsDirty = true;
	bool ortho = false;
	bool freeRotation = false;

	// Debug bools
	bool drawFrustum = true;
	bool frustumCulling = false;

	bool isMainCamera = false;
	bool isEngineCamera = false;

	float lastDeltaX = 0.f, lastDeltaY = 0.f;
private:

	C_Transform* componentTransform = nullptr;
	
};

#endif // !__C_CAMERA_H__