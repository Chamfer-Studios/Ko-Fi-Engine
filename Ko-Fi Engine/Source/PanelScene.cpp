#include "PanelScene.h"
#include <imgui.h>

#include "Editor.h"
#include "GameObject.h"

PanelScene::PanelScene(Editor* editor)
{
	this->editor = editor;
}

PanelScene::~PanelScene()
{
}

bool PanelScene::Awake()
{
	return true;
}

bool PanelScene::PreUpdate()
{
	return true;
}

bool PanelScene::Update()
{
	// Window with a button to create another window
	ImGui::Begin("Scene");
	editor->Markdown("# Game Objects");

	if (!editor->gameObjects.empty())
	{
		for (int i = 0; i < editor->gameObjects.size(); i++)
		{
			if (ImGui::Selectable(editor->gameObjects.at(i).GetName().c_str()))
				editor->panelGameObjectInfo.currentGameObjectID = i;
		}
	}

	ImGui::End();

	return true;
}

bool PanelScene::PostUpdate()
{
	return true;
}
