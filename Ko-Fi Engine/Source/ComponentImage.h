#ifndef _COMPONENT_IMAGE_H_
#define _COMPONENT_IMAGE_H_

#include "Component.h"
#include "ComponentTransform2D.h"

#include "Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string.h>
#include <vector>

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class ComponentMesh;
class TMPPlane;
struct TextureObject;
class SDL_Texture;
class SDL_Surface;

class ComponentImage : public Component {
public:
	ComponentImage(GameObject* parent);
	~ComponentImage();

	bool Update() override;
	bool PostUpdate(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	SDL_Texture* LoadTexture(const char* path);
	SDL_Texture* const LoadSurface(SDL_Surface* surface);


	float4x4 GetTransform();
	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	TMPPlane* plane = nullptr;

	float4 imageColor = { 1.000f,1.000f, 1.000f, 1.000f };
	SDL_Texture* SDLTexture = nullptr;
	Texture openGLTexture;
private:
	float4x4 transform3D;

	std::string texturePath;
	uint textureBufferId = 0;
	float time = 0;

};

#endif

