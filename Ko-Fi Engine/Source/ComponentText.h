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

class ComponentText : public Component {
public:
	ComponentText(GameObject* parent);
	~ComponentText();

	bool Update() override;
	bool PostUpdate(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void SetTextValue(std::string newValue);

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	SDL_Texture* SDLTexture;
	GLuint openGLTexture;

private:
	GLuint SurfaceToOpenGLTexture(SDL_Surface* surface);
	SDL_Texture* SurfaceToSDLTexture(SDL_Surface* surface);

	std::string textValue = "HELLO WORLD!";
};

