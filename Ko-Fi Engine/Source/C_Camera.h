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
	// Constructors
	C_Camera(GameObject* gameObject, bool isEngineCamera = false);
	~C_Camera();

	// Game Loop
	bool Start() override;
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser); // (OnGui)

	// Serialization
	void Save(Json& json) const override;
	void Load(Json& json) override;

	// Getters
	float GetFarPlaneHeight() const;
	float GetFarPlaneWidth() const;
	inline float GetNearPlaneDistance() const { return nearPlaneDistance; }
	inline float GetFarPlaneDistance() const { return farPlaneDistance; }
	inline int GetSpeedMultiplier() const { return speedMultiplier; }
	inline float GetVerticalFloat() const { return verticalFOV; }
	inline float GetCameraSpeed() const { return cameraSpeed; }
	inline float GetCameraSensitivity() const { return cameraSensitivity; }
	inline float GetLastDeltaX() const { return lastDeltaX; }
	inline float GetLastDeltaY() const { return lastDeltaY; }
	inline float GetAspectRatio() const { return aspectRatio; }

	inline float3 GetPosition() const { return position; }
	inline float3 GetRight() const { return right; }
	inline float3 GetUp() const { return up; }
	inline float3 GetFront() const { return front; }
	inline float3 GetReference() const { return reference; }

	inline Frustum GetCameraFrustum() const { return cameraFrustum; }
	float4x4 GetViewMatrix() const;

	inline bool GetIsProjectionDirty() const { return isProjectionDirty; }
	inline bool GetIsEngineCamera() const { return isEngineCamera; }
	inline bool GetIsMainCamera() const { return isMainCamera; }
	inline bool GetIsDrawFrustumActive() const { return isDrawFrustumActive; }
	
	// Setters
	void SetAspectRatio(const float& aspectRatio);
	inline void SetIsProjectionDirty(bool value) { isProjectionDirty = value; }
	inline void SetIsEngineCamera(bool value) { isEngineCamera = value; }
	inline void SetIsMainCamera(bool value) { isMainCamera = value; }
	inline void SetPosition(float3 newPos) { position = newPos; }
	inline void SetRight(float3 newRigth) { right = newRigth; }
	inline void SetUp(float3 newUp) { up = newUp; }
	inline void SetFront(float3 newFront) { front = newFront; }
	inline void SetReference(float3 newReference) { reference = newReference; }
	inline void SetLastDeltaX(float newValueX) { lastDeltaX = newValueX; }
	inline void SetLastDeltaY(float newValueY) { lastDeltaY = newValueY; }

	// Camera Functions
	void LookAt(const float3& point);
	void ChangeSpeed(int multiplier);
	
	void CalculateViewMatrix(bool ortho = false);
	void RecalculateProjection(bool ortho = false);
	
	// Frustum Culling
	void FrustumCulling();
	void ResetFrustumCulling();
	void DrawFrustum() const;
	
	bool ClipsWithBBox(const AABB& refBox) const;

private:
	// Properties
	float3 right, up, front, position, reference;
	Frustum cameraFrustum;

	float aspectRatio = 1.f;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 100.f;

	float cameraSensitivity = .1f;
	float cameraSpeed = 60.f;
	float baseCameraSpeed = 60.f;
	int speedMultiplier = 1.0f;

	float lastDeltaX = 0.f, lastDeltaY = 0.f;

	// Debug bools
	bool isDrawFrustumActive = true;
	bool isFrustumCullingActive = false;
	bool isProjectionDirty = true;

	bool isMainCamera = false;
	bool isEngineCamera = false;

	C_Transform* componentTransform = nullptr;
	
};

#endif // !__C_CAMERA_H__