#include "Renderer3D.h"
#include "Primitive.h"

// OpenGL / GLEW
#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Log.h"
#include "Window.h"
#include "Engine.h"
#include "Camera3D.h"
#include "SceneManager.h"
#include "ViewportFrameBuffer.h"
#include "ImGuiAppLog.h"
#include "FileSystem.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "Material.h"


#include "UI.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Renderer3D::Renderer3D(KoFiEngine* engine) : Module()
{
	name = "Renderer3D";

	this->engine = engine;
}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Awake(Json configModule)
{
	CONSOLE_LOG("Creating 3D Renderer context");
	appLog->AddLog("Creating 3D Renderer context\n");
	bool ret = true;
	ret = InitOpenGL();
	OnResize();
	SetVsync(configModule["Vsync"].get<bool>());

	return ret;
}

// PreUpdate: clear buffer
bool Renderer3D::PreUpdate(float dt)
{
	bool ret = true;
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	Camera3D* currentCamera = engine->GetCamera3D();
	if (currentCamera)
	{
		if (currentCamera->projectionIsDirty) {
			RecalculateProjectionMatrix();
			currentCamera->CalculateViewMatrix();
		}
			
		glLoadMatrixf((GLfloat*)currentCamera->viewMatrix.Transposed().ptr());
	}
	float3 cameraPos = float3::zero;
	//TODO NEED TO CHANGE THIS TO engine->camera->currentcamera when the component camera can be set as camera.
	if (engine->GetCamera3D())
	{
		cameraPos = engine->GetCamera3D()->position;
	}
	else {
		cameraPos = float3(0.0f, 20.0f, 0.0f);
	}
	return ret;
}

// PostUpdate present buffer to screen
bool Renderer3D::PostUpdate(float dt)
{
	RenderScene();
	SDL_GL_SwapWindow(engine->GetWindow()->GetWindow());
	return true;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	CONSOLE_LOG("Destroying 3D Renderer");
	appLog->AddLog("Destroying 3D Renderer\n");

	SDL_GL_DeleteContext(context);

	return true;
}

bool Renderer3D::SaveConfiguration(Json& configModule) const
{
	configModule["Vsync"] = vsync;
	return true;
}

bool Renderer3D::LoadConfiguration(Json& configModule)
{
	return true;
}

bool Renderer3D::InitOpenGL()
{
	bool ret = true;
	context = SDL_GL_CreateContext(engine->GetWindow()->GetWindow());
	int contextMajorVersion, contextMinorVersion;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &contextMajorVersion);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &contextMinorVersion);
	if (!context)
	{
		CONSOLE_LOG("[ERROR] OpenGL context could not be created! SDL_ERROR: %s\n", SDL_GetError());
		ret = false;
	}
	ret = InitGlew();
	if (ret)
	{
		if (this->vsync) 
		{
			SDL_GL_SetSwapInterval(1) < 0 ?
				CONSOLE_LOG("[ERROR] Unable to set Vsync! SDL Error: %s\n", SDL_GetError()) : CONSOLE_LOG("[STATUS] Vsync is activated!");
		}
		else
		{
			SDL_GL_SetSwapInterval(0) < 0 ?
				CONSOLE_LOG("[ERROR] Unable to set frame update interval to immediate! SDL Error: %s\n", SDL_GetError()) : CONSOLE_LOG("[STATUS] Vsync is deactivated!");
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		SetGLFlag(GL_DEPTH_TEST, true);
		SetGLFlag(GL_CULL_FACE, true);
		lights[0].Active(true);
		SetGLFlag(GL_LIGHTING, true);
		SetGLFlag(GL_COLOR_MATERIAL, true);
		SetGLFlag(GL_TEXTURE_2D, true);



		GLenum err = glGetError();
		while (err != GL_NO_ERROR)
		{
			CONSOLE_LOG("OpenGl error: %d", err);
			err = glGetError();
		}
	}

	return ret;
}

bool Renderer3D::InitGlew()
{
	bool ret = true;

	// Initializing glew.
	GLuint GLEWerr = glewInit();
	if (GLEWerr != GLEW_OK)
	{
		CONSOLE_LOG("[ERROR] GLEW could not initialize!: %s\n", glewGetErrorString(GLEWerr));
		ret = false;
	}
	return ret;
}

void Renderer3D::SetGLFlag(GLenum flag, bool setTo)
{
	if (setTo != (bool)glIsEnabled(flag))
	{
		setTo ? glEnable(flag) : glDisable(flag);
	}
}

