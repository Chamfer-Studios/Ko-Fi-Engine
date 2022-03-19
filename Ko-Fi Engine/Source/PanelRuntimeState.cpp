#include "PanelRuntimeState.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "Input.h"

PanelRuntimeState::PanelRuntimeState(Editor* editor, KoFiEngine* engine)
{
    panelName = "RuntimeState";
	this->editor = editor;
    this->engine = engine;
}

PanelRuntimeState::~PanelRuntimeState()
{
}

bool PanelRuntimeState::Awake()
{
	return true;
}

bool PanelRuntimeState::PreUpdate()
{
	return true;
}

bool PanelRuntimeState::Update()
{
	DrawRuntimePanel();

	return true;
}

bool PanelRuntimeState::PostUpdate()
{
	return true;
}

void PanelRuntimeState::DrawRuntimePanel()
{
    if (ImGui::Begin("Game state"/*, ImVec2(405, 38), true, ImGuiWindowFlags_NoMove*/))
    {
        SceneManager* sceneManager = engine->GetSceneManager();
        Editor* editor = engine->GetEditor();
        Input* input = engine->GetInput();
        RuntimeState state = sceneManager->GetState();

        if (state == RuntimeState::PLAYING)
        {
            if (ImGui::Button("PAUSE", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
                sceneManager->OnPause();
        }
        else if (state == RuntimeState::PAUSED)
        {
            if (ImGui::Button("CONTINUE", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
                sceneManager->OnResume();
        }

        ImGui::SameLine();

        if (state != RuntimeState::PLAYING && state != RuntimeState::PAUSED)
        {
            if (ImGui::Button("PLAY", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
            {
                sceneManager->OnPlay();
                editor->OnPlay();
            }
        }
        else
        {
            if (ImGui::Button("STOP", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
                sceneManager->OnStop();
        }

        // ADD THESE FEATURES IN THE FUTURE! (And more...)
        /*ImGui::SameLine(150.f);
        ImGui::Checkbox("Grid", &draw_plane);
        ImGui::SameLine();
        ImGui::Checkbox("Axis", &draw_axis);
        ImGui::SameLine();
        ImGui::Checkbox("Dbg Draw", &debug_draw);

        ImGui::SameLine();
        ImGui::ColorEdit3("Bg", (float*)&camera->background, ImGuiColorEditFlags_NoInputs);*/

    }
    ImGui::End();
}