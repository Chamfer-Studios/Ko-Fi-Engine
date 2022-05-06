#ifndef __C_CAMERA_H__
#define __C_CAMERA_H__

#include "Component.h"

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Math/MathFunc.h"

class GameObject;
class C_Transform;
using Json = nlohmann::json;
	enum class CameraType {
		PERSPECTIVE,
		ORTHOGRAPHIC,
	};


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
	// ORTHOGRAPHIC FAR PLANE AND NEAR ARE THE SAME SIZE.
	inline float GetNearPlaneHeight() const { return cameraFrustum.NearPlaneHeight(); }
	// ORTHOGRAPHIC FAR PLANE AND NEAR ARE THE SAME SIZE.
	inline float GetNearPlaneWidth() const { return cameraFrustum.NearPlaneWidth(); }
	
	inline float GetAspectRatio() const { return cameraFrustum.AspectRatio(); }

	inline bool IsEngineCamera() const { return isEngineCamera; }
	inline bool GetIsMainCamera() const { return isMainCamera; }
	inline bool GetIsDrawFrustumActive() const { return isDrawFrustumActive; }

	inline Frustum GetCameraFrustum() const { return cameraFrustum; }
	inline float3 GetRight() const { return cameraFrustum.WorldRight(); }
	inline float3 GetFront() const { return cameraFrustum.Front(); }
	inline float3 GetUp() const { return cameraFrustum.Up(); }
	inline float3 GetPosition() const { return cameraFrustum.Pos(); }
	inline float3 GetReference() const { return reference; }

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
	
	inline void SetFrontAndUp(float3 newFront, float3 newUp) { this->cameraFrustum.SetFrontUp(newFront.Normalized(),newUp.Normalized()); }
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
	void ChangeCameraType(const CameraType& type);

	// Frustum Culling
	void FrustumCulling();
	void ResetFrustumCulling();
	void DrawFrustum() const;
	
	bool ClipsWithBBox(const AABB& refBox) const;

	bool isOrtho = false;
private:
	// Properties
	float3 reference;
	Frustum cameraFrustum;

	// Debug bools
	bool isDrawFrustumActive = true;
	bool isFrustumCullingActive = false;

	bool isMainCamera = false;
	bool isEngineCamera = false;

	CameraType cameraType = CameraType::PERSPECTIVE;

	float hFov, vFov = 0.0f;

	
};

#endif // !__C_CAMERA_H__