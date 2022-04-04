#include "PanelGame.h"
#include <imgui.h>
#include "Editor.h"

PanelGame::PanelGame(Editor* editor)
{
	this->editor = editor;
	panelName = "Game";
}

PanelGame::~PanelGame()
{
	CleanUp();
}

bool PanelGame::CleanUp()
{
	editor = nullptr;
	return true;
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