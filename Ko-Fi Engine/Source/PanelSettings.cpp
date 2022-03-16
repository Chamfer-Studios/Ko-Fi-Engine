#include "PanelSettings.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "Window.h"


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

void PanelSettings::ShowPanel(bool* toggleSettingsPanel)
{
    if(ImGui::Begin("Engine Settings", toggleSettingsPanel))
    {
        if(ImGui::CollapsingHeader("Window##"))
        {
            ImGui::Combo("##resolutionCombo", &currentResolution, "Select Resolution\0R1024x768\0R1920x1080\0R1280x720");
                
            ImGui::SameLine();
            if(ImGui::Button("SetResolution##"))
            {
                switch(currentResolution)
                {
                    case 0:
                    break;
                    case 1: 
                        // Set 1024x768 resolution
                        editor->engine->GetWindow()->SetWidth(1024);
                        editor->engine->GetWindow()->SetHeight(768);
                    break;
                    case 2:
                        // Set 1920x1080 resolution
                        editor->engine->GetWindow()->SetWidth(1920);
                        editor->engine->GetWindow()->SetHeight(1080);
                    break;
                    case 3:
                        // Set 1280x720 resolution
                        editor->engine->GetWindow()->SetWidth(1280);
                        editor->engine->GetWindow()->SetHeight(720);
                    break;
                    default:
                    break;
              
                }

                editor->engine->SaveConfiguration();
            }
        }

    }
    ImGui::End();
}
