#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL_video.h"
#define MAX_LIGHTS 8

class Window;
class Camera3D;
class Renderer3D : public Module
{
public:

	Renderer3D(Window* window,Camera3D* camera);
	~Renderer3D();

	bool Awake(Json configModule);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	bool GetVsync() const;
	void SetVsync(bool vsync);
	void OnResize(int width, int height);

	void draw1();

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	Window* window = nullptr;
	Camera3D* camera = nullptr;

private:

	bool vsync = false;
};