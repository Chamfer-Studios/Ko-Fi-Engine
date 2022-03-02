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

	SDL_Texture* LoadTexture(const char* path);
	SDL_Texture* const LoadSurface(SDL_Surface* surface);

	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;

private:
	enum class BUTTON_STATE {
		IDLE,
		HOVER,
		PRESSED
	};

	GLuint fboId = 0;

	void FreeTextures(BUTTON_STATE type);

	SDL_Texture* idleSDLTexture = nullptr;
	Texture idleOpenGLTexture;

	SDL_Texture* hoverSDLTexture = nullptr;
	Texture hoverOpenGLTexture;

	SDL_Texture* pressedSDLTexture = nullptr;
	Texture pressedOpenGLTexture;

	BUTTON_STATE state = BUTTON_STATE::IDLE;
};

