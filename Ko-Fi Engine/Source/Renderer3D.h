#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "SDL/include/SDL_video.h"
#define MAX_LIGHTS 8
class Window;
class Camera3D;
class Renderer3D : public Module
{
public:
	Renderer3D(Window* window,Camera3D* camera);
	~Renderer3D();

	bool Awake();
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
	Window* window = nullptr;
	Camera3D* camera = nullptr;
};