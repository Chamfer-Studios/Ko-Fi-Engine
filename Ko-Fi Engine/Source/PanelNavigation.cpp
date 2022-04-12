#include "PanelNavigation.h"
#include "M_Editor.h"
#include "Engine.h"
#include "M_Navigation.h"
#include <imgui.h>

PanelNavigation::PanelNavigation(M_Editor* editor)
{
	this->editor = editor;
	panelName = "M_Navigation";
}

PanelNavigation::~PanelNavigation()
{
}

bool PanelNavigation::Awake()
{
	return true;
}

bool PanelNavigation::Update()
{
	OPTICK_EVENT();

	editor->engine->GetNavigation()->OnGui();

	return true;
}
