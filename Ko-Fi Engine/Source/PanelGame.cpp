#include "PanelGame.h"
#include <imgui.h>
#include "M_Editor.h"

PanelGame::PanelGame(M_Editor* editor)
{
	this->editor = editor;
	panelName = "Game";
}

PanelGame::~PanelGame()
{
}

bool PanelGame::Awake()
{
	return true;
}

bool PanelGame::PreUpdate()
{
	return true;
}

bool PanelGame::Update()
{
	ImGui::Begin("Game", &editor->panelsState.showGameWindow, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
	ImGui::End();

	return true;
}

bool PanelGame::PostUpdate()
{
	return true;
}