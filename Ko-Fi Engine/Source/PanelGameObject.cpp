#include "PanelGameObject.h"
#include <imgui.h>
#include "Editor.h"
#include "GameObject.h"

PanelGameObject::PanelGameObject(Editor* editor)
{
	this->editor = editor;
}

PanelGameObject::~PanelGameObject()
{
}

bool PanelGameObject::Awake()
{
	return true;
}

bool PanelGameObject::PreUpdate()
{
	return true;
}

bool PanelGameObject::Update()
{
	// Panel game object info. to manage the options of the current game object
	PanelGameObjectInfo panelGameObjectInfo = editor->panelGameObjectInfo;

	if (panelGameObjectInfo.currentGameObjectID == -1)
	{
		ImGui::Begin("Game Object (empty)");
		ImGui::Text("There is no selected Game Object.");
		ImGui::Text("Select one to display its options.");
	}
	else
	{
		// Current game object (the one we have selected at the moment)
		GameObject* currentGameObject = &editor->gameObjects.at(editor->panelGameObjectInfo.currentGameObjectID);
		ImGui::Begin(currentGameObject->GetName().c_str());

		if (ImGui::Button("Debug options"))
		{
			debugOptions = !debugOptions;
		}

		if (debugOptions)
		{
			// Draw vertex normals
			if (ImGui::Checkbox("Draw vertex normals", &drawVertexNormals))
			{
				for (int i = 0; i < currentGameObject->GetMeshes()->size(); i++)
				{
					currentGameObject->GetMeshes()->at(i).ToggleVertexNormals();
				}
			}

			// Draw faces normals
			if (ImGui::Checkbox("Draw faces normals", &drawFacesNormals))
			{
				for (int i = 0; i < currentGameObject->GetMeshes()->size(); i++)
				{
					currentGameObject->GetMeshes()->at(i).ToggleFacesNormals();
				}
			}
			// Draw faces normals
			if (ImGui::	Button("New Texture"))
			{
				
					currentGameObject->SetNewTexture("Assets/Images/brick.jpg");
				
			}
		}
	}

	ImGui::End();

	return true;
}

bool PanelGameObject::PostUpdate()
{
	return true;
}
