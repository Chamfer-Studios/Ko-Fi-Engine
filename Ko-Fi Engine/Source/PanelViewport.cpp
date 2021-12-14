//Modules
#include "PanelViewport.h"
#include "Editor.h"
#include "Engine.h"
#include "Camera3D.h"
#include "ViewportFrameBuffer.h"
#include "Input.h"
#include "Window.h"

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
    ImGui::Begin("Scene", &editor->panelsState.showViewportWindow, /*ImGuiWindowFlags_NoScrollbar*/ ImGuiWindowFlags_NoScrollWithMouse);

    ImVec2 viewportSize = ImGui::GetCurrentWindow()->Size;
    if (viewportSize.x != editor->lastViewportSize.x || viewportSize.y != editor->lastViewportSize.y)
    {
        engine->GetCamera3D()->aspectRatio = viewportSize.x / viewportSize.y;
        engine->GetCamera3D()->RecalculateProjection();
    }
    editor->lastViewportSize = viewportSize;
    ImGui::Image((ImTextureID)engine->GetViewportFrameBuffer()->texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));

    editor->scenePanelOrigin = ImGui::GetWindowPos();
    editor->scenePanelOrigin.x += ImGui::GetWindowContentRegionMin().x;
    editor->scenePanelOrigin.y += ImGui::GetWindowContentRegionMin().y;

    int winX, winY;
    engine->GetWindow()->GetPosition(winX, winY);
    editor->scenePanelOrigin.x -= winX;
    editor->scenePanelOrigin.y -= winY;

    editor->mouseScenePosition.x = engine->GetInput()->GetMouseX() - editor->scenePanelOrigin.x;
    editor->mouseScenePosition.y = engine->GetInput()->GetMouseY() - editor->scenePanelOrigin.y;

    ImGui::End();

	return true;
}

bool PanelViewport::PostUpdate()
{
    

	return true;
}