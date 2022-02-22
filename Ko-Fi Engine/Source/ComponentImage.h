#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "Component.h"
#include "ComponentTransform2D.h"

#include "Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string.h>
#include <vector>

class ComponentMesh;
struct TextureObject;

class ComponentImage : public Component {
public:
	ComponentImage(GameObject* parent);
	~ComponentImage();

	bool Update() override;
	bool InspectorDraw(PanelChooser* chooser) override;

	float4x4 GetTransform();
	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	void AddPlaneToMesh(ComponentMesh*);

	ComponentMesh* plane = nullptr;

	//float4 imageColor = { 1.000f,1.000f, 1.000f, 1.000f };
	Texture texture;
private:
	std::string texturePath;
	uint textureBufferId = 0;

};

#endif

