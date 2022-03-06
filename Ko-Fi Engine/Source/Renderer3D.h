#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL_video.h"

#include "MathGeoLib/Geometry/LineSegment.h"

#define MAX_LIGHTS 8

class GameObject;

class Renderer3D : public Module
{
public:
	Renderer3D(KoFiEngine* engine);
	~Renderer3D();

	bool Awake(Json configModule);
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);

	bool GetVsync() const;
	void SetVsync(bool vsync);
	void OnResize(int width, int height);

	// Debug ray for mouse picking
	void DrawRay();
	void SetRay(LineSegment ray);
	LineSegment GetRay();

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	bool vsync = false;
	KoFiEngine* engine = nullptr;

	// Debug ray for mouse picking
	LineSegment ray;
};

#endif // !__RENDERER_3D_H__