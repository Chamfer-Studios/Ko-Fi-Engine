#include "UI.h"
#include "glew.h"
#include "MathGeoLib/Math/float2.h"

#include "Engine.h"
#include "GameObject.h"
#include "Editor.h"
#include "ImGuiAppLog.h"
#include "Camera3D.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Window.h"
#include "FileSystem.h"
#include "Texture.h"

#include <string>

#include "ComponentMesh.h"
#include "ComponentTransform2D.h"
#include "ComponentCamera.h"

#include "SDL.h"

#include <queue>

Shadert::Shadert()
{
	const char* vShaderCode = R"(
		#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
out vec2 TexCoord;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
TexCoord = texCoord;
}
	)";

	const char* fShaderCode = R"(
		#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
      color = texture(ourTexture, TexCoord)*ourColor;
}
	)";

	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");

	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);

	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shadert::Begin()
{
	glUseProgram(ID);
}

void Shadert::End()
{
	glUseProgram(0);
}

void Shadert::CheckCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);

			appLog->AddLog("Shader error -> Type: %s\nLog: %s\n", type, infoLog);
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);

			appLog->AddLog("Shader error -> Type: %s\nLog: %s\n", type, infoLog);
		}
	}
}

MyPlane::MyPlane() {
	vertices.push_back({ -0.5, -0.5, 0 });
	vertices.push_back({ -0.5, 0.5, 0 });
	vertices.push_back({ 0.5, -0.5, 0 });
	vertices.push_back({ 0.5, 0.5, 0 });

	texCoords.push_back({ 0, 0 });
	texCoords.push_back({ 0, 1 });
	texCoords.push_back({ 1, 0 });
	texCoords.push_back({ 1, 1 });

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &textureBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);
}

MyPlane::~MyPlane()
{
}

void MyPlane::DrawPlane2D(Texture* texture)
{
	if (texture->width != -1 && texture->height != -1)
		DrawPlane2D(texture->GetTextureId());
	else
		DrawPlane2D((unsigned int)0);
}

void MyPlane::DrawPlane2D(unsigned int texture) {
	if (texture == 0) return;
	ComponentTransform2D* cTransform = owner->GetComponent<ComponentTransform2D>();

	float2 normalizedPosition = cTransform->GetNormalizedPosition();
	float2 normalizedSize = cTransform->GetNormalizedSize();

	float3 position3d = { normalizedPosition.x, normalizedPosition.y, 2 };
	float3 rotation3d = cTransform->GetRotation();
	Quat quaternion3d = Quat::FromEulerXYZ(rotation3d.x, rotation3d.y, rotation3d.z);
	float3 size3d = { normalizedSize.x, normalizedSize.y, 1 };

	math::float4x4 transform = float4x4::FromTRS(position3d, quaternion3d, size3d);

	glEnableClientState(GL_VERTEX_ARRAY);

	if (texture != 0) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	if (texture != 0)
		glBindTexture(GL_TEXTURE_2D, texture);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

	glPushMatrix();
	glMultMatrixf(transform.Transposed().ptr());
	glColor3f(1.0f, 1.0f, 1.0f);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
	glPopMatrix();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (texture != 0) {
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
}

UI::UI(KoFiEngine* engine) : Module()
{
	name = "UI";
	this->engine = engine;
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(engine->GetWindow()->window, -1, flags);
	TTF_Init();
	rubik = TTF_OpenFont("Assets/Fonts/Rubik_Mono_One/RubikMonoOne-Regular.ttf", 60);
}

UI::~UI()
{
	delete drawablePlane;
}

bool UI::Start()
{
	drawablePlane = new MyPlane();

	return true;
}

bool UI::PreUpdate(float dt)
{
	return true;
}

bool UI::Update(float dt) {

	return true;
}

bool UI::PostUpdate(float dt)
{

	return true;
}

bool UI::CleanUp()
{
	TTF_CloseFont(rubik);
	return true;
}

void UI::OnNotify(const Event& event)
{
}

void UI::PrepareUIRender(GameObject* owner)
{
	right = engine->GetCamera3D()->right;
	up = engine->GetCamera3D()->up;
	front = engine->GetCamera3D()->front;
	position = engine->GetCamera3D()->position;

	engine->GetCamera3D()->position = { 0, 0, 0 };
	engine->GetCamera3D()->LookAt({ 0, 0, 1 });

	engine->GetCamera3D()->projectionIsDirty = true;
	engine->GetCamera3D()->CalculateViewMatrix(true);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());

	//glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);

	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_LIGHTING);

	drawablePlane->owner = owner;
}

void UI::EndUIRender()
{
	drawablePlane->owner = nullptr;

	glEnable(GL_LIGHTING);

	//glColor3f(255, 255, 255);
	glDisable(GL_BLEND);

	glPopMatrix();

	engine->GetCamera3D()->right = right;
	engine->GetCamera3D()->up = up;
	engine->GetCamera3D()->front = front;
	engine->GetCamera3D()->position = position;

	engine->GetCamera3D()->projectionIsDirty = true;
	engine->GetCamera3D()->CalculateViewMatrix(false);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());
}

float2 UI::GetUINormalizedMousePosition()
{
	float mouseX = engine->GetEditor()->mouseScenePosition.x;
	float mouseY = engine->GetEditor()->mouseScenePosition.y;

	return { mouseX, mouseY };
}

void UI::OnGui()
{
}

/*void ModuleUI::OnLoad(const JSONReader& reader)
{
}

void ModuleUI::OnSave(JSONWriter& writer) const
{
}*/