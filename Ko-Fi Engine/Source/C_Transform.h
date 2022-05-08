#ifndef __C_TRANSFORM_H__
#define __C_TRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

using Json = nlohmann::json;
class GameObject;

class C_Transform : public Component
{
public:
	// Constructors
	C_Transform(GameObject* parent);
	~C_Transform();

	// Game Loop
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser *chooser) override; // OngGui

	// Setters
	void SetPosition(const float3 &newPosition);
	void SetScale(const float3 &newScale);
	void SetRotationEuler(const float3 &newRotation);
	void SetRotationQuat(const Quat &newRotation);
	void LookAt(float3 &front, float3& up);
	void SetGlobalTransform(const float4x4 &globalTransform);
	void SetDirty(bool isDirty);

	// Getters
	float3 GetPosition() const;
	float3 GetScale() const;
	float3 GetRotationEuler() const;
	Quat GetRotationQuat() const;
	const float3 &Right() const;
	const float3 &GlobalRight() const;
	const float3 &Up() const;
	const float3 &GlobalUp() const;
	const float3 &Front() const;
	const float3 &GlobalFront() const;
	inline float4x4 GetGlobalTransform() const { return transformMatrix; }
	inline float4x4 GetLocalTransform() const { return transformMatrixLocal; }
	inline bool GetDirty() const { return isDirty; }

	// Transform Functions
	void RecomputeGlobalMatrix();

	// Serialization
	void Save(Json &json) const override;
	void Load(Json &json) override;

private:
	// Transfrom Properties
	float4x4 transformMatrix;
	float4x4 transformMatrixLocal;
	bool isDirty = true;
};

#endif // !__C_TRANSFORM_H__