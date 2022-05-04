// Modules
#include "PanelViewport.h"
//#include "ViewportFrameBuffer.h"
// Modules
#include "Engine.h"
#include "M_Camera3D.h"
#include "M_SceneManager.h"
#include "M_Editor.h"
#include "M_Input.h"
#include "M_Window.h"
#include "M_FileSystem.h"
#include "M_Renderer3D.h"
#include "M_ResourceManager.h"
//Importer
#include "I_Texture.h"
//Resources
#include "R_Texture.h"

// GameObject
#include "GameObject.h"
#include "C_Material.h"
#include "C_Camera.h"

#include "Importer.h"
#include "R_Texture.h"

#include "Log.h"
// Tools
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_internal.h"

#include "optick.h"

PanelViewport::PanelViewport(M_Editor* editor, KoFiEngine* engine)
{
	this->editor = editor;
	this->engine = engine;
	panelName = "Viewport";
}

PanelViewport::~PanelViewport()
{
}

bool PanelViewport::Start()
{
	speedCameraIcon = new R_Texture();
	litIcon = new R_Texture();
	gizmoMoveIcon = new R_Texture();
	gizmoRotateIcon = new R_Texture();
	gizmoScaleIcon = new R_Texture();
	Importer::GetInstance()->textureImporter->Import("Assets/Icons/video.png", speedCameraIcon);
	Importer::GetInstance()->textureImporter->Import("Assets/Icons/lit_mode.png", litIcon);
	Importer::GetInstance()->textureImporter->Import("Assets/Icons/translate.png", gizmoMoveIcon);
	Importer::GetInstance()->textureImporter->Import("Assets/Icons/3d-rotate.png", gizmoRotateIcon);
	Importer::GetInstance()->textureImporter->Import("Assets/Icons/maximize.png", gizmoScaleIcon);
	return true;
}

bool PanelViewport::Update()
{
	OPTICK_EVENT();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Scene", &editor->panelsState.showViewportWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize))
	{
		editor->scenePanelOrigin = ImGui::GetWindowPos();
		editor->scenePanelOrigin.x += ImGui::GetWindowContentRegionMin().x;
		editor->scenePanelOrigin.y += ImGui::GetWindowContentRegionMin().y;

		int winX, winY;
		engine->GetWindow()->GetPosition(winX, winY);

		editor->mouseScenePosition.x = engine->GetInput()->GetMouseX() - editor->scenePanelOrigin.x;
		editor->mouseScenePosition.y = engine->GetInput()->GetMouseY() - editor->scenePanelOrigin.y;


		ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
		viewportSize.y -= 26; // Make the viewport substract 26 pixels from the imgui window (corresponds to the imgui viewport header)

		if (viewportSize.x != editor->lastViewportSize.x || viewportSize.y != editor->lastViewportSize.y)
		{
			editor->lastViewportSize = viewportSize;
			engine->GetCamera3D()->currentCamera->aspectRatio = viewportSize.x / viewportSize.y;
			engine->GetCamera3D()->currentCamera->RecalculateProjection();
			engine->GetRenderer()->ResizeFrameBuffers(viewportSize.x, viewportSize.y);
		}
		editor->viewportSize = viewportSize;
		ImGui::Image((ImTextureID)engine->GetRenderer()->GetTextureBuffer(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_ITEM");
			if (payload != nullptr)
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
				{
					std::string path = (const char*)payload->Data;
					path = engine->GetResourceManager()->GetValidPath(path.c_str());
					Resource* resource = engine->GetResourceManager()->GetResourceFromLibrary(path.c_str());
					if (resource != nullptr)
						engine->GetSceneManager()->LoadResourceToScene(resource);
					else
					{
						if (path.find(".json") != std::string::npos)
							Importer::GetInstance()->sceneImporter->LoadScene(engine->GetSceneManager()->GetCurrentScene(), engine->GetFileSystem()->GetNameFromPath(path).c_str());
						else
							dragDropPopup = true;
					}
					// OLD STUF
					//if (path.find(".fbx") != std::string::npos || path.find(".md5mesh") != std::string::npos)
					//{
					//	Importer::GetInstance()->sceneImporter->Import(path.c_str());
					//}
					//else if (path.find(".jpg") != std::string::npos || path.find(".png") != std::string::npos)
					//{
					//	for (int i = 0; i < engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
					//	{
					//		if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i] != 0)
					//		{
					//			GameObject* go = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjects[i]);

					//			if (go->GetComponent<C_Material>())
					//			{
					//				R_Texture* texture = new R_Texture();
					//				Importer::GetInstance()->textureImporter->Import(path.c_str(), texture);

					//				go->GetComponent<C_Material>()->texture = texture;
					//				//cMaterial->textures.push_back(texture);
					//			}
					//		}
					//	}
					//	// Apply texture
					//	
					//}
				}
			}
			ImGui::EndDragDropTarget();
		}
		DrawViewportBar();
		if (ImGui::IsMouseClicked(1)) ImGui::SetWindowFocus();
		isFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
	}
	ImGui::End();

	ImGui::PopStyleVar();

	return true;
}

bool PanelViewport::IsWindowFocused()
{
	return isFocused;
}

void PanelViewport::SetIsFocused(bool isFocused)
{
	this->isFocused = isFocused;
}

void PanelViewport::DrawViewportBar()
{
	ImGui::SetItemAllowOverlap();
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth()/2 - (5*24),ImGui::GetWindowContentRegionMin().y+8));
	ImGui::BeginGroup();
	//Overlay Items
	if (ImGui::ImageButton((ImTextureID)litIcon->GetTextureId(), ImVec2(24, 24)))
	{
		ImGui::OpenPopup("Lit Popup");
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)gizmoMoveIcon->GetTextureId(), ImVec2(24, 24)))
	{
		editor->engine->GetSceneManager()->SetGizmoOperation(ImGuizmo::OPERATION::TRANSLATE);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)gizmoRotateIcon->GetTextureId(), ImVec2(24, 24)))
	{
		editor->engine->GetSceneManager()->SetGizmoOperation(ImGuizmo::OPERATION::ROTATE);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)gizmoScaleIcon->GetTextureId(), ImVec2(24, 24)))
	{
		editor->engine->GetSceneManager()->SetGizmoOperation(ImGuizmo::OPERATION::SCALE);
	}
	ImGui::SameLine();
	if (ImGui::ImageButton((ImTextureID)speedCameraIcon->GetTextureId(),ImVec2(24,24)))
	{
		ImGui::OpenPopup("Camera Speed Popup");
	}
//Popups
	if (ImGui::BeginPopup("Lit Popup"))
	{
		if (ImGui::Selectable("Wireframe"))
		{

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		}
		if (ImGui::Selectable("Lit"))
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopup("Camera Speed Popup"))
	{
		if (ImGui::SliderInt("##Camera Speed", &editor->engine->GetCamera3D()->engineCamera->speedMultiplier, 1.0f, 5.0f))
		{
			editor->engine->GetCamera3D()->engineCamera->ChangeSpeed(editor->engine->GetCamera3D()->engineCamera->speedMultiplier);
		}
		ImGui::EndPopup();
	}
	if (dragDropPopup)
	{
		ImGui::OpenPopup("Unsupported Extension");
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Unsupported Extension", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("The current file extension is unsupported.\nYour asset is not imported yet, maybe you should try refreshing?\n\n");

			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				dragDropPopup = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::EndPopup();
		}
	}


	
	
	ImGui::EndGroup();
}
