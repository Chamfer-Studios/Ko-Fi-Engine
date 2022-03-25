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
typedef unsigned int GLenum;
class ComponentMesh;
class Renderer3D : public Module
{
public:
	Renderer3D(KoFiEngine* engine);
	~Renderer3D();

	bool Awake(Json configModule);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	bool CleanUp();

	// Engine config serialization --------------------------------------
	bool SaveConfiguration(Json& configModule) const override;
	bool LoadConfiguration(Json& configModule) override;
	// ------------------------------------------------------------------
	
	// Engine config inspector draw -------------------------------------
	bool InspectorDraw() override;
	// ------------------------------------------------------------------

	// Method to receive and manage events
	//Renderer Set Up Functions
	bool InitOpenGL();
	bool InitGlew();
	void SetGLFlag(GLenum flag, bool setTo);
	void PassProjectionAndViewToRenderer();
	void RecalculateProjectionMatrix();
	
	//Render Functions
	void RenderScene();
	void RenderBoundingBox(ComponentMesh* cMesh);
	void RenderMeshes(GameObject* go);

	void OnNotify(const Event& event);

	bool GetVsync() const;
	void SetVsync(bool vsync);
	void OnResize();

	// Debug ray for mouse picking
	void DrawRay();
	void SetRay(LineSegment ray);
	LineSegment GetRay();

	// Viewport frame buffer methods
	void InitFrameBuffers();
	void PrepareFrameBuffers();
	void UnbindFrameBuffers();
	void ResizeFrameBuffers(int width, int height);
	void ReleaseFrameBuffers();

	uint GetTextureBuffer();

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
	float timeWaterShader = 0;

	// Viewport frame buffer
	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint textureBuffer = 0;
	bool show_viewport_window = true;
};

#endif // !__RENDERER_3D_H__