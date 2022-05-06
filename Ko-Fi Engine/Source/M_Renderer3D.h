#ifndef __RENDERER_3D_H__
#define __RENDERER_3D_H__

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL_video.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float3.h"

#include "MathGeoLib/Geometry/LineSegment.h"

#define MAX_LIGHTS 8

class GameObject;
typedef unsigned int GLenum;
class C_Mesh;
class C_Camera;
class R_Texture;
class PieShape;
class FocalLight;


struct ParticleRenderer
{
	ParticleRenderer(R_Texture& tex, Color color, const float4x4 transform);

	//void Render();

	R_Texture&	tex;
	Color		color;
	float4x4	transform;
};

class M_Renderer3D : public Module
{
public:
	M_Renderer3D(KoFiEngine* engine);
	~M_Renderer3D();

	bool Awake(Json configModule);
	bool PreUpdate(float dt);
	bool Update(float dt);
	bool PostUpdate(float dt);
	void SwapWindow();
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
	void PassPreviewProjectionAndViewToRenderer();
	void RecalculateProjectionMatrix();
	
	//Render Functions
	void RenderScene(C_Camera* camera);
	void RenderBoundingBox(C_Mesh* cMesh);
	void RenderMeshes(C_Camera* camera, GameObject* go);

	void RenderUI(GameObject* go);

	void OnNotify(const Event& event);

	bool GetVsync() const;
	void SetVsync(bool vsync);
	void OnResize();

	void DrawCylinder(float4x4 transform);
	void DrawCone(float3 position, float3 forward, float3 up, float angle, int length);

	// Debug ray for mouse picking
	void DrawRay();
	void SetRay(LineSegment ray);
	LineSegment GetRay();

	// Viewport frame buffer methods
	void InitFrameBuffers();
	void PrepareFrameBuffers();
	void UnbindFrameBuffers();
	void ResizeFrameBuffers(int width, int height);
	void ResizePreviewFrameBuffers(int width, int height);
	void ReleaseFrameBuffers();

	uint GetTextureBuffer();
	uint GetPreviewTextureBuffer();
	void AddParticle(R_Texture& tex, Color color, const float4x4 transform, float distanceToCamera);
	void RenderParticle(ParticleRenderer* particle);
	void RenderAllParticles();

	void InitDepthMapFramebufferAndTexture();
	void LightUniforms(uint shader);
	bool ShadowMapUniforms(C_Mesh* cMesh, uint shader);
	void FillShadowMap(C_Camera* camera);

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool isFirstPass = true;
	
	//Lights
	unsigned int depthMapFBO;
	unsigned int depthMapTexture;

private:
	bool vsync = false;
	KoFiEngine* engine = nullptr;

	// Debug ray for mouse picking
	LineSegment ray;
	float timeWaterShader = 0;

	// Viewport frame buffer
	uint frameBuffer = 0;
	uint previewFrameBuffer = 0;
	uint renderBufferoutput = 0;
	uint renderPreviewBufferoutput = 0;
	uint textureBuffer = 0;
	uint previewTextureBuffer = 0;
	bool show_viewport_window = true;

	//Particle Map
	std::map<float, ParticleRenderer> particles;

	
};

#endif // !__RENDERER_3D_H__