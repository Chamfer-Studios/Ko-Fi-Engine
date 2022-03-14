#include "ComponentImage.h"

#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "Camera3D.h"
#include "Engine.h"
#include "Editor.h"
#include "Window.h"

#include "PanelChooser.h"
#include "SceneIntro.h"

#include "par_shapes.h"
#include "Log.h"
#include "UI.h"
#include "Material.h"

#include "MathGeoLib/Math/Quat.h"
#include "ImGuiAppLog.h"
#include "SDL_image.h"

#include "SDL.h"

#include "glew.h"
#include <vector>

ComponentImage::ComponentImage(GameObject* parent) : Component(parent)
{
	type = ComponentType::IMAGE;
	glGenFramebuffers(1, &fboId);
}

ComponentImage::~ComponentImage()
{

}

void ComponentImage::Save(Json& json) const
{
	json["type"] = "image";

	json["texture"] = openGLTexture.path;
	json["mask"] = {
		mask.x,
		mask.y
	};
}

void ComponentImage::Load(Json& json)
{
	std::string path = json["texture"].get<std::string>();
	SetTexture(path.c_str());

	std::vector<float> values = json["mask"].get<std::vector<float>>();
	mask.x = values[0];
	mask.y = values[1];
}

bool ComponentImage::Update(float dt)
{
	return true;
}

bool ComponentImage::PostUpdate(float dt)
{
	/*Frustum frustum;
	frustum.pos = owner->GetEngine()->GetCamera3D()->cameraFrustum.pos;
	frustum.front = owner->GetEngine()->GetCamera3D()->cameraFrustum.front;
	frustum.up = owner->GetEngine()->GetCamera3D()->cameraFrustum.up;
	frustum.type = FrustumType::OrthographicFrustum;

	frustum.orthographicWidth = owner->GetEngine()->GetEditor()->lastViewportSize.x;
	frustum.orthographicHeight = owner->GetEngine()->GetEditor()->lastViewportSize.y;
	frustum.nearPlaneDistance = -1.0f;
	frustum.farPlaneDistance = 1000.0f;*/

	/*float3 right = owner->GetEngine()->GetCamera3D()->right;
	float3 up = owner->GetEngine()->GetCamera3D()->up;
	float3 front = owner->GetEngine()->GetCamera3D()->front;
	float3 position = owner->GetEngine()->GetCamera3D()->position;

	owner->GetEngine()->GetCamera3D()->position = { 0, 0, 0 };
	owner->GetEngine()->GetCamera3D()->LookAt({ 0, 0, 1 });

	owner->GetEngine()->GetCamera3D()->projectionIsDirty = true;
	owner->GetEngine()->GetCamera3D()->CalculateViewMatrix();*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glDisable(GL_LIGHTING);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, 1000);

	owner->GetEngine()->GetUI()->drawablePlane->owner = this->owner;
	owner->GetEngine()->GetUI()->drawablePlane->DrawPlane2D(&openGLTexture);
	owner->GetEngine()->GetUI()->drawablePlane->owner = nullptr;

	/*owner->GetEngine()->GetCamera3D()->right = right;
	owner->GetEngine()->GetCamera3D()->up = up;
	owner->GetEngine()->GetCamera3D()->front = front;
	owner->GetEngine()->GetCamera3D()->position = position;

	owner->GetEngine()->GetCamera3D()->projectionIsDirty = true;
	owner->GetEngine()->GetCamera3D()->CalculateViewMatrix();*/

	//glMatrixMode(GL_PROJECTION);
	//glLoadMatrixf(owner->GetEngine()->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	//glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(owner->GetEngine()->GetCamera3D()->viewMatrix.Transposed().ptr());

	glEnable(GL_LIGHTING);

	glDisable(GL_ALPHA_TEST);
	glColor3f(255, 255, 255);

	return true;
}

bool ComponentImage::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Image")) {
		// Texture display
		ImGui::Text("Texture: ");
		ImGui::SameLine();
		if (openGLTexture.GetTextureId() == 0) // Supposedly there is no textureId = 0 in textures array
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(openGLTexture.GetTexturePath());
			ImGui::Image((ImTextureID)openGLTexture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("AddTextureImage")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				SetTexture(path.c_str());
			}
		}

		if (ImGui::Button("Set Texture")) {
			panelChooser->OpenPanel("AddTextureImage", "png");
		}

		ImGui::DragFloat2("Mask", &mask[0], 0.005f, 0.0f, 1.0f);
	}

	return true;
}

void ComponentImage::SetTexture(const char* path)
{
	FreeTextures();
	Importer::GetInstance()->textureImporter->Import(path, &openGLTexture);
}

void ComponentImage::FreeTextures()
{
	if (openGLTexture.GetTextureId() != 0) {
		GLuint id = openGLTexture.GetTextureId();
		glDeleteTextures(1, &id);
	}
}