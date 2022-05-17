#pragma once

#include "Component.h"
#include "C_Transform2D.h"
#include "C_RenderedUI.h"

#include "SDL_ttf.h"

#include "R_Texture.h"

#include "glew.h"

#include "Globals.h"
#include <string>
#include <vector>

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class SDL_Texture;
class SDL_Surface;
class MyPlane;

class C_Text : public C_RenderedUI {
public:
	C_Text(GameObject* parent);
	~C_Text();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void SetTextValue(std::string newValue);
	void SetFont(std::string path);
	void SetSize(int size);
	std::string GetTextValue() { return textValue; };

	void Draw() override;

	GLuint openGLTexture = 0;

private:
	GLuint SurfaceToOpenGLTexture(SDL_Surface* surface);
	void FreeTextures();

	SDL_Color GetColor();

	void SetColor(SDL_Color color);

	bool ColorPicker(const char* label);

	std::string textValue = "HELLO WORLD!";

	TTF_Font* selectedFont = nullptr;
	SDL_Color col = { 255, 255, 255, 255 };
	std::string fontPath = "Assets/Fonts/Rubik_Mono_One/RubikMonoOne-Regular.ttf";
	int size = 0;
};

