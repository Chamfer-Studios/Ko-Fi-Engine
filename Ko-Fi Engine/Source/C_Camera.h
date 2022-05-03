#ifndef __C_CAMERA_H__
#define __C_CAMERA_H__

#include "Component.h"

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/MathFunc.h"

class GameObject;
class C_Transform;
using Json = nlohmann::json;

class C_Camera : public Component
{
public:
	// Constructors
	C_Camera(GameObject* gameObject);
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
	inline float GetAspectRatio() const { return cameraFrustum.AspectRatio(); }
	inline float3 GetReference() const { return reference; }

	inline bool IsEngineCamera() const { return isEngineCamera; }
	inline bool GetIsMainCamera() const { return isMainCamera; }
	inline bool GetIsDrawFrustumActive() const { return isDrawFrustumActive; }

	inline Frustum GetCameraFrustum() const { return cameraFrustum; }
	inline float3 GetRight() const { return cameraFrustum.WorldRight(); }
	inline float3 GetFront() const { return cameraFrustum.Front(); }
	inline float3 GetUp() const { return cameraFrustum.Up(); }
	inline float3 GetPosition() const { return cameraFrustum.Pos(); }

	// CAUTION! --> The Value is on DEG
	// Vertical FOV is LOCKED, You can't set it.
	inline float GetVerticalFov() const { return RadToDeg(cameraFrustum.VerticalFov()); }
	// CAUTION! --> The Value is on DEG
	inline float GetHorizontalFov() const { return RadToDeg(cameraFrustum.HorizontalFov()); }

	inline float GetNearPlaneDistance() const { return cameraFrustum.NearPlaneDistance(); }
	inline float GetFarPlaneDistance() const { return cameraFrustum.FarPlaneDistance(); }
	inline bool GetIsFrustumActive() const { return isFrustumCullingActive; }

	float4x4 GetViewMatrix() const;
	float4x4 GetWorldMatrix() const;
	float4x4 GetProjectionMatrix() const;
	
	// Setters
	void SetAspectRatio(const float& aspectRatio);

	inline void SetReference(float3 newReference) { reference = newReference; }

	inline void SetIsEngineCamera(bool value) { isEngineCamera = value; }
	inline void SetIsMainCamera(bool value) { isMainCamera = value; }
	
	inline void SetFront(float3 newFront) { this->cameraFrustum.SetFront(newFront.Normalized()); }
	inline void SetUp(float3 newUp) { this->cameraFrustum.SetUp(newUp.Normalized()); }
	void SetPosition(float3 newPos);

	// IMPORTANT!! Horizontal Fov Must Be In DEG, not in radians!
	inline void SetHorizontalFov(float horizontalFov) { this->cameraFrustum.SetHorizontalFovAndAspectRatio(DegToRad(horizontalFov) , cameraFrustum.AspectRatio()); }
	inline void SetNearPlaneDistance(float nearPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(nearPlaneDistance,cameraFrustum.FarPlaneDistance());}
	inline void SetFarPlaneDistance(float farPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(cameraFrustum.NearPlaneDistance(),farPlaneDistance);}
	inline void SetViewPlaneDistances(float nearPlaneDistance,float farPlaneDistance) { this->cameraFrustum.SetViewPlaneDistances(nearPlaneDistance,farPlaneDistance);}

	inline void SetIsFrustumActive(bool value) { isFrustumCullingActive = value; }
	inline void SetIsDrawFrustumActive(bool newValue) { isDrawFrustumActive = newValue; }
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

	// Debug bools
	bool isDrawFrustumActive = true;
	bool isFrustumCullingActive = false;

	bool isMainCamera = false;
	bool isEngineCamera = false;
	
};

#endif // !__C_CAMERA_H__