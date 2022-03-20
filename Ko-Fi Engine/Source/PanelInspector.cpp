#include "PanelInspector.h"
#include <imgui.h>
#include "Editor.h"
#include "Engine.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentScript.h"
#include "ComponentCamera.h"
#include "ComponentInfo.h"
#include "ComponentTransform.h"
#include "ComponentCollider.h"
#include "ComponentRigidBody.h"

#include <queue>

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
		/*if (currentGameObject->GetComponent<ComponentMesh>() != nullptr && (currentGameObject->GetComponent<ComponentMesh>()->GetMesh() != nullptr))
			currentGameObject->GetComponent<ComponentMesh>()->DrawBoundingBox(currentGameObject->GetComponent<ComponentMesh>()->GetMesh()->localAABB, float3(1.0f, 0.0f, 0.0f));*/

		for (Component* component : currentGameObject->GetComponents())
		{
			component->InspectorDraw(editor->GetPanelChooser());
		}

		ImGui::Separator();

		// Take care with the order in the combo, it has to follow the ComponentType enum class order
		ImGui::Combo("##combo", &componentType, "Add Component\0Mesh\0Material\0Camera\0Collider\0Script\0RigidBody\0Transform 2D\0Canvas\0Image\0Button\0Text\0Transform\0Walkable\0Follow Path");

		ImGui::SameLine();

		if ((ImGui::Button("ADD")))
		{
			if (componentType != (int)ComponentType::NONE)
			{
				currentGameObject->AddComponentByType((ComponentType)componentType);
				componentType = 0;
			}
		}

		if ((ImGui::Button("ADD TO CHILDREN")))
		{
			if (componentType != (int)ComponentType::NONE)
			{
				std::queue<GameObject*> q;

				for (auto o : currentGameObject->children) {
					q.push(o);
				}

				while (!q.empty()) {
					GameObject* go = q.front();
					q.pop();

					for (auto c : go->children) {
						q.push(c);
					}

					go->AddComponentByType((ComponentType)componentType);
				}

				componentType = 0;
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
