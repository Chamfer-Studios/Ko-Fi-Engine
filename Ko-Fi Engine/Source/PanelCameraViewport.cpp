// Modules
#include "PanelCameraViewport.h"
#include "Editor.h"
#include "Engine.h"
#include "Camera3D.h"
#include "SceneManager.h"
#include "Input.h"
#include "Window.h"
#include "Importer.h"
#include "Renderer3D.h"
#include "Texture.h"
#include "ComponentMaterial.h"

#include "Log.h"
// Tools
#include <imgui.h>
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_internal.h"
PanelCameraViewport::PanelCameraViewport(Editor* editor, KoFiEngine* engine)
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

bool PanelCameraViewport::PreUpdate()
{
	return true;
}

bool PanelCameraViewport::Update()
{
	if (editor->panelsState.showCameraViewportWindow) RenderPanel(&editor->panelsState.showCameraViewportWindow);

	return true;
}

bool PanelCameraViewport::PostUpdate()
{
	return true;
}

bool PanelCameraViewport::RenderPanel(bool* showPanel )
{
	if (ImGui::Begin("Camera Preview", showPanel, ImGuiWindowFlags_NoScrollbar))
	{
		ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;

		ImGui::Image((ImTextureID)engine->GetRenderer()->GetTextureBuffer(), viewportSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

	}
	ImGui::End();
	return true;
}
