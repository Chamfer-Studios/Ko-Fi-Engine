//Modules
#include "PanelViewport.h"
#include "Editor.h"
#include "Engine.h"
#include "Camera3D.h"
#include "ViewportFrameBuffer.h"

//Tools
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
    ImGui::Begin("Scene", &editor->panelsState.showViewportWindow, ImGuiWindowFlags_NoScrollbar);

    ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
    if (viewportSize.x != editor->lastViewportSize.x || viewportSize.y != editor->lastViewportSize.y)
    {
        engine->GetCamera3D()->aspectRatio = viewportSize.x / viewportSize.y;
        engine->GetCamera3D()->RecalculateProjection();
    }
    editor->lastViewportSize = viewportSize;
    ImGui::Image((ImTextureID)engine->GetViewportFrameBuffer()->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

	return true;
}

bool PanelViewport::PostUpdate()
{
	return true;
}