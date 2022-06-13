#pragma once

#include "Component.h"

#include "MathGeoLib/Math/float2.h"

#include "C_Transform2D.h"

class KoFiEngine;
class GameObject;

class C_Canvas : public C_Transform2D
{
public:
	C_Canvas(GameObject* parent);
	virtual ~C_Canvas();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool CleanUp() override;

	bool InspectorDraw(PanelChooser* chooser) override;

	void SetLogicalSize(float2 _newVal) { logicalSize = _newVal; };
	float2 GetLogicalSize();
	float2 GetNormalizedSize() override;
	float2 GetNormalizedPosition() override;

	float2 LogicalToViewport(float2 vec);
	float2 ViewportToLogical(float2 vec);
	static float2 ScreenToViewport(float2 vec);
	float2 ScreenToLogical(float2 vec);

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

private:
	float2 logicalSize;
	inline static KoFiEngine* engine = nullptr;
};