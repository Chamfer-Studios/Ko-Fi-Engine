#ifndef __C_MATERIAL_H__
#define __C_MATERIAL_H__

#include "Component.h"
#include "Texture.h"
#include <vector>
#include <string>

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

using Json = nlohmann::json;

class PanelChooser;
class Material;

class C_Material : public Component
{
public:
	C_Material(GameObject* parent);
	~C_Material();

	bool Update(float dt) override;
	bool CleanUp() override;

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void SetMaterial(Material* material);
	inline Material* GetMaterial() const { return material; }

	void LoadMaterial(const char* path = "");

	bool InspectorDraw(PanelChooser* chooser) override;

	bool LoadDefaultMaterial();

public:
	Texture texture;
	//std::vector<Texture> textures;

private:
	Material* material = nullptr;

	int currentTextureId = 0;
};

#endif // !__C_MATERIAL_H__