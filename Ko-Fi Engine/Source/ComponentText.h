#pragma once

#include "Component.h"
#include "ComponentTransform2D.h"
#include "ComponentRenderedUI.h"

#include "R_Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string.h>
#include <vector>

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class SDL_Texture;
class SDL_Surface;
class MyPlane;

class ComponentText : public ComponentRenderedUI {
public:
	ComponentText(GameObject* parent);
	~ComponentText();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void SetTextValue(std::string newValue);
	std::string GetTextValue() { return textValue; };

	void Draw() override;

	GLuint openGLTexture = 0;

private:
	GLuint SurfaceToOpenGLTexture(SDL_Surface* surface);
	void FreeTextures();

	std::string textValue = "HELLO WORLD!";
};

