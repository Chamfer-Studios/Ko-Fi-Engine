#pragma once

#include "Component.h"

#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"

class KoFiEngine;
class MyPlane;

class C_FOW : public Component {
public:
	C_FOW(GameObject* parent);
	~C_FOW();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool CleanUp() override;
	virtual bool InspectorDraw(PanelChooser* chooser) override;
	void CleanTexture();
	void GenerateTexture();
	void UpdateTexture();

	float2 GlobalToPixel(float3 global);
	float GlobalRadiusToPixel(float radius);

	void WriteCircle(float3 position, float radius, int value);

	MyPlane* drawablePlane = nullptr;

	unsigned int textureID = 0;

	bool isDirty = false;

	float2 size;
	unsigned char*** fowMap = nullptr;
};