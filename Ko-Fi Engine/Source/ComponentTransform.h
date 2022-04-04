#ifndef __COMPONENT_TRANSFORM_H__
#define __COMPONENT_TRANSFORM_H__

#include "Component.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/Quat.h"

using Json = nlohmann::json;

class ComponentTransform : public Component
{
public:
	// Constructors
	ComponentTransform(GameObject *parent);
	~ComponentTransform();

	// Game Loop
	bool Update(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser *chooser) override; // OngGui

	// Setters
	void SetPosition(const float3 &newPosition);
	void SetScale(const float3 &newScale);
	void SetRotationEuler(const float3 &newRotation);
	void SetRotationQuat(const Quat &newRotation);
	void SetFront(const float3 &front);
	void SetGlobalTransform(const float4x4 &globalTransform);
	void SetDirty(bool isDirty);

	// Getters
	float3 GetPosition() const;
	float3 GetScale() const;
	float3 GetRotationEuler() const;
	Quat GetRotationQuat() const;
	const float3 &Right() const;
	const float3 &Up() const;
	const float3 &Front() const;
	float4x4 GetGlobalTransform();
	inline bool GetDirty() const { return isDirty; }

	// Transform Functions
	void RecomputeGlobalMatrix();

	// Guizmo
	void UpdateGuizmoParameters(float4x4 &transformMatrix);

	// Serialization
	void Save(Json &json) const override;
	void Load(Json &json) override;

private:
	// Transfrom Properties
	float4x4 transformMatrix;
	float4x4 transformMatrixLocal;

	float3 rotationEuler; // Just for ImGui Values
	bool isDirty = true;
};

#endif // !__COMPONENT_TRANSFORM_H__