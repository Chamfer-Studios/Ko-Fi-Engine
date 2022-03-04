#include "PanelInspector.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"

PanelInspector::PanelInspector(Editor* editor)
{
	this->editor = editor;
	panelName = "Inspector";
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
	if (panelGameObjectInfo.selectedGameObjectID != -1)
	{
		// Current game object (the one we have selected at the moment)
		GameObject* currentGameObject = editor->engine->GetSceneManager()->GetCurrentScene()->GetGameObject(editor->panelGameObjectInfo.selectedGameObjectID);
		for (Component* component : currentGameObject->GetComponents())
		{
			component->InspectorDraw(editor->GetPanelChooser());
		}

		ImGui::Separator();

		// Take care with the order in the combo, it has to follow the ComponentType enum class order (ignore enum[0] --> NONE)
		ImGui::Combo("##", &componentType, "Add Component\0Transform\0Mesh\0Material\0Info\0Camera\0RigidBody");

		ImGui::SameLine();

		if ((ImGui::Button("ADD")))
		{
			if (componentType != (int)ComponentType::NONE)
			{
				currentGameObject->AddComponentByType((ComponentType)componentType);
			}
		}
	}

	ImGui::End();
	return true;
}

bool PanelInspector::PostUpdate()
{
	return true;
}
