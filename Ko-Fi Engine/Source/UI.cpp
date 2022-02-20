#include "UI.h"
#include "glew.h"
#include "MathGeoLib/Math/float2.h"

#include "Engine.h"
#include "GameObject.h"
#include "Editor.h"
#include "ImGuiAppLog.h"

#include "ComponentMesh.h"

#include <queue>

UI::UI(KoFiEngine* engine) : Module()
{
	name = "UI";
	this->engine = engine;
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

bool UI::Update(float dt)
{
	return true;

	/*float3 right = App->camera->right;
	float3 up = App->camera->up;
	float3 front = App->camera->front;
	float3 position = App->camera->position;

	App->camera->position = { 0, 0, 2 };
	App->camera->LookAt({ 0, 0, 0 });

	App->camera->projectionIsDirty = true;
	App->camera->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->viewMatrix.Transposed().ptr());

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, 1000);

	uiCameraViewport[0] = viewport[0];
	uiCameraViewport[1] = viewport[1];
	uiCameraViewport[2] = viewport[2];
	uiCameraViewport[3] = viewport[3];

	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::queue<GameObject*> S;
	for (GameObject* child : App->scene->root->children)
	{
		if (child->active)
			S.push(child);
	}

	while (!S.empty())
	{
		GameObject* go = S.front();
		if (go->GetComponent<ComponentTransform2D>() != nullptr) {
			go->Update(dt);
		}
		S.pop();
		for (GameObject* child : go->children)
		{
			if (child->active)
				S.push(child);
		}
	}

	glPopAttrib();

	glPopMatrix();
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	App->camera->right = right;
	App->camera->up = up;
	App->camera->front = front;
	App->camera->position = position;

	App->camera->projectionIsDirty = true;
	App->camera->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->viewMatrix.Transposed().ptr());

	return UPDATE_CONTINUE;*/
}

bool UI::PostUpdate(float dt)
{
	return true;
}

bool UI::CleanUp()
{
	return true;
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