#include "PanelInspector.h"
#include <imgui.h>
#include "Editor.h"
#include "GameObject.h"

PanelInspector::PanelInspector(Editor* editor)
{
	this->editor = editor;
}

PanelInspector::~PanelInspector()
{
}

bool PanelInspector::Awake()
{
	return true;
}

bool PanelInspector::PreUpdate()
{
	return true;
}

bool PanelInspector::Update()
{

	// Panel game object info. to manage the options of the current game object
	PanelGameObjectInfo panelGameObjectInfo = editor->panelGameObjectInfo;
	ImGui::Begin("Inspector");
	if (panelGameObjectInfo.currentGameObjectID != -1)
	{
		// Current game object (the one we have selected at the moment)
		GameObject* currentGameObject = editor->gameObjects.at(editor->panelGameObjectInfo.currentGameObjectID);
		for (Component* component : currentGameObject->GetComponents())
		{
			component->InspectorDraw();
		}
	}

	ImGui::End();
	return true;
}

bool PanelInspector::PostUpdate()
{
	return true;
}
