#include "PanelSettings.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "Window.h"
#include "Renderer3D.h"
#include "SceneManager.h"
#include "Physics.h"


PanelSettings::PanelSettings(Editor* editor)
{
    panelName = "Settings";
    this->editor = editor;
}

PanelSettings::~PanelSettings()
{
}

bool PanelSettings::Awake()
{
	return true;
}

bool PanelSettings::PreUpdate()
{
	return true;
}

bool PanelSettings::Update()
{
    if (editor->toggleSettingsPanel)
    {
        ShowPanel(&editor->toggleSettingsPanel);
    }
	return true;
}

bool PanelSettings::PostUpdate()
{
	return true;
}

bool PanelSettings::ShowPanel(bool* toggleSettingsPanel)
{
    bool ret = true;

    if(ImGui::Begin("Engine Settings", toggleSettingsPanel))
    {
        if (ImGui::Button("Save Config##"))
            editor->engine->SaveConfiguration();

        std::list<Module*>::iterator item = editor->engine->AllModules().begin();;
        while (item != editor->engine->AllModules().end() && ret)
        {
            ret = (*item)->InspectorDraw();
            item++;
        }
    }
    ImGui::End();

    return ret;
}
