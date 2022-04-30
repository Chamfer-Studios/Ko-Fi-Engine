#include "PanelHierarchy.h"
#include <imgui.h>

#include "Engine.h"
#include "M_Editor.h"
#include "M_SceneManager.h"
#include "M_Input.h"

#include "GameObject.h"
#include "PanelChooser.h"
#include "Log.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
	ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(1, 0.75, 0, 1));
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	ImGui::PopStyleColor();
}

PanelHierarchy::PanelHierarchy(M_Editor* editor)
{
	this->editor = editor;
	panelName = "Hierarchy";
}

PanelHierarchy::~PanelHierarchy()
{
	CleanUp();
}

bool PanelHierarchy::CleanUp()
{
	editor = nullptr;
	selectedGameObject = nullptr;
	destinationGameObject = nullptr;

	return true;
}

bool PanelHierarchy::Awake()
{
	return true;
}

bool PanelHierarchy::Update()
{
	OPTICK_EVENT();

	ImGui::Begin("Scene Hierarchy");

	if (!editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList.empty())
	{
		editor->Markdown("# Game Objects");
		ImGui::SameLine();
		ImGui::Text("Here you can manage your game objects.");
		ImGui::SameLine();
		HelpMarker(
			"This is a more typical looking tree with selectable nodes.\n"
			"Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
		
		static bool alignLabelWithCurrentXPosition = false;

		if (alignLabelWithCurrentXPosition)
			ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		int id = 0;
		DisplayTree(editor->engine->GetSceneManager()->GetCurrentScene()->rootGo, flags, id);

		if (alignLabelWithCurrentXPosition)
			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());

		if (editor->GetPanelChooser()->IsReadyToClose("CreatePrefab")) {
			if (editor->GetPanelChooser()->OnChooserClosed() != nullptr) {
				std::string path = editor->GetPanelChooser()->OnChooserClosed();
				Importer::GetInstance()->sceneImporter->Import(path.c_str(), true);
			}
		}
		if (ImGui::Button("Create Prefab")) {
			editor->GetPanelChooser()->OpenPanel("CreatePrefab", "fbx", {"fbx","dae","obj","stl","gltf"});
		}
		if (editor->GetPanelChooser()->IsReadyToClose("LoadPrefab")) {
			if (editor->GetPanelChooser()->OnChooserClosed() != nullptr) {
				std::string path = editor->GetPanelChooser()->OnChooserClosed();
				//Importer::GetInstance()->sceneImporter->Import(path.c_str(), true);
				GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
				go->LoadPrefabJson(path.c_str(), false);
			}
		}
		if (ImGui::Button("Open Prefab")) {
			editor->GetPanelChooser()->OpenPanel("LoadPrefab", "json", { "json" });
		}
	}

	ImGui::End();

	return true;
}

