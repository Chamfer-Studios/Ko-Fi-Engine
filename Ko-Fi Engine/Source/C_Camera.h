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
	inline int GetSpeedMultiplier() const { return speedMultiplier; }
	inline float GetCameraSpeed() const { return cameraSpeed; }
	inline float GetCameraSensitivity() const { return cameraSensitivity; }
	inline float GetLastDeltaX() const { return lastDeltaX; }
	inline float GetLastDeltaY() const { return lastDeltaY; }
	inline float GetAspectRatio() const { return aspectRatio; }
	inline float3 GetReference() const { return reference; }

	inline bool GetIsEngineCamera() const { return isEngineCamera; }
	inline bool GetIsMainCamera() const { return isMainCamera; }
	inline bool GetIsDrawFrustumActive() const { return isDrawFrustumActive; }

	inline Frustum GetCameraFrustum() const { return cameraFrustum; }
	inline float3 GetRight() const { return cameraFrustum.WorldRight(); }
	inline float3 GetFront() const { return cameraFrustum.Front(); }
	inline float3 GetUp() const { return cameraFrustum.Up(); }
	inline float3 GetPosition() const { return cameraFrustum.Pos(); }
	inline float GetVerticalFov() const { return cameraFrustum.VerticalFov(); }
	inline float GetHorizontalFov() const { return cameraFrustum.HorizontalFov(); }
	inline float GetNearPlaneDistance() const { return cameraFrustum.NearPlaneDistance(); }
	inline float GetFarPlaneDistance() const { return cameraFrustum.FarPlaneDistance(); }

	float4x4 GetViewMatrix() const;
	
	// Setters
	void SetAspectRatio(const float& aspectRatio);
	void ChangeSpeed(int multiplier);

	inline void SetReference(float3 newReference) { reference = newReference; }
	inline void SetLastDeltaX(float newValueX) { lastDeltaX = newValueX; }
	inline void SetLastDeltaY(float newValueY) { lastDeltaY = newValueY; }

	inline void SetIsEngineCamera(bool value) { isEngineCamera = value; }
	inline void SetIsMainCamera(bool value) { isMainCamera = value; }
	
	inline void SetFront(float3 newFront) { this->cameraFrustum.SetFront(newFront); }
	inline void SetUp(float3 newUp) { this->cameraFrustum.SetUp(newUp); }
	inline void SetPosition(float3 newPos) { this->cameraFrustum.SetPos(newPos); }
	inline void SetVerticalFov(float verticalFov) { this->cameraFrustum.SetVerticalFovAndAspectRatio(verticalFov, aspectRatio); }
	inline void SetHorizontalFov(float horizontalFov) { this->cameraFrustum.SetHorizontalFovAndAspectRatio(horizontalFov, aspectRatio); }
	inline void SetNearPlaneDistance(float nearPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(nearPlaneDistance,cameraFrustum.FarPlaneDistance());}
	inline void SetFarPlaneDistance(float farPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(cameraFrustum.NearPlaneDistance(),farPlaneDistance);}
	inline void SetViewPlaneDistances(float nearPlaneDistance,float farPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(nearPlaneDistance,farPlaneDistance);}

	// Camera Functions
	void LookAt(const float3& point);

	// Frustum Culling
	void FrustumCulling();
	void ResetFrustumCulling();
	void DrawFrustum() const;
	
	bool ClipsWithBBox(const AABB& refBox) const;

private:
	// Properties
	float3 reference;
	Frustum cameraFrustum;

	float aspectRatio = 1.f;
	

	float cameraSensitivity = .1f;
	float cameraSpeed = 60.f;
	float baseCameraSpeed = 60.f;
	int speedMultiplier = 1.0f;

	float lastDeltaX = 0.f, lastDeltaY = 0.f;

	// Debug bools
	bool isDrawFrustumActive = true;
	bool isFrustumCullingActive = false;

	bool isMainCamera = false;
	bool isEngineCamera = false;

	C_Transform* componentTransform = nullptr;
	
};

#endif // !__C_CAMERA_H__