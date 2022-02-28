#pragma once

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "GameObject.h"
#include "SDL_ttf.h"


#include "glew.h"
#include <vector>

#include "SDL.h"

class ComponentCamera;

class UI : public Module
{
public:
	UI(KoFiEngine* engine);
	~UI();

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;

	float2 GetUINormalizedMousePosition();

	SDL_Renderer* renderer = nullptr;
	std::vector<SDL_Texture*> loadedTextures;

	void OnGui() override;
	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;
public:
	GLint uiCameraViewport[4] = { 0, 0, 0, 0 };
	TTF_Font* rubik = nullptr;
private:
	KoFiEngine* engine = nullptr;


};