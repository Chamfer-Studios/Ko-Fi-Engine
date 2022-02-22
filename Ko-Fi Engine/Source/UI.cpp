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

#include "ComponentMesh.h"
#include "ComponentTransform2D.h"

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

bool UI::PostUpdate(float dt)
{
	return true;

	float3 right = engine->GetCamera3D()->right;
	float3 up = engine->GetCamera3D()->up;
	float3 front = engine->GetCamera3D()->front;
	float3 position = engine->GetCamera3D()->position;

	engine->GetCamera3D()->position = { 0, 0, 2 };
	engine->GetCamera3D()->LookAt({ 0, 0, 0 });

	engine->GetCamera3D()->projectionIsDirty = true;
	engine->GetCamera3D()->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());

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
	for (GameObject* child : engine->GetSceneManager()->GetCurrentScene()->rootGo->GetChildren())
	{
		if (child->active)
			S.push(child);
	}

	while (!S.empty())
	{
		GameObject* go = S.front();
		if (go->GetComponent<ComponentTransform2D>() != nullptr) {
			go->PostUpdate(dt);
		}
		S.pop();
		for (GameObject* child : go->GetChildren())
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

	engine->GetCamera3D()->right = right;
	engine->GetCamera3D()->up = up;
	engine->GetCamera3D()->front = front;
	engine->GetCamera3D()->position = position;

	engine->GetCamera3D()->projectionIsDirty = true;
	engine->GetCamera3D()->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(engine->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(engine->GetCamera3D()->viewMatrix.Transposed().ptr());

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