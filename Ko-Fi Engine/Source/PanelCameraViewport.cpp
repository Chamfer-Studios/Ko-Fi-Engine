// Modules
#include "PanelCameraViewport.h"
#include "M_Editor.h"
#include "Engine.h"
#include "M_Camera3D.h"
#include "M_SceneManager.h"
#include "M_Input.h"
#include "M_Window.h"
#include "Importer.h"
#include "M_Renderer3D.h"
#include "R_Texture.h"
#include "C_Material.h"
#include "C_Camera.h"

#include "Log.h"
// Tools
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_internal.h"
PanelCameraViewport::PanelCameraViewport(M_Editor* editor, KoFiEngine* engine)
{
	this->editor = editor;
	this->engine = engine;
	panelName = "CameraViewport";
}

PanelCameraViewport::~PanelCameraViewport()
{
}

bool PanelCameraViewport::Awake()
{
	return true;
}

bool PanelCameraViewport::Update()
{
	OPTICK_EVENT();
	if (editor->panelsState.showCameraViewportWindow) RenderPanel(&editor->panelsState.showCameraViewportWindow);

	return true;
}

bool PanelCameraViewport::RenderPanel(bool* showPanel )
{
	

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	float width = editor->engine->GetCamera3D()->gameCamera->GetNearPlaneWidth();
	float height = editor->engine->GetCamera3D()->gameCamera->GetNearPlaneHeight();
	ImGui::SetNextWindowSize(ImVec2(width*250, height*250));
	if (ImGui::Begin("Camera Preview", showPanel, ImGuiWindowFlags_NoScrollbar ))
	{
		ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
		
		if (viewportSize.x != editor->lastCameraViewportSize.x || viewportSize.y != editor->lastCameraViewportSize.y)
		{
			editor->lastCameraViewportSize = viewportSize;
			engine->GetCamera3D()->gameCamera->SetAspectRatio(viewportSize.x / viewportSize.y);

			engine->GetRenderer()->ResizePreviewFrameBuffers(viewportSize.x, viewportSize.y);


		}
		editor->cameraViewportSize = viewportSize;
		ImGui::Image((ImTextureID)engine->GetRenderer()->GetPreviewTextureBuffer(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	}
	ImGui::End();
	ImGui::PopStyleVar();
	return true;
}
