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
	ComponentTransform(GameObject* parent);
	~ComponentTransform();

	bool Update() override;
	bool InspectorDraw(PanelChooser* chooser); // OngGui

	void SetPosition(const float3& newPosition);
	void SetRotation(const float3& newRotation);
	void SetRotation(const Quat& newRotation);
	void SetScale(const float3& newScale);

	inline float3 GetPosition() const { return position; };
	inline float3 GetRotation() const { return rotationEuler; };
	inline float3 GetScale() const { return scale; };

	inline const float3& Right() const { return right; }
	inline const float3& Up() const { return up; }
	inline const float3& Front() const { return front; }

	void NewAttachment();
	void OnParentMoved();

	void RecomputeGlobalMatrix();

	float4x4 GetGlobalTransform();

	bool GetDirty() const;
	void SetDirty(bool isDirty);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	float4x4 transformMatrix;
	float4x4 transformMatrixLocal;

private:
	bool isDirty = true;

	float3 position;
	Quat rotation;
	float3 rotationEuler;
	float3 scale;

	float3 front = float3::unitZ;
	float3 up = float3::unitY;
	float3 right = float3::unitX;
};

#endif // !__COMPONENT_TRANSFORM_H__