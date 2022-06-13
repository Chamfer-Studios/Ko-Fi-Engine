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

bool PanelSettings::Update()
{
    OPTICK_EVENT();

    if (editor->toggleSettingsPanel)
    {
        ShowPanel(&editor->toggleSettingsPanel);
    }
	return true;
}

bool PanelSettings::ShowPanel(bool* toggleSettingsPanel)
{
    bool ret = true;

    if(ImGui::Begin("Engine Settings", toggleSettingsPanel))
    {
        if (ImGui::Button("Save Config##"))
            editor->engine->SaveConfiguration();

        auto item = editor->engine->AllModules().begin();;
        while (item != editor->engine->AllModules().end() && ret)
        {
            ret = (*item)->InspectorDraw();
            item++;
        }
    }
    ImGui::End();

    return ret;
}
