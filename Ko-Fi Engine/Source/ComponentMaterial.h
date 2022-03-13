#ifndef __COMPONENT_MATERIAL_H__
#define __COMPONENT_MATERIAL_H__

#include "Component.h"
#include "Texture.h"
#include <vector>
#include <string>

#define CHECKERS_HEIGHT 128
#define CHECKERS_WIDTH 128

using Json = nlohmann::json;

class PanelChooser;
class Material;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* parent);
	~ComponentMaterial();

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

#endif // !__COMPONENT_MATERIAL_H__