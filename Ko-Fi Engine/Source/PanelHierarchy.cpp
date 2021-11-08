#include "PanelHierarchy.h"
#include <imgui.h>

#include "Engine.h"
#include "Editor.h"
#include "SceneIntro.h"
#include "GameObject.h"

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

PanelHierarchy::PanelHierarchy(Editor* editor)
{
	this->editor = editor;
}

PanelHierarchy::~PanelHierarchy()
{
}

bool PanelHierarchy::Awake()
{
	return true;
}

bool PanelHierarchy::PreUpdate()
{
	return true;
}

bool PanelHierarchy::Update()
{

	ImGui::Begin("Scene Hierarchy");

	if (!editor->engine->GetSceneIntro()->gameObjectList.empty())
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
		for (int i = 0; i < editor->engine->GetSceneIntro()->rootGo->GetChildren().size(); ++i) {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
			
			DisplayTree(editor->engine->GetSceneIntro()->rootGo->GetChildren().at(i), flags);
			
		}
		
		
		if (alignLabelWithCurrentXPosition)
			ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
	}

	ImGui::End();

	return true;
}

bool PanelHierarchy::PostUpdate()
{
	return true;
}

void PanelHierarchy::DisplayTree(GameObject* go, int flags)
{
	if (go->GetChildren().size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;
	if (ImGui::TreeNodeEx(go->GetName().c_str(),flags))
	{
		DragNDrop(go);
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			editor->panelGameObjectInfo.currentGameObjectID = go->GetId();

		for (int i = 0; i < go->GetChildren().size(); i++)
		{
			
			DisplayTree(go->GetChildren().at(i),flags);
			
		}
		ImGui::TreePop();

	}
	else {
		DragNDrop(go);
	}
	
	
	

	
	
	
}

void PanelHierarchy::DragNDrop(GameObject* go)
{
	if (ImGui::BeginDragDropSource()) {

		ImGui::SetDragDropPayload("Hierarchy", go, sizeof(GameObject));
		selectedGameObject = go;
		ImGui::Text(go->name.c_str());
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy");
		if (payload != nullptr)
		{

			if (selectedGameObject != nullptr)
			{
				if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) {
					destinationGameObject = go;
					destinationGameObject->SetChild(selectedGameObject);
					selectedGameObject = nullptr;
					destinationGameObject = nullptr;

				}
			}
		}
		ImGui::EndDragDropTarget();
	}
}