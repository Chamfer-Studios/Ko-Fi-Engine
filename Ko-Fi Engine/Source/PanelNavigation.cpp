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
	CleanUp();
}

bool PanelNavigation::CleanUp()
{
	editor = nullptr;

	return true;
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
	editor->engine->GetNavigation()->OnGui();

	return true;
}

bool PanelNavigation::PostUpdate()
{
	return true;
}
