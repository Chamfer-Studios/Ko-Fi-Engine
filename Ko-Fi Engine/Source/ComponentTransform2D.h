#pragma once

#include "Component.h"
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"

class KoFiEngine;

class ComponentTransform2D : public Component {
public:
	enum class Anchor {
		TOP_LEFT,
		TOP_CENTER,
		TOP_RIGHT,
		LEFT,
		CENTER,
		RIGHT,
		BOTTOM_LEFT,
		BOTTOM_CENTER,
		BOTTOM_RIGHT,
	};
public:
	ComponentTransform2D(KoFiEngine* engine, GameObject* parent);

	bool Update() override;
	virtual bool InspectorDraw(PanelChooser* chooser) override;

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	// Setters
	void SetPosition(const float2& newPosition);
	void SetPivot(const float2& newPivot);
	void SetRotation(const float3& newRotation);
	void SetSize(const float2& newSize);
	void SetAnchor(const Anchor& newAnchor);

	// Getters
	inline float2 GetPosition() const { return position; };
	inline float2 GetPivot() const { return pivot; };
	inline float3 GetRotation() const { return rotation; };
	inline float2 GetSize() const { return size; };
	inline Anchor GetAnchor() const { return anchor; };

	virtual void GetRealPosition(float2& position, bool ignoreCanvas = false);
	virtual void GetRealSize(float2& realSize);

	float2 GetCanvasCenter();

	bool CheckMouseInsideBounds();

	float2 GetAnchorPosition(Anchor anchor);
	float2 GetRelativeAnchorPosition(Anchor anchor);

	float2 position = { 0, 0 };
	float2 pivot = { 0, 0 };
	float3 rotation = { 0.f,0.f,0.f };
	float2 size = { 0, 0 };

	Anchor anchor = Anchor::CENTER;

protected:
	KoFiEngine* engine = nullptr;
};