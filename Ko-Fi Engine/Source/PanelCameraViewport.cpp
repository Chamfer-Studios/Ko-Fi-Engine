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
	if (ImGui::Begin("Camera Preview", showPanel, ImGuiWindowFlags_NoScrollbar))
	{
		ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
		viewportSize.y -= 26; // Make the viewport substract 26 pixels from the imgui window (corresponds to the imgui viewport header)

		if (viewportSize.x != editor->lastCameraViewportSize.x || viewportSize.y != editor->lastCameraViewportSize.y)
		{
			editor->lastCameraViewportSize = viewportSize;
			
			engine->GetRenderer()->ResizePreviewFrameBuffers(viewportSize.x, viewportSize.y);
			engine->GetRenderer()->ResizeFrameBuffers(editor->cameraViewportSize.x, editor->cameraViewportSize.y);
	

		}
		editor->cameraViewportSize = viewportSize;
		ImGui::Image((ImTextureID)engine->GetRenderer()->GetPreviewTextureBuffer(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	}
	ImGui::End();
	return true;
}
