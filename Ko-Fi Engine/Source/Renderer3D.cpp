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
#include "ImGuiAppLog.h"
#include "FileSystem.h"
#include "Texture.h"

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

	// Create context

	context = SDL_GL_CreateContext(engine->GetWindow()->window);
	if (context == NULL)
	{
		CONSOLE_LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		appLog->AddLog("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		// Use Vsync
		vsync = configModule.at("Vsync");
		SetVsync(vsync);

		// Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			appLog->AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		// Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			appLog->AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		// Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		// Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			CONSOLE_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			appLog->AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

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

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(engine->GetWindow()->GetWidth(), engine->GetWindow()->GetHeight());

	// Init the GLEW library
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		CONSOLE_LOG("Glew library could not be initiated! Glew Error: %s\n", glewGetErrorString(err));
		appLog->AddLog("Glew library could not be initiated! GLEW Error: %s\n", glewGetErrorString(err));
		ret = false;
	}

	if (err == GLEW_OK)
	{
		CONSOLE_LOG("Using Glew %s\n", glewGetString(GLEW_VERSION));
		appLog->AddLog("Using Glew %s\n", glewGetString(GLEW_VERSION));

		// Current hardware and driver capabilities
		CONSOLE_LOG("Vendor: %s", glGetString(GL_VENDOR));
		CONSOLE_LOG("Renderer: %s", glGetString(GL_RENDERER));
		CONSOLE_LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		CONSOLE_LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		appLog->AddLog("Vendor: %s\n", glGetString(GL_VENDOR));
		appLog->AddLog("Renderer: %s\n", glGetString(GL_RENDERER));
		appLog->AddLog("OpenGL version supported %s\n", glGetString(GL_VERSION));
		appLog->AddLog("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	return ret;
}

// PreUpdate: clear buffer
bool Renderer3D::PreUpdate(float dt)
{
	SDL_RenderClear(engine->GetUI()->renderer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());

	// light 0 on cam pos
	lights[0].SetPos(engine->GetCamera3D()->position.x, engine->GetCamera3D()->position.y, engine->GetCamera3D()->position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();
	return true;
}

// PostUpdate present buffer to screen
bool Renderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(engine->GetWindow()->window);
	SDL_SetRenderDrawColor(engine->GetUI()->renderer, 0, 0, 0, 0);
	//SDL_RenderPresent(engine->GetUI()->renderer);
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

void Renderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
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

void Renderer3D::AddParticle(Texture* tex, Color color, const float4x4 transform, float distanceToCamera)
{
	ParticleRenderer pRenderer = ParticleRenderer(tex, color, transform);
	pRenderer.shaderID = particleShader;
	particles.insert(std::map<float, ParticleRenderer>::value_type(distanceToCamera, pRenderer));
}

void Renderer3D::RenderAllParticles()
{
	for (auto particle : particles)
	{
		RenderParticle(&particle.second);
	}

	particles.clear();
}

ParticleRenderer::ParticleRenderer(Texture* tex, Color color, const float4x4 transform):
tex(tex),
color(color),
transform(transform),
VAO(0),
shaderID(0)
{

}

void ParticleRenderer::LoadBuffers()
{
	glGenBuffers(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(ParticlesCoords), ParticlesCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer3D::RenderParticle(ParticleRenderer* particle)
{
	//glEnable(GL_BLEND);
	//glEnable(GL_ALPHA_TEST);

	//glUseProgram(particle->shaderID);

	//if(particle->tex)
	//	glBindTexture(GL_TEXTURE_2D, particle->tex->GetTextureId());
	//else
	//	glDisable(GL_TEXTURE_2D);

	//// Matrices
	//GLint model_matrix = glGetUniformLocation(particle->shaderID, "modelMatrix");
	//glUniformMatrix4fv(model_matrix, 1, GL_FALSE, particle->transform.Transposed().ptr());

	//GLint projection_location = glGetUniformLocation(particle->shaderID, "projectionMatrix");
	//glUniformMatrix4fv(projection_location, 1, GL_FALSE, engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());

	//GLint view_location = glGetUniformLocation(particle->shaderID, "viewMatrix");
	//glUniformMatrix4fv(view_location, 1, GL_FALSE, engine->GetCamera3D()->viewMatrix.Transposed().ptr());

	//glUniform4fv(glGetUniformLocation(particle->shaderID, "color"), 1, (GLfloat*)&particle->color);

	//glBindBuffer(GL_ARRAY_BUFFER, particle->VAO);

	//glDrawArrays(GL_TRIANGLES, 0, 6);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);

	//glUseProgram(0);

	//glDisable(GL_BLEND);
	//glDisable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	
	if(particle->tex)
		glBindTexture(GL_TEXTURE_2D, particle->tex->GetTextureId());
	else
		glDisable(GL_TEXTURE_2D);

	glColor4f(particle->color.r, particle->color.g, particle->color.b, particle->color.a);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	//glMultMatrixf(engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());// proj
															// *
	glMultMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());// view
															// *
	glMultMatrixf(particle->transform.Transposed().ptr());	// model
	
	//Drawing to tris in direct mode
	glBegin(GL_TRIANGLES);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-.5f, -.5f, .0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(.5f, .5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_NORMAL_ARRAY, 0);
	//glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}
