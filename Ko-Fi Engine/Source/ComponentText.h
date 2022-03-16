#pragma once

#include "Component.h"
#include "ComponentTransform2D.h"

#include "Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string.h>
#include <vector>

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class SDL_Texture;
class SDL_Surface;
class MyPlane;

class ComponentText : public Component {
public:
	ComponentText(GameObject* parent);
	~ComponentText();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void SetTextValue(std::string newValue);
	std::string GetTextValue() { return textValue; };

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	//SDL_Texture* SDLTexture = nullptr;
	GLuint openGLTexture = 0;

private:
	GLuint SurfaceToOpenGLTexture(SDL_Surface* surface);
	SDL_Texture* SurfaceToSDLTexture(SDL_Surface* surface);
	void FreeTextures();

	int texW = 0;
	int texH = 0;

	std::string textValue = "HELLO WORLD!";

	GLuint fboId = 0;

	MyPlane* drawablePlane = nullptr;
};

