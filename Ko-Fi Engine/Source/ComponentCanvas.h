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

	void AdjustPositionAndSize();
	float2 GetNormalizedSize() override;
	float2 GetNormalizedPosition() override;

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;
};