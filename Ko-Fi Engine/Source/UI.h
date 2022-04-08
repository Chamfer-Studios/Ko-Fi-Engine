#pragma once

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "SDL_ttf.h"

#include "glew.h"
#include <vector>
#include "MathGeoLib/Geometry/Frustum.h"

#include "SDL.h"

class C_Camera;
class Texture;
class GameObject;
class MyPlane
{
public:
	MyPlane(GameObject* owner);
	~MyPlane();
	void DrawPlane2D(Texture* texture, SDL_Color color);
	void DrawPlane2D(unsigned int texture, SDL_Color color);
public:
	unsigned int vertexBufferId = 0;
	unsigned int textureBufferId = 0;
	unsigned int indexBufferId = 0;
	GameObject* owner;
public:
	std::vector<unsigned int> indices;
	std::vector<float2> texCoords;
	std::vector<float3> vertices;
};

class UI : public Module
{
public:
	UI(KoFiEngine* engine);
	~UI();

	bool Awake(Json configModule);
	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	void OnNotify(const Event& event) override;


	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------

	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	void PrepareUIRender();
	void EndUIRender();

	float2 GetUINormalizedMousePosition();

	SDL_Renderer* renderer = nullptr;
	std::vector<SDL_Texture*> loadedTextures;

	void OnGui() override;
public:
	TTF_Font* rubik = nullptr;
private:
	KoFiEngine* engine = nullptr;

	float3 right;
	float3 up;
	float3 front;
	float3 position;
};