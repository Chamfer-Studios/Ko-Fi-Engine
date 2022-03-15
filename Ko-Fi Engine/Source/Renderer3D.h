#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL_video.h"
#include "MathGeoLib/Math/float4x4.h"

#include "MathGeoLib/Geometry/LineSegment.h"

#define MAX_LIGHTS 8

class GameObject;
class Texture;

const float ParticlesCoords[] = {
1, 1,
1, 0,
0, 0,
1, 0,
};

struct ParticleRenderer
{
	ParticleRenderer(Texture* tex, Color color, const float4x4 transform);

	//void Render();
	void LoadBuffers(); 

	uint		VAO;
	uint		shaderID;

	Texture*	tex;
	Color		color;
	float4x4	transform;
};

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

	void AddParticle(Texture* tex, Color color, const float4x4 transform, float distanceToCamera);
	void RenderParticle(ParticleRenderer* particle);
	void RenderAllParticles();

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

	//Particle Map
	uint particleShader = 0;
	std::map<float, ParticleRenderer> particles;
};

#endif // !__RENDERER_3D_H__