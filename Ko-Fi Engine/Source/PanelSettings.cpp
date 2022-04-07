#include "PanelSettings.h"
#include <imgui.h>
#include "M_Editor.h"
#include "Engine.h"
#include "M_Window.h"
#include "M_Renderer3D.h"
#include "M_SceneManager.h"
#include "M_Physics.h"


PanelSettings::PanelSettings(M_Editor* editor)
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