void PanelHierarchy::DisplayTree(GameObject* go, int flags, int& id)
{
	ImGui::PushID(id);
	if (go->isPrefab)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.50f, 1.00f, 1.f);
	}
	if (go->GetChildren().size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (dragging)
	{
		//Drawing inter-GO buttons
		ImVec2 cursorPos = ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y - 4.0f);
		ImGui::SetCursorScreenPos(cursorPos);
		ImGui::PushID(go->GetUID()); //TODO: should be done in tree header, we need a new id here
		ImVec2 buttonSize = ImVec2(ImGui::GetWindowSize().x, 4);
		ImGui::InvisibleButton("Button", buttonSize);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(ImVec2(cursorPos), ImVec2(cursorPos.x + ImGui::GetWindowSize().x, cursorPos.y + 4), ImGui::GetColorU32(ImGuiCol_HeaderActive));
			if (editor->engine->GetInput()->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
			{
				//1 attach child
				if (go->GetParent() == selectedGameObject->GetParent())
				{
					auto insertIndex = std::find(go->GetParent()->children.begin(), go->GetParent()->children.end(), go);
					auto moveIndex = std::find(go->GetParent()->children.begin(), go->GetParent()->children.end(), selectedGameObject);
					ReorderElement(go->GetParent()->children, insertIndex - go->GetParent()->children.begin(), moveIndex - go->GetParent()->children.begin());
				}
				else {
					go->GetParent()->AttachChild(selectedGameObject);
					auto insertIndex = std::find(go->GetParent()->children.begin(), go->GetParent()->children.end(), go);
					auto moveIndex = std::find(go->GetParent()->children.begin(), go->GetParent()->children.end(), selectedGameObject);
					ReorderElement(go->GetParent()->children, insertIndex - go->GetParent()->children.begin(), moveIndex - go->GetParent()->children.begin());
				}

				/*auto insertIndex = std::find(go->GetParent()->children.begin(), go->GetParent()->children.end(), go);
				auto moveIndex = std::find(selectedGameObject->GetParent()->children.begin(), selectedGameObject->GetParent()->children.end(), selectedGameObject);
				ReorderElement(insertIndex- editor->engine->GetSceneManager()->GetCurrentScene()->rootGo->children.begin(),moveIndex- editor->engine->GetSceneManager()->GetCurrentScene()->rootGo->children.begin());*/
			}
		}
		ImGui::PopID();
		ImGui::SetCursorScreenPos(ImVec2(ImGui::GetCursorScreenPos().x - 0.0f, ImGui::GetCursorScreenPos().y - 4.0f));

	}
	

	if (ImGui::TreeNodeEx(go->GetName(), flags))
	{
		DragNDrop(go);
		if (((ImGui::IsItemDeactivated() && ImGui::IsItemHovered()) || ImGui::IsItemClicked(1)))
		{
			editor->panelGameObjectInfo.selectedGameObjects.clear();
			editor->panelGameObjectInfo.selectedGameObjects.shrink_to_fit();
			editor->panelGameObjectInfo.selectedGameObjects.push_back(go->GetUID());

			CONSOLE_LOG("%s || %d", go->GetName(), go->GetUID());
		}
		if (ImGui::IsItemClicked(1)) {
			ImGui::OpenPopup("Test");
		}
		
		for (int i = 0; i < go->GetChildren().size(); i++)
		{

			DisplayTree(go->GetChildren().at(i), flags, ++id);

		}
		if (ImGui::BeginPopup("Test"))
		{
			if (ImGui::MenuItem("Create Empty Child")) {
				GameObject* child = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
				for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
					for (int i = 0; i < editor->panelGameObjectInfo.selectedGameObjects.size(); i++)
					{
						if (go->GetUID() == editor->panelGameObjectInfo.selectedGameObjects[i])
							go->AttachChild(child);
					}
				}
			}
			if (ImGui::MenuItem("Delete")) {
				for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
					auto it = std::find(editor->panelGameObjectInfo.selectedGameObjects.begin(), editor->panelGameObjectInfo.selectedGameObjects.end(), go->GetUID());
					if (it == editor->panelGameObjectInfo.selectedGameObjects.end()) continue;

					editor->engine->GetSceneManager()->GetCurrentScene()->DeleteGameObject(go);
				}
			}
			if (ImGui::MenuItem("Set as Prefab")) {
				for (GameObject* go : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList) {
					for (int i = 0; i < editor->panelGameObjectInfo.selectedGameObjects.size(); i++)
					{
						if (go->GetUID() == editor->panelGameObjectInfo.selectedGameObjects[i] && go->GetUID() != -1) {
							go->isPrefab = true;
							editor->panelGameObjectInfo.selectedGameObjects.erase(editor->panelGameObjectInfo.selectedGameObjects.begin() + i);
						}
					}
				}
			}
			ImGui::EndPopup();
		}
		ImGui::TreePop();

	}
	else
	{
		DragNDrop(go);
	}
	if (go->isPrefab)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.f);
	}
	ImGui::PopID();
}

void PanelHierarchy::DragNDrop(GameObject* go)
{
	if (ImGui::BeginDragDropSource())
	{
		dragging = true;
		ImGui::SetDragDropPayload("Hierarchy", go, sizeof(GameObject));
		selectedGameObject = go;
		ImGui::Text(go->GetName());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy");
		if (payload != nullptr)
		{
			if (selectedGameObject != nullptr)
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
				{
					destinationGameObject = go;
					destinationGameObject->AttachChild(selectedGameObject);
					selectedGameObject = nullptr;
					destinationGameObject = nullptr;
				}
			}
		}
		ImGui::EndDragDropTarget();
		dragging = false;
	}
}

void PanelHierarchy::ReorderElement(std::vector<GameObject*>& list, int insertIndex, int moveIndex)
{
	
	while (moveIndex > insertIndex)
	{
		std::swap(list[moveIndex], list[moveIndex - 1]);
		--moveIndex;
	}
}