void Renderer3D::RecalculateProjectionMatrix()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (engine->GetCamera3D())
	{
		glLoadMatrixf((GLfloat*)engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	}
	else
	{
		CONSOLE_LOG("[ERROR] Renderer3D: Could not recalculate the projection matrix!Error : Current Camera was nullptr.");
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Renderer3D::RenderScene()
{
	for (GameObject* go : engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
	{
		ComponentMesh* cMesh = go->GetComponent<ComponentMesh>();
		if (cMesh)
		{
			RenderMeshes(go);
		}
	}
}

void Renderer3D::RenderMeshes(GameObject* go)
{
	//Get needed variables
	ComponentMaterial* cMat = go->GetComponent<ComponentMaterial>();
	ComponentMesh* cMesh = go->GetComponent<ComponentMesh>();
	Mesh* mesh = cMesh->GetMesh();
	//Check textures
	if (cMat && mesh)
	{
		if (!cMat->active)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, cMat->texture.GetTextureId());
		}
	}
	//Set Shaders
	if (cMesh->GetRenderMesh())
	{
		uint shader = cMat->GetMaterial()->shaderProgramID;
		if (shader != 0)
		{
			glUseProgram(shader);
			// Passing Shader Uniforms
			GLint model_matrix = glGetUniformLocation(shader, "model_matrix");
			glUniformMatrix4fv(model_matrix, 1, GL_FALSE, cMesh->owner->GetTransform()->GetGlobalTransform().Transposed().ptr());
			GLint view_location = glGetUniformLocation(shader, "view");
			glUniformMatrix4fv(view_location, 1, GL_FALSE, engine->GetCamera3D()->viewMatrix.Transposed().ptr());


			GLint projection_location = glGetUniformLocation(shader, "projection");
			glUniformMatrix4fv(projection_location, 1, GL_FALSE, engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());

			GLint refractTexCoord = glGetUniformLocation(shader, "refractTexCoord");
			glUniformMatrix4fv(refractTexCoord, 1, GL_FALSE, engine->GetCamera3D()->viewMatrix.Transposed().ptr());

			float2 resolution = float2(1080.0f, 720.0f);
			glUniform2fv(glGetUniformLocation(shader, "resolution"), 1, resolution.ptr());

			this->timeWaterShader += 0.02f;
			glUniform1f(glGetUniformLocation(shader, "time"), this->timeWaterShader);
			//Pass all varibale uniforms from the material to the shader
			for (Uniform* uniform : cMat->GetMaterial()->uniforms)
			{
				switch (uniform->type)
				{
				case GL_INT:
				{
					glUniform1d(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<int>*)uniform)->value);
				}
				break;
				case GL_FLOAT:
				{
					glUniform1f(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<float>*)uniform)->value);
				}
				break;
				case GL_BOOL:
				{
					glUniform1d(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<bool>*)uniform)->value);
				}
				break;
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
					glUniform2fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf2->value.ptr());
				}
				break;
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
					glUniform3fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf3->value.ptr());
				}
				break;
				case GL_FLOAT_VEC4:
				{
					UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
					glUniform4fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf4->value.ptr());
				}
				break;
				default:
					break;
				}
			}
			//Draw Mesh
			mesh->Draw();
			glUseProgram(0);
		}
	}
}

// Method to receive and manage events
void Renderer3D::OnNotify(const Event& event)
{
	// Manage events
}

bool Renderer3D::GetVsync() const
{
	return vsync;
}

void Renderer3D::SetVsync(bool vsync)
{
	if (this->vsync != vsync)
	{
		this->vsync = vsync;
		if (SDL_GL_SetSwapInterval(vsync ? 1 : 0) < 0) {
			CONSOLE_LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			appLog->AddLog("Warning: Unable to set VSync! SDL Error: %s\n",SDL_GetError());
		}
		
		SDL_GL_GetSwapInterval() ? appLog->AddLog("Vsync Started\n") : appLog->AddLog("Vsync Stopped\n");;
	}
}

void Renderer3D::OnResize()
{
	glViewport(0, 0, engine->GetWindow()->GetWidth(), engine->GetWindow()->GetHeight());
	if (engine->GetCamera3D())
	{
		//engine->GetCamera3D()->aspectRatio = engine->GetWindow()->GetWidth() / engine->GetWindow()->GetHeight();
		engine->GetCamera3D()->SetAspectRatio(engine->GetWindow()->GetWidth() / engine->GetWindow()->GetHeight());
	}
	else
	{
		CONSOLE_LOG("[ERROR] Renderer 3D: Could not recalculate the aspect ratio! Error: Current Camera was nullptr.");

	}
	RecalculateProjectionMatrix();

}

// Debug ray for mouse picking
void Renderer3D::DrawRay()
{
	glColor3f(0.0f, 1.0f, 1.0f);
	glLineWidth(3.0f);

	glBegin(GL_LINES);
	glVertex3f(ray.a.x, ray.a.y, ray.a.z);
	glVertex3f(ray.b.x, ray.b.y, ray.b.z);
	glEnd();

	glLineWidth(1.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void Renderer3D::SetRay(LineSegment ray)
{
	this->ray = ray;
}

LineSegment Renderer3D::GetRay()
{
	return ray;
}
