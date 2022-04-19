#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

using Json = nlohmann::json;

class PanelChooser;
class R_Material;
class R_Texture;
class vector;
class string;
class GameObject;

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
	//std::vector<R_Texture> textures;

private:
	R_Material* material = nullptr;

	int currentTextureId = 0;
};

#endif // !__C_MATERIAL_H__