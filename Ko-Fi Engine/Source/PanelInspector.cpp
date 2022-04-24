#include "PanelInspector.h"
#include <imgui.h>
#include "M_Editor.h"
#include "Engine.h"
#include "M_SceneManager.h"
#include "GameObject.h"
#include "C_Material.h"
#include "C_Mesh.h"
#include "C_Script.h"
#include "C_Camera.h"
#include "C_Info.h"
#include "C_Transform.h"
#include "C_RigidBody.h"
#include "C_Animator.h"
#include "C_LightSource.h"

#include <queue>

PanelInspector::PanelInspector(M_Editor* editor)
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

bool PanelInspector::Update()
{
	OPTICK_EVENT();

	// Panel game object info. to manage the options of the current game object
	PanelGameObjectInfo panelGameObjectInfo = editor->panelGameObjectInfo;

	ImGui::Begin("Inspector");
	for (int i = 0; i < editor->engine->GetEditor()->panelGameObjectInfo.selectedGameObjects.size(); i++)
	{
		
		if (panelGameObjectInfo.selectedGameObjects[i] != -1)
		{
			// Current game object (the one we have selected at the moment)
			GameObject* currentGameObject = editor->engine->GetSceneManager()->GetCurrentScene()->GetGameObject(editor->panelGameObjectInfo.selectedGameObjects[i]);
			/*if (currentGameObject->GetComponent<C_Mesh>() != nullptr && (currentGameObject->GetComponent<C_Mesh>()->GetMesh() != nullptr))
				currentGameObject->GetComponent<C_Mesh>()->DrawBoundingBox(currentGameObject->GetComponent<C_Mesh>()->GetMesh()->localAABB, float3(1.0f, 0.0f, 0.0f));*/

			ImGui::PushID(currentGameObject->GetUID());

			if (currentGameObject->isPrefab)
			{
				if (ImGui::Button("Save"))
				{
					currentGameObject->PrefabSaveJson();
				}
				if (ImGui::Button("Update changes"))
				{
					/*int prefabsCount = 0;
					for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
						if (go->prefabPath == currentGameObject->prefabPath && go->GetUID() != currentGameObject->GetUID())
						{
							editor->engine->GetSceneManager()->GetCurrentScene()->DeleteGameObject(go);
							prefabsCount++;
						}
					}
					for (int i = 0; i < prefabsCount; ++i)
					{
						GameObject* gameObj = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
						gameObj->LoadPrefabJson(currentGameObject->prefabPath.c_str(), true);
					}*/
					for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
						if (go->prefabPath == currentGameObject->prefabPath && go->GetUID() != currentGameObject->GetUID())
						{
							go->LoadPrefabJson(currentGameObject->prefabPath.c_str(), true);
						}
					}
				}
			}

			for (Component* component : currentGameObject->GetComponents())
			{
				component->InspectorDraw(editor->GetPanelChooser());
			}

			ImGui::Separator();

			if (ImGui::BeginCombo("Add Component##", "Add Component"))
			{
				for (int i = (int)ComponentType::NONE + 1; i != (int)ComponentType::END; ++i)
				{
					std::string componentTypeName = componentTypeUtils::ComponentTypeToString((ComponentType)i);
					if (ImGui::Selectable(componentTypeName.c_str()))
					{
						currentGameObject->AddComponentByType((ComponentType)i);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::PopID();

		}
	}
	
	ImGui::End();
	return true;
}
