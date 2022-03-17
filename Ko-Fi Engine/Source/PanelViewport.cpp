// Modules
#include "PanelViewport.h"
#include "Editor.h"
#include "Engine.h"
#include "Camera3D.h"
#include "SceneManager.h"
#include "ViewportFrameBuffer.h"
#include "Input.h"
#include "Window.h"
#include "Importer.h"
#include "Texture.h"
#include "ComponentMaterial.h"

#include "Log.h"
// Tools
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_internal.h"

PanelViewport::PanelViewport(Editor* editor, KoFiEngine* engine)
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

bool PanelViewport::PreUpdate()
{
	return true;
}

bool PanelViewport::Update()
{
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	if (ImGui::Begin("Scene", &editor->panelsState.showViewportWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove ))
	{
		editor->scenePanelOrigin = ImGui::GetWindowPos();
		editor->scenePanelOrigin.x += ImGui::GetWindowContentRegionMin().x;
		editor->scenePanelOrigin.y += ImGui::GetWindowContentRegionMin().y;

		int winX, winY;
		engine->GetWindow()->GetPosition(winX, winY);

		editor->mouseScenePosition.x = engine->GetInput()->GetMouseX() - editor->scenePanelOrigin.x;
		editor->mouseScenePosition.y = engine->GetInput()->GetMouseY() - editor->scenePanelOrigin.y;


		ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;

		if (viewportSize.x != editor->lastViewportSize.x || viewportSize.y != editor->lastViewportSize.y)
		{
			editor->lastViewportSize = viewportSize;
			engine->GetCamera3D()->camera->aspectRatio = viewportSize.x / viewportSize.y;
			engine->GetCamera3D()->camera->RecalculateProjection();
			engine->GetViewportFrameBuffer()->OnResize(viewportSize.x, viewportSize.y);

		}
		editor->viewportSize = viewportSize;

		ImGui::Image((ImTextureID)engine->GetViewportFrameBuffer()->textureBuffer, viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

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

					if (path.find(".fbx") != std::string::npos)
					{
						Importer::GetInstance()->sceneImporter->Import(path.c_str());
					}
					else if (path.find(".jpg") != std::string::npos || path.find(".png") != std::string::npos )
					{
						// Apply texture
						if (engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID != -1)
						{
							GameObject* go = engine->GetSceneManager()->GetCurrentScene()->GetGameObject(engine->GetEditor()->panelGameObjectInfo.selectedGameObjectID);

							if (go->GetComponent<ComponentMaterial>())
							{
								Texture texture = Texture();
								Importer::GetInstance()->textureImporter->Import(path.c_str(), &texture);

								go->GetComponent<ComponentMaterial>()->texture = texture;
								//cMaterial->textures.push_back(texture);
							}
						}
					}
					else if (path.find(".json") != std::string::npos) {
						
						Importer::GetInstance()->sceneImporter->Load(engine->GetSceneManager()->GetCurrentScene(), Importer::GetInstance()->GetNameFromPath(path).c_str());
					}
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::End();

	//ImGui::PopStyleVar();

	return true;
}

bool PanelViewport::PostUpdate()
{
	return true;
}

bool PanelViewport::IsWindowFocused()
{
	return isFocused;
}