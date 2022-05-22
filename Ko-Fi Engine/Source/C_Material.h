#pragma once

#include "Component.h"

using Json = nlohmann::json;

class PanelChooser;
class R_Material;
class R_Texture;
class vector;
class string;
class GameObject;
class Component;

class C_Material : public Component
{
public:
	C_Material(GameObject* parent);
	~C_Material();

	bool Update(float dt) override;
	bool CleanUp() override;

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void SetMaterial(R_Material* material);
	inline R_Material* GetMaterial() const { return material; }

	void LoadMaterial(const char* path = "");

	bool InspectorDraw(PanelChooser* chooser) override;

	bool LoadDefaultMaterial();

public:
	R_Texture* texture = nullptr;

	bool checkerTexture = false;

private:
	R_Material* material = nullptr;

	int currentTextureId = 0;
};