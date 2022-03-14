#pragma once

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "GameObject.h"
#include "SDL_ttf.h"

#include "glew.h"
#include <vector>
#include "MathGeoLib/Geometry/Frustum.h"

#include "SDL.h"

class ComponentCamera;
class Texture;

class Shadert
{
public:
	unsigned int ID;

	Shadert();

	void Begin();
	void End();

private:
	void CheckCompileErrors(uint shader, std::string type);
};

class MyPlane
{
public:
	MyPlane();
	~MyPlane();
	void DrawPlane2D(Texture* texture);
	void DrawPlane2D(unsigned int texture);
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

	bool Start() override;
	bool PreUpdate(float dt) override;
	bool Update(float dt) override;
	bool PostUpdate(float dt) override;
	bool CleanUp() override;
	void OnNotify(const Event& event) override;

	void PrepareUIRender(GameObject* owner);
	void EndUIRender();

	float2 GetUINormalizedMousePosition();

	SDL_Renderer* renderer = nullptr;
	std::vector<SDL_Texture*> loadedTextures;

	void OnGui() override;
	//void OnLoad(const JSONReader& reader) override;
	//void OnSave(JSONWriter& writer) const override;
public:
	TTF_Font* rubik = nullptr;
	MyPlane* drawablePlane = nullptr;
private:
	KoFiEngine* engine = nullptr;

	float3 right;
	float3 up;
	float3 front;
	float3 position;
};