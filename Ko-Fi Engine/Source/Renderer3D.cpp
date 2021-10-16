#include "Renderer3D.h"
#include "Primitive.h"

// OpenGL / GLEW
#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include "Log.h"
#include "Window.h"
#include "Camera3D.h"
#include "ImGuiAppLog.h"
#include "FileLoader.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Renderer3D::Renderer3D(Window* window, Camera3D* camera, FileLoader* fileLoader) : Module()
{
	name = "Renderer3D";

	this->window = window;
	this->camera = camera;
	this->fileLoader = fileLoader;
}

// Destructor
Renderer3D::~Renderer3D()
{}

// Called before render is available
bool Renderer3D::Awake(Json configModule)
{
	LOG("Creating 3D Renderer context");
	appLog->AddLog("Creating 3D Renderer context\n");
	bool ret = true;

	// Create context
	context = SDL_GL_CreateContext(window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
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
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
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
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
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
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
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
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	// Init the GLEW library
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		LOG("Glew library could not be initiated! Glew Error: %s\n", glewGetErrorString(err));
		appLog->AddLog("Glew library could not be initiated! GLEW Error: %s\n", glewGetErrorString(err));
		ret = false;
	}

	if (err == GLEW_OK)
	{
		LOG("Using Glew %s\n", glewGetString(GLEW_VERSION));
		appLog->AddLog("Using Glew %s\n", glewGetString(GLEW_VERSION));

		// Current hardware and driver capabilities
		LOG("Vendor: %s", glGetString(GL_VENDOR));
		LOG("Renderer: %s", glGetString(GL_RENDERER));
		LOG("OpenGL version supported %s", glGetString(GL_VERSION));
		LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		appLog->AddLog("Vendor: %s\n", glGetString(GL_VENDOR));
		appLog->AddLog("Renderer: %s\n", glGetString(GL_RENDERER));
		appLog->AddLog("OpenGL version supported %s\n", glGetString(GL_VERSION));
		appLog->AddLog("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	InitMeshes(fileLoader->meshes);

	return ret;
}

// PreUpdate: clear buffer
bool Renderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(camera->Position.x, camera->Position.y, camera->Position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return true;
}

// Update
bool Renderer3D::Update(float dt)
{
	return true;
}

// PostUpdate present buffer to screen
bool Renderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(window->window);
	return true;
}

// Called before quitting
bool Renderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	appLog->AddLog("Destroying 3D Renderer\n");

	SDL_GL_DeleteContext(context);

	return true;
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
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
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

void Renderer3D::InitMeshes(std::vector<Mesh> meshes)
{
	std::vector<Mesh>::iterator item = meshes.begin();
	while (item != fileLoader->meshes.end())
	{
		InitMesh((Mesh)*item);
		++item;
	}
}

void Renderer3D::InitMesh(Mesh mesh)
{
	/*glGenBuffers(1, &mesh.id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertices * 3, mesh.vertices, GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_indices, mesh.indices, GL_STATIC_DRAW);*/
}

void Renderer3D::DrawMesh(Mesh mesh)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	/*glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);*/
	glVertexPointer(3, GL_FLOAT, 0, mesh.vertices);
	// … bind and use other buffers
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, NULL);*/
	glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, mesh.indices);
	glDisableClientState(GL_VERTEX_ARRAY);
}
