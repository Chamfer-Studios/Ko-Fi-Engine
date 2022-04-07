#include "PanelRuntimeState.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "M_Camera3D.h"
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
        M_Camera3D* camera = engine->GetCamera3D();
        Input* input = engine->GetInput();
        GameState state = sceneManager->GetGameState();

        if (state == GameState::PLAYING)
        {
            if (ImGui::Button("PAUSE", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
                sceneManager->OnPause();
        }
        else if (state == GameState::PAUSED)
        {
            if (ImGui::Button("CONTINUE", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
                sceneManager->OnResume();
        }

        ImGui::SameLine();

        if (state != GameState::PLAYING && state != GameState::PAUSED)
        {
            if (ImGui::Button("PLAY", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
            {
                sceneManager->OnPlay();
                editor->OnPlay();
                camera->OnPlay();
            }
        }
        else
        {
            if (ImGui::Button("STOP", ImVec2(120, 22)) || input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
            {
                sceneManager->OnStop();
                camera->OnStop();
            }
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