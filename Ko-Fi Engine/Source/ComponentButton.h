#pragma once

#include "Component.h"
#include "ComponentTransform2D.h"
#include "ComponentRenderedUI.h"

#include "R_Texture.h"

#include "glew.h"

#include "Globals.h"

#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float4.h"

class SDL_Texture;
class SDL_Surface;
class MyPlane;

class ComponentButton : public ComponentRenderedUI {
public:
	ComponentButton(GameObject* parent);
	~ComponentButton();

	void Save(Json& json) const override;
	void Load(Json& json) override;

	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	bool InspectorDraw(PanelChooser* chooser) override;

	void Draw() override;

	R_Texture GetIdleTexture() { return idleOpenGLTexture; };
	R_Texture GetHoverTexture() { return hoverOpenGLTexture; };
	R_Texture GetPressedTexture() { return pressedOpenGLTexture; };

	void SetIdleTexture(const char* path);
	void SetHoverTexture(const char* path);
	void SetPressedTexture(const char* path);

	bool IsPressed();
	bool IsHovered();

private:
	enum class BUTTON_STATE {
		IDLE,
		HOVER,
		PRESSED
	};

	void FreeTextures(BUTTON_STATE type);

	R_Texture idleOpenGLTexture;
	R_Texture hoverOpenGLTexture;
	R_Texture pressedOpenGLTexture;

	BUTTON_STATE state = BUTTON_STATE::IDLE;

	bool isPressed = false;
	bool isHovered = false;
};