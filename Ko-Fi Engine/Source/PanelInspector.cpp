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
#include "ComponentAnimator.h"
#include "ComponentLightSource.h"

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

		if (currentGameObject->isPrefab)
		{
			if (ImGui::Button("Update Changes"))
			{
				currentGameObject->PrefabSaveJson();
			}
		}

		for (Component* component : currentGameObject->GetComponents())
		{
			component->InspectorDraw(editor->GetPanelChooser());
		}

		ImGui::Separator();
		
		if (ImGui::BeginCombo("Add Component##", "Add Component"))
		{
			for (int i = (int)ComponentType::NONE+1; i != (int)ComponentType::END; ++i)
			{
				std::string componentTypeName = componentTypeUtils::ComponentTypeToString((ComponentType)i);
				if (ImGui::Selectable(componentTypeName.c_str()))
				{
					currentGameObject->AddComponentByType((ComponentType)i);
				}
			}
			ImGui::EndCombo();
		}

	}
	ImGui::End();
	return true;
}

bool PanelInspector::PostUpdate()
{
	return true;
}
