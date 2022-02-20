#pragma once

#include "Component.h"

#include "MathGeoLib/Math/float2.h"

#include "ComponentTransform2D.h"

class KoFiEngine;

class ComponentCanvas : public ComponentTransform2D {
public:
	ComponentCanvas(KoFiEngine* engine, GameObject* parent);

	bool Update() override;

	bool InspectorDraw(PanelChooser* chooser) override;

	void AdjustPositionAndSize();

	void GetRealPosition(float2& position, bool ignoreCanvas = false) override;
	void GetRealSize(float2& realSize) override;

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;
};