#pragma once

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "SDL_video.h"
#include "MathGeoLib/Math/float4x4.h"
#include "MathGeoLib/Math/float3x3.h"
#include "MathGeoLib/Math/float3.h"

#include "MathGeoLib/Geometry/LineSegment.h"
#include <vector>
#include <set>
#define MAX_LIGHTS 8

class GameObject;
typedef unsigned int GLenum;
typedef unsigned int GLuint;
typedef int GLint;
class C_Mesh;
class R_Material;
class C_Camera;
class R_Texture;
class PieShape;
class FocalLight;
class SkyBox;

struct ParticleRenderer
{
	ParticleRenderer(R_Texture& tex, Color color, const float4x4 transform);

	//void Render();

	R_Texture&	tex;
	Color		color;
	float4x4	transform;
};

class OcclusionQuery
{
public:
	OcclusionQuery();

	~OcclusionQuery();
	void BeginQuery() const;
	void EndQuery();
	GLint GetNumSamplesPassed()const;
	GLint GetResultAvilable()const;
	bool AnySamplesPassed()const;

private:
	GLuint queryID =  0 ; // OpenGL query object ID
	GLint samplesPassed = 0; // Number of samples passed in last query
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
	void QueryScene1(C_Camera* camera);
	void QueryScene2(C_Camera* camera);
	void RenderBoundingBox(C_Mesh* cMesh);
	void RenderMeshes(C_Camera* camera, GameObject* go);
	void RenderMeshesQuery(C_Camera* camera, GameObject* go,int queryPosition);
	void RenderSkyBox(C_Camera* camera, SkyBox &skybox);

	void RenderUI(GameObject* go);

	void OnNotify(const Event& event);

	bool GetVsync() const;
	void SetVsync(bool vsync);
	void OnResize();

	void DrawCylinder(float4x4 transform);
	void DrawCone(float3 position, float3 forward, float3 up, float angle, int length);
	void DrawCircle(float3 position, float radius);

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
	void ShadowMapUniforms(C_Mesh* cMesh, uint shader, GameObject* light);
	void FillShadowMap(C_Camera* camera);
	bool renderShadowMap;

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	bool isFirstPass = true;
	bool enableOcclusionCulling = false;
	
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
	//Occlusion Culling things
	OcclusionQuery* query = nullptr;
	R_Material* occlusionMat = nullptr;

	struct GOComp
	{
		// uses forward decl from before in arguments. since we're
		//  using pointers, no other type info is required. we don't
		//  actually implement this yet (we can't, we don't know what
		//  "base" really is yet).
		bool operator ()(const GameObject* lhs, const GameObject* rhs) const;
	};

	std::set<GameObject*, GOComp> gameObejctsToRenderDistanceOrdered;
};