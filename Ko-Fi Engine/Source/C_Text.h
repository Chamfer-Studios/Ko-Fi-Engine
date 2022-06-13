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
	virtual ~C_Text();

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

	inline float GetOpacity() const { return (float)col.a; }
	inline void SetOpacity(const float opacity) { col.a = opacity; }

	GLuint openGLTexture = 0;

	void SetColor(SDL_Color color);
	inline void SetColorRGB(const int r, const int g, const int b) { col.r = (unsigned int)r; col.g = (unsigned int)g; col.b = (unsigned int)b; }
private:
	GLuint SurfaceToOpenGLTexture(SDL_Surface* surface);
	void FreeTextures();

	SDL_Color GetColor();


	bool ColorPicker(const char* label);

	std::string textValue = "HELLO WORLD!";

	TTF_Font* selectedFont = nullptr;
	SDL_Color col = { 255, 255, 255, 255 };
	std::string fontPath = "Assets/Fonts/Rubik_Mono_One/RubikMonoOne-Regular.ttf";
	int size = 0;
};

