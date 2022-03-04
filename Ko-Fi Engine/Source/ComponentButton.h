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

class ComponentButton : public Component {
public:
	ComponentButton(GameObject* parent);
	~ComponentButton();

	bool Update() override;
	bool PostUpdate(float dt) override;
	bool InspectorDraw(PanelChooser* chooser) override;

	//SDL_Texture* LoadTexture(const char* path);
	//SDL_Texture* const LoadSurface(SDL_Surface* surface);

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

	Texture GetIdleTexture() { return idleOpenGLTexture; };
	Texture GetHoverTexture() { return hoverOpenGLTexture; };
	Texture GetPressedTexture() { return pressedOpenGLTexture; };

	void SetIdleTexture(const char* path);
	void SetHoverTexture(const char* path);
	void SetPressedTexture(const char* path);

private:
	enum class BUTTON_STATE {
		IDLE,
		HOVER,
		PRESSED
	};

	GLuint fboId = 0;

	void FreeTextures(BUTTON_STATE type);

	Texture idleOpenGLTexture;
	Texture hoverOpenGLTexture;
	Texture pressedOpenGLTexture;

	BUTTON_STATE state = BUTTON_STATE::IDLE;
};

