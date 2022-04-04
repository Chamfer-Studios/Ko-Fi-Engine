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

ComponentImage::ComponentImage(GameObject* parent) : ComponentRenderedUI(parent)
{
	type = ComponentType::IMAGE;
}

ComponentImage::~ComponentImage()
{
	FreeTextures();
}

bool ComponentImage::CleanUp()
{
	FreeTextures();
	return true;
}

void ComponentImage::Save(Json& json) const
{
	json["type"] = "image";

	json["texture"] = openGLTexture.path;
}

void ComponentImage::Load(Json& json)
{
	std::string path = json["texture"].get<std::string>();
	SetTexture(path.c_str());
}

bool ComponentImage::Update(float dt)
{
	return true;
}

bool ComponentImage::PostUpdate(float dt)
{
	return true;
}

bool ComponentImage::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_AllowItemOverlap)) {
		DrawDeleteButton(owner, this);

		// Texture display
		ImGui::Text("Texture: ");
		ImGui::SameLine();
		if (openGLTexture.GetTextureId() == TEXTUREID_DEFAULT) // Supposedly there is no textureId = 0 in textures array
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
			panelChooser->OpenPanel("AddTextureImage", "png", { "png","jpg","jpeg" });
		}
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}

void ComponentImage::SetTexture(const char* path)
{
	FreeTextures();
	Importer::GetInstance()->textureImporter->Import(path, &openGLTexture);
}

void ComponentImage::Draw()
{
	owner->GetEngine()->GetUI()->PrepareUIRender();
	owner->GetComponent<ComponentTransform2D>()->drawablePlane->DrawPlane2D(openGLTexture.GetTextureId(), { 255, 255, 255 });
	owner->GetEngine()->GetUI()->EndUIRender();
}

void ComponentImage::FreeTextures()
{
	if (openGLTexture.GetTextureId() != TEXTUREID_DEFAULT) {
		GLuint id = openGLTexture.GetTextureId();
		glDeleteTextures(1, &id);
	}
}