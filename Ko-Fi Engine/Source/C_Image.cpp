#include "C_Image.h"

#include "C_Mesh.h"
#include "GameObject.h"
#include "C_Transform2D.h"
#include "C_Material.h"
#include "C_Transform.h"
#include "M_Camera3D.h"
#include "Engine.h"
#include "M_Editor.h"
#include "M_Window.h"

#include "PanelChooser.h"
#include "SceneIntro.h"

#include "Log.h"
#include "M_UI.h"
#include "R_Material.h"

#include "MathGeoLib/Math/Quat.h"
#include "ImGuiAppLog.h"
#include "SDL_image.h"

#include "SDL.h"

#include "glew.h"
#include <vector>

C_Image::C_Image(GameObject* parent) : C_RenderedUI(parent)
{
	type = ComponentType::IMAGE;
}

C_Image::~C_Image()
{
	FreeTextures();
}

bool C_Image::CleanUp()
{
	FreeTextures();
	return true;
}

void C_Image::Save(Json& json) const
{
	json["type"] = "image";

	json["texture"] = openGLTexture.path;
}

void C_Image::Load(Json& json)
{
	std::string path = json["texture"].get<std::string>();
	SetTexture(path.c_str());
}

bool C_Image::Update(float dt)
{
	return true;
}

bool C_Image::PostUpdate(float dt)
{
	return true;
}

bool C_Image::InspectorDraw(PanelChooser* panelChooser)
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

void C_Image::SetTexture(const char* path)
{
	FreeTextures();
	Importer::GetInstance()->textureImporter->Import(path, &openGLTexture);
}

void C_Image::Draw()
{
	owner->GetEngine()->GetUI()->PrepareUIRender();
	owner->GetComponent<C_Transform2D>()->drawablePlane->DrawPlane2D(openGLTexture.GetTextureId(), { 255, 255, 255 });
	owner->GetEngine()->GetUI()->EndUIRender();
}

void C_Image::FreeTextures()
{
	if (openGLTexture.GetTextureId() != TEXTUREID_DEFAULT) {
		GLuint id = openGLTexture.GetTextureId();
		glDeleteTextures(1, &id);
	}
}