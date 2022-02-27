#pragma once

#include "Component.h"

#include "MathGeoLib/Math/float2.h"

#include "ComponentTransform2D.h"

class KoFiEngine;

class ComponentCanvas : public ComponentTransform2D {
public:
	ComponentCanvas(GameObject* parent);

	bool Update() override;

	bool InspectorDraw(PanelChooser* chooser) override;

	float2 GetLogicalSize();
	float2 GetNormalizedSize() override;
	float2 GetNormalizedPosition() override;

	float2 LogicalToScreen(float2 vec);
	float2 ScreenToLogical(float2 vec);

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

private:
	float2 logicalSize;
};