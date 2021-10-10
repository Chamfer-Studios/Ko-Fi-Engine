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

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

Renderer3D::Renderer3D(Window* window,Camera3D* camera) : Module()
{
	name = "Renderer3D";

	this->window = window;
	this->camera = camera;
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
	bool ret = true;

	// draw a line 10 units upwards
	/*glLineWidth(2.0f);

	glBegin(GL_LINES);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 10.f, 0.f);
	glEnd();

	glLineWidth(1.0f);*/

    // Draw cube (with each different method)
    draw1();
    draw2();
    draw3();
    draw4();
    draw5();

    // Back to default white
    glColor3ub(255, 255, 255);

    // Draw sphere
    Sphere sphere(1, 25, 25);
    sphere.InnerRender(0,0,0);

	return ret;
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

// cube ///////////////////////////////////////////////////////////////////////
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// vertex coords array for glDrawArrays() =====================================
// A cube has 6 sides and each side has 2 triangles, therefore, a cube consists
// of 36 vertices (6 sides * 2 tris * 3 vertices = 36 vertices). And, each
// vertex is 3 components (x,y,z) of floats, therefore, the size of vertex
// array is 108 floats (36 * 3 = 108).
GLfloat vertices1[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,      // v0-v1-v2 (front)
                       -1,-1, 1,   1,-1, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1,-1, 1,   1,-1,-1,      // v0-v3-v4 (right)
                        1,-1,-1,   1, 1,-1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   1, 1,-1,  -1, 1,-1,      // v0-v5-v6 (top)
                       -1, 1,-1,  -1, 1, 1,   1, 1, 1,      // v6-v1-v0

                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,      // v1-v6-v7 (left)
                       -1,-1,-1,  -1,-1, 1,  -1, 1, 1,      // v7-v2-v1

                       -1,-1,-1,   1,-1,-1,   1,-1, 1,      // v7-v4-v3 (bottom)
                        1,-1, 1,  -1,-1, 1,  -1,-1,-1,      // v3-v2-v7

                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,      // v4-v7-v6 (back)
                       -1, 1,-1,   1, 1,-1,   1,-1,-1 };    // v6-v5-v4

// normal array
GLfloat normals1[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,      // v0-v1-v2 (front)
                        0, 0, 1,   0, 0, 1,   0, 0, 1,      // v2-v3-v0

                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v0-v3-v4 (right)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,      // v4-v5-v0

                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,      // v6-v1-v0

                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v1-v6-v7 (left)
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,      // v7-v2-v1

                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v7-v4-v3 (bottom)
                        0,-1, 0,   0,-1, 0,   0,-1, 0,      // v3-v2-v7

                        0, 0,-1,   0, 0,-1,   0, 0,-1,      // v4-v7-v6 (back)
                        0, 0,-1,   0, 0,-1,   0, 0,-1 };    // v6-v5-v4

// color array
GLfloat colors1[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,      // v0-v1-v2 (front)
                        1, 0, 0,   1, 0, 1,   1, 1, 1,      // v2-v3-v0

                        1, 1, 1,   1, 0, 1,   0, 0, 1,      // v0-v3-v4 (right)
                        0, 0, 1,   0, 1, 1,   1, 1, 1,      // v4-v5-v0

                        1, 1, 1,   0, 1, 1,   0, 1, 0,      // v0-v5-v6 (top)
                        0, 1, 0,   1, 1, 0,   1, 1, 1,      // v6-v1-v0

                        1, 1, 0,   0, 1, 0,   0, 0, 0,      // v1-v6-v7 (left)
                        0, 0, 0,   1, 0, 0,   1, 1, 0,      // v7-v2-v1

                        0, 0, 0,   0, 0, 1,   1, 0, 1,      // v7-v4-v3 (bottom)
                        1, 0, 1,   1, 0, 0,   0, 0, 0,      // v3-v2-v7

                        0, 0, 1,   0, 0, 0,   0, 1, 0,      // v4-v7-v6 (back)
                        0, 1, 0,   0, 1, 1,   0, 0, 1 };    // v6-v5-v4



// vertex array for glDrawElements() and glDrawRangeElement() =================
// Notice that the sizes of these arrays become samller than the arrays for
// glDrawArrays() because glDrawElements() uses an additional index array to
// choose designated vertices with the indices. The size of vertex array is now
// 24 instead of 36, but the index array size is 36, same as the number of
// vertices required to draw a cube.
GLfloat vertices2[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
                        1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
                        1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
                       -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
                       -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
                        1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)

// normal array
GLfloat normals2[]  = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
                        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
                        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
                       -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
                        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

// color array
GLfloat colors2[]   = { 1, 1, 1,   1, 1, 0,   1, 0, 0,   1, 0, 1,   // v0,v1,v2,v3 (front)
                        1, 1, 1,   1, 0, 1,   0, 0, 1,   0, 1, 1,   // v0,v3,v4,v5 (right)
                        1, 1, 1,   0, 1, 1,   0, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
                        1, 1, 0,   0, 1, 0,   0, 0, 0,   1, 0, 0,   // v1,v6,v7,v2 (left)
                        0, 0, 0,   0, 0, 1,   1, 0, 1,   1, 0, 0,   // v7,v4,v3,v2 (bottom)
                        0, 0, 1,   0, 0, 0,   0, 1, 0,   0, 1, 1 }; // v4,v7,v6,v5 (back)

// index array of vertex array for glDrawElements() & glDrawRangeElement()
GLubyte indices[]  = { 0, 1, 2,   2, 3, 0,      // front
                       4, 5, 6,   6, 7, 4,      // right
                       8, 9,10,  10,11, 8,      // top
                      12,13,14,  14,15,12,      // left
                      16,17,18,  18,19,16,      // bottom
                      20,21,22,  22,23,20 };    // back



// interleaved vertex array for glDrawElements() & glDrawRangeElements() ======
// All vertex attributes (position, normal, color) are packed together as a
// struct or set, for example, ((V,N,C), (V,N,C), (V,N,C),...).
// It is called an array of struct, and provides better memory locality.
GLfloat vertices3[] = { 1, 1, 1,   0, 0, 1,   1, 1, 1,              // v0 (front)
                       -1, 1, 1,   0, 0, 1,   1, 1, 0,              // v1
                       -1,-1, 1,   0, 0, 1,   1, 0, 0,              // v2
                        1,-1, 1,   0, 0, 1,   1, 0, 1,              // v3

                        1, 1, 1,   1, 0, 0,   1, 1, 1,              // v0 (right)
                        1,-1, 1,   1, 0, 0,   1, 0, 1,              // v3
                        1,-1,-1,   1, 0, 0,   0, 0, 1,              // v4
                        1, 1,-1,   1, 0, 0,   0, 1, 1,              // v5

                        1, 1, 1,   0, 1, 0,   1, 1, 1,              // v0 (top)
                        1, 1,-1,   0, 1, 0,   0, 1, 1,              // v5
                       -1, 1,-1,   0, 1, 0,   0, 1, 0,              // v6
                       -1, 1, 1,   0, 1, 0,   1, 1, 0,              // v1

                       -1, 1, 1,  -1, 0, 0,   1, 1, 0,              // v1 (left)
                       -1, 1,-1,  -1, 0, 0,   0, 1, 0,              // v6
                       -1,-1,-1,  -1, 0, 0,   0, 0, 0,              // v7
                       -1,-1, 1,  -1, 0, 0,   1, 0, 0,              // v2

                       -1,-1,-1,   0,-1, 0,   0, 0, 0,              // v7 (bottom)
                        1,-1,-1,   0,-1, 0,   0, 0, 1,              // v4
                        1,-1, 1,   0,-1, 0,   1, 0, 1,              // v3
                       -1,-1, 1,   0,-1, 0,   1, 0, 0,              // v2

                        1,-1,-1,   0, 0,-1,   0, 0, 1,              // v4 (back)
                       -1,-1,-1,   0, 0,-1,   0, 0, 0,              // v7
                       -1, 1,-1,   0, 0,-1,   0, 1, 0,              // v6
                        1, 1,-1,   0, 0,-1,   0, 1, 1 };            // v5



///////////////////////////////////////////////////////////////////////////////
// draw 1: immediate mode
// 78 calls = 36 glVertex*() calls + 36 glColor*() calls + 6 glNormal*() calls
///////////////////////////////////////////////////////////////////////////////
void Renderer3D::draw1()
{
    glPushMatrix();
        glTranslatef(-2, 2, 0); // move to upper left corner
        glBegin(GL_TRIANGLES);
        // front faces
        glNormal3f(0, 0, 1);
        // face v0-v1-v2
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);
        glColor3f(1, 1, 0);
        glVertex3f(-1, 1, 1);
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1, 1);
        // face v2-v3-v0
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1, 1);
        glColor3f(1, 0, 1);
        glVertex3f(1, -1, 1);
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);

        // right faces
        glNormal3f(1, 0, 0);
        // face v0-v3-v4
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);
        glColor3f(1, 0, 1);
        glVertex3f(1, -1, 1);
        glColor3f(0, 0, 1);
        glVertex3f(1, -1, -1);
        // face v4-v5-v0
        glColor3f(0, 0, 1);
        glVertex3f(1, -1, -1);
        glColor3f(0, 1, 1);
        glVertex3f(1, 1, -1);
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);

        // top faces
        glNormal3f(0, 1, 0);
        // face v0-v5-v6
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);
        glColor3f(0, 1, 1);
        glVertex3f(1, 1, -1);
        glColor3f(0, 1, 0);
        glVertex3f(-1, 1, -1);
        // face v6-v1-v0
        glColor3f(0, 1, 0);
        glVertex3f(-1, 1, -1);
        glColor3f(1, 1, 0);
        glVertex3f(-1, 1, 1);
        glColor3f(1, 1, 1);
        glVertex3f(1, 1, 1);

        // left faces
        glNormal3f(-1, 0, 0);
        // face  v1-v6-v7
        glColor3f(1, 1, 0);
        glVertex3f(-1, 1, 1);
        glColor3f(0, 1, 0);
        glVertex3f(-1, 1, -1);
        glColor3f(0, 0, 0);
        glVertex3f(-1, -1, -1);
        // face v7-v2-v1
        glColor3f(0, 0, 0);
        glVertex3f(-1, -1, -1);
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1, 1);
        glColor3f(1, 1, 0);
        glVertex3f(-1, 1, 1);

        // bottom faces
        glNormal3f(0, -1, 0);
        // face v7-v4-v3
        glColor3f(0, 0, 0);
        glVertex3f(-1, -1, -1);
        glColor3f(0, 0, 1);
        glVertex3f(1, -1, -1);
        glColor3f(1, 0, 1);
        glVertex3f(1, -1, 1);
        // face v3-v2-v7
        glColor3f(1, 0, 1);
        glVertex3f(1, -1, 1);
        glColor3f(1, 0, 0);
        glVertex3f(-1, -1, 1);
        glColor3f(0, 0, 0);
        glVertex3f(-1, -1, -1);

        // back faces
        glNormal3f(0, 0, -1);
        // face v4-v7-v6
        glColor3f(0, 0, 1);
        glVertex3f(1, -1, -1);
        glColor3f(0, 0, 0);
        glVertex3f(-1, -1, -1);
        glColor3f(0, 1, 0);
        glVertex3f(-1, 1, -1);
        // face v6-v5-v4
        glColor3f(0, 1, 0);
        glVertex3f(-1, 1, -1);
        glColor3f(0, 1, 1);
        glVertex3f(1, 1, -1);
        glColor3f(0, 0, 1);
        glVertex3f(1, -1, -1);
    glEnd();

    glPopMatrix();
}

///////////////////////////////////////////////////////////////////////////////
// draw cube at upper-right corner with glDrawArrays
// A cube has only 8 vertices, but each vertex is shared for 3 different faces,
// which have different normals. Therefore, we need more than 8 vertex data to
// draw a cube. Since each face has 2 triangles, we need 6 vertices per face.
// (2 * 3 = 6) And, a cube has 6 faces, so, the total number of vertices for
// drawing a cube is 36 (= 6 faces * 6 vertices).
// Note that there are some duplicated vertex data for glDrawArray() because
// the vertex data in the vertex array must be sequentially placed in memory.
// For a cube, there are 24 unique vertex data and 12 redundant vertex data in
// the vertex array.
///////////////////////////////////////////////////////////////////////////////
void Renderer3D::draw2()
{
    // enble and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals1);
    glColorPointer(3, GL_FLOAT, 0, colors1);
    glVertexPointer(3, GL_FLOAT, 0, vertices1);

    glPushMatrix();
    glTranslatef(2, 2, 0);                  // move to upper-right corner

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

///////////////////////////////////////////////////////////////////////////////
// draw cube at bottom-left corner with glDrawElements
// The main advantage of glDrawElements() over glDrawArray() is that
// glDrawElements() allows hopping around the vertex array with the associated
// index values.
// In a cube, the number of vertex data in the vertex array can be reduced to
// 24 vertices for glDrawElements().
// Note that you need an additional array (index array) to store how to traverse
// the vertext data. For a cube, we need 36 entries in the index array.
///////////////////////////////////////////////////////////////////////////////
void Renderer3D::draw3()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals2);
    glColorPointer(3, GL_FLOAT, 0, colors2);
    glVertexPointer(3, GL_FLOAT, 0, vertices2);

    glPushMatrix();
    glTranslatef(-2, -2, 0);                // move to bottom-left corner

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

///////////////////////////////////////////////////////////////////////////////
// draw cube at bottom-right corner with glDrawRangeElements()
// glDrawRangeElements() has two more parameters than glDrawElements() needs;
// start and end index. These values specifies a range of vertex data to be
// loaded into OpenGL. "start" param specifies where the range starts from, and
// "end" param specifies where the range ends. All the index values to be drawn
// must be between "start" and "end".
// Note that not all vertices in the range [start, end] will be referenced.
// But, if you specify a sparsely used range, it causes unnecessary process for
// many unused vertices in that range.
///////////////////////////////////////////////////////////////////////////////
void Renderer3D::draw4()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 0, normals2);
    glColorPointer(3, GL_FLOAT, 0, colors2);
    glVertexPointer(3, GL_FLOAT, 0, vertices2);

    glPushMatrix();
    glTranslatef(2, -2, 0);                 // move to bottom-right

    // draw first half (18 elements){0,1,2, 2,3,0, 4,5,6, 6,7,4, 8,9,10, 10,11,8}
    // The minimum index value in this range is 0, and the maximum index is 11,
    // therefore, "start" param is 0 and "end" param is 11.
    // Then, OpenGL will prefetch only 12 vertex data from the array prior to
    // rendering. (half of total data)
    glDrawRangeElements(GL_TRIANGLES, 0, 11, 18, GL_UNSIGNED_BYTE, indices);

    // draw last half (18 elements) {12,13,14, 14,15,12, 16,17,18, 18,19,16, 20,21,22, 22,23,20}
    // The minimum index value in this range is 12, and the maximum index is 23,
    // therefore, "start" param is 12 and "end" param is 23.
    // Then, OpenGL will prefetch only 12 vertex data from the array prior to
    // rendering.
    // Note that the last param of glDrawRangeElements(). It is the pointer to
    // the location of the first index value to be drawn.
    glDrawRangeElements(GL_TRIANGLES, 12, 23, 18, GL_UNSIGNED_BYTE, indices + 18);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

///////////////////////////////////////////////////////////////////////////////
// draw cube at bottom-left corner with glDrawElements and interleave array
// All the vertex data (position, normal, colour) can be placed together into a 
// single array, and be interleaved like (VNCVNC...). The interleave vertex data
// provides better memory locality.
// Since we are using a single interleaved vertex array to store vertex
// positions, normals and colours, we need to specify "stride" and "pointer"
// parameters properly for glVertexPointer, glNormalPointer and glColorPointer.
// Each vertex has 9 elements of floats (3 position + 3 normal + 3 color), so,
// the stride param should be 36 (= 9 * 4 bytes).
///////////////////////////////////////////////////////////////////////////////
void Renderer3D::draw5()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), vertices3 + 3);
    glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), vertices3 + 6);
    glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), vertices3);

    glPushMatrix();
    glTranslatef(-2, -2, 0);                // move to bottom-left

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}
