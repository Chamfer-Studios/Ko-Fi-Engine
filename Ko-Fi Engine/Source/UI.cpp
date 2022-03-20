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

MyPlane::MyPlane(GameObject* _owner) {
	vertices.push_back({ 0, 0, 0 });
	vertices.push_back({ 0, 1, 0 });
	vertices.push_back({ 1, 0, 0 });
	vertices.push_back({ 1, 1, 0 });

	texCoords.push_back({ 0, 1 });
	texCoords.push_back({ 0, 0 });
	texCoords.push_back({ 1, 1 });
	texCoords.push_back({ 1, 0 });

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(2);
	indices.push_back(1);

	indices.push_back(2);
	indices.push_back(1);
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(3);

	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &indexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &textureBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);

	owner = _owner;
}

MyPlane::~MyPlane()
{
}

void MyPlane::DrawPlane2D(Texture* texture, SDL_Color color)
{
	if (texture->width != -1 && texture->height != -1)
		DrawPlane2D(texture->GetTextureId(), color);
	else
		DrawPlane2D((unsigned int)0, color);
}

void MyPlane::DrawPlane2D(unsigned int texture, SDL_Color color) {
	if (texture == 0) return;
	ComponentTransform2D* cTransform = owner->GetComponent<ComponentTransform2D>();

	float2 normalizedPosition = cTransform->GetNormalizedPosition();
	float2 normalizedSize = cTransform->GetNormalizedSize();

	float3 position3d = { normalizedPosition.x, normalizedPosition.y, -1};
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
	glColor3f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f);
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
}

bool UI::Start()
{
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

void UI::PrepareUIRender()
{
	right = engine->GetCamera3D()->currentCamera->right;
	up = engine->GetCamera3D()->currentCamera->up;
	front = engine->GetCamera3D()->currentCamera->front;
	position = engine->GetCamera3D()->currentCamera->position;

	float2 offset = { engine->GetEditor()->lastViewportSize.x / 2, engine->GetEditor()->lastViewportSize.y / 2 };
	engine->GetCamera3D()->currentCamera->position = { offset.x, offset.y, 0 };
	engine->GetCamera3D()->currentCamera->LookAt({ offset.x, offset.y, -1 });

	engine->GetCamera3D()->currentCamera->projectionIsDirty = true;
	engine->GetCamera3D()->currentCamera->CalculateViewMatrix(true);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->currentCamera->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->currentCamera->viewMatrix.Transposed().ptr());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_LIGHTING);
}

void UI::EndUIRender()
{
	glEnable(GL_LIGHTING);

	//glColor3f(255, 255, 255);
	glDisable(GL_BLEND);

	glPopMatrix();

	engine->GetCamera3D()->currentCamera->right = right;
	engine->GetCamera3D()->currentCamera->up = up;
	engine->GetCamera3D()->currentCamera->front = front;
	engine->GetCamera3D()->currentCamera->position = position;

	engine->GetCamera3D()->currentCamera->projectionIsDirty = true;
	engine->GetCamera3D()->currentCamera->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->currentCamera->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->currentCamera->viewMatrix.Transposed().ptr());
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