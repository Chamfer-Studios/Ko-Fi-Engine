#include "PanelInspector.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentScript.h"

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

		ImGui::Combo("##combo", &componentType, "Add Component\0Mesh\0Material\0Camera\0Collider\0Script\0RigidBody");

		ImGui::SameLine();

		if ((ImGui::Button("ADD")))
		{
			switch (componentType)
			{
			case (int)ComponentType::NONE: break;
			case (int)ComponentType::MESH: currentGameObject->CreateComponent<ComponentMesh>(); break;
			case (int)ComponentType::MATERIAL: currentGameObject->CreateComponent<ComponentMaterial>(); break;
			//case (int)ComponentType::CAMERA: currentGameObject->CreateComponent<ComponentCamera>(); break;
			//case (int)ComponentType::COLLIDER: currentGameObject->CreateComponent<ComponentCollider>(); break;
			case (int)ComponentType::SCRIPT: currentGameObject->CreateComponent<ComponentScript>(); break;
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
