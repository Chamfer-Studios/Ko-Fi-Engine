#include "PanelNavigation.h"
#include "Editor.h"
#include "Engine.h"
#include "Navigation.h"
#include <imgui.h>

PanelNavigation::PanelNavigation(Editor* editor)
{
	this->editor = editor;
	panelName = "Navigation";
}

PanelNavigation::~PanelNavigation()
{
}

bool PanelNavigation::Awake()
{
	return true;
}

bool PanelNavigation::PreUpdate()
{
	return true;
}

bool PanelNavigation::Update()
{
	ImGui::Begin("Navigator");

	if (ImGui::Button("Bake Navmesh")) {
		editor->engine->GetNavigation()->ComputeNavmesh();
		editor->engine->GetNavigation()->PrepareDetour();
	}

	ImGui::End();

	return true;
}

bool PanelNavigation::PostUpdate()
{
	return true;
}
