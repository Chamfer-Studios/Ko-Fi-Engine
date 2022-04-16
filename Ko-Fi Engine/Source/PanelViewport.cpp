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

bool PanelViewport::Awake()
{
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

		if (ImGui::IsMouseClicked(1)) ImGui::SetWindowFocus();
		isFocused = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_ITEM");
			if (payload != nullptr)
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
				{
					std::string path = (const char*)payload->Data;

					if (path.find(".fbx") != std::string::npos || path.find(".md5mesh") != std::string::npos)
					{
						Importer::GetInstance()->sceneImporter->Import(path.c_str());
					}
					else if (path.find(".jpg") != std::string::npos || path.find(".png") != std::string::npos )
					{
						// Apply texture
						if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
						{
							GameObject* go = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);

							if (go->GetComponent<C_Material>())
							{
								R_Texture *texture = new R_Texture();
								Importer::GetInstance()->textureImporter->Import(path.c_str(), texture);

								go->GetComponent<C_Material>()->texture = texture;
								//cMaterial->textures.push_back(texture);
							}
						}
					}
					else if (path.find(".json") != std::string::npos) {
						
						Importer::GetInstance()->sceneImporter->Load(engine->GetSceneManager()->GetCurrentScene(), engine->GetFileSystem()->GetNameFromPath(path).c_str());
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
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