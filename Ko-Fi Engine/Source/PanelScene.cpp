#include "PanelScene.h"
#include <imgui.h>

#include "Editor.h"
#include "GameObject.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

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
	//ImGui::Begin("Scene Hierarchy");

	//if (!editor->gameObjects.empty())
	//{
	//	int index = 0;
	//	int nodeClicked = -1;
	//	for (GameObject obj : gameObjects)
	//	{
	//		ImGui::PushID(index);
	//		boolean treeNodeOpen = ImGui::TreeNodeEx(
	//			obj.GetName().c_str(),
	//			ImGuiTreeNodeFlags(ImGuiTreeNodeFlags_DefaultOpen |
	//				ImGuiTreeNodeFlags_FramePadding |
	//				ImGuiTreeNodeFlags_OpenOnArrow |
	//				ImGuiTreeNodeFlags_SpanAvailWidth),
	//			obj.GetName().c_str()
	//		);
	//		ImGui::PopID();

	//		if (treeNodeOpen) {
	//			ImGui::TreePop();
	//		}

	//		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	//		{
	//			nodeClicked = index;
	//			editor->panelGameObjectInfo.currentGameObjectID = nodeClicked;
	//		}

	//		index++;
	//	}
	//}

	//ImGui::End();

    ImGui::Begin("Scene Hierarchy");

    editor->Markdown("# Game Objects");

    std::vector<GameObject*> gameObjects = editor->gameObjects; // It should be an std::list and located in SceneIntro...
    
    if (!gameObjects.empty())
    {
        if (ImGui::TreeNode("Advanced, with Selectable nodes"))
        {
            HelpMarker(
                "This is a more typical looking tree with selectable nodes.\n"
                "Click to select, CTRL+Click to toggle, click on arrows or double-click to open.");
            static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            static bool align_label_with_current_x_position = false;
            static bool test_drag_and_drop = false;
            /*ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnArrow", &base_flags, ImGuiTreeNodeFlags_OpenOnArrow);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_OpenOnDoubleClick", &base_flags, ImGuiTreeNodeFlags_OpenOnDoubleClick);
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanAvailWidth", &base_flags, ImGuiTreeNodeFlags_SpanAvailWidth); ImGui::SameLine(); HelpMarker("Extend hit area to all available width instead of allowing more items to be laid out after the node.");
            ImGui::CheckboxFlags("ImGuiTreeNodeFlags_SpanFullWidth", &base_flags, ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::Checkbox("Align label with current X position", &align_label_with_current_x_position);
            ImGui::Checkbox("Test tree node as drag source", &test_drag_and_drop);*/
            if (align_label_with_current_x_position)
                ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
            // 'selection_mask' is dumb representation of what may be user-side selection state.
            //  You may retain selection state inside or outside your objects in whatever format you see fit.
            // 'node_clicked' is temporary storage of what node we have clicked to process selection at the end
            /// of the loop. May be a pointer to your own node type, etc.
            static int selection_mask = (1 << 2);
            int node_clicked = -1;
            for (int i = 0; i < gameObjects.size(); i++)
            {
                // Disable the default "open on single-click behavior" + set Selected flag according to our selection.
                // To alter selection we use IsItemClicked() && !IsItemToggledOpen(), so clicking on an arrow doesn't alter selection.
                ImGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = (selection_mask & (1 << i)) != 0;
                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                // Items 0..2 are Tree Node
                bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, gameObjects.at(i)->GetName().c_str(), i);
                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                    node_clicked = i;
                if (test_drag_and_drop && ImGui::BeginDragDropSource())
                {
                    ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                    ImGui::Text("This is a drag and drop source");
                    ImGui::EndDragDropSource();
                }
                if (node_open)
                {
                    ImGui::TreePop();

                    // Tree Leaves
                    // The only reason we use TreeNode at all is to allow selection of the leaf. Otherwise we can
                    // use BulletText() or advance the cursor by GetTreeNodeToLabelSpacing() and call Text().
                    std::vector<Component*> components = gameObjects.at(i)->GetComponents();
                    for (int i = 0; i < components.size(); i++)
                    {
                        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                        std::string componentName;
                        switch (components.at(i)->type)
                        {
                        case COMPONENT_TYPE::COMPONENT_TRANSFORM:
                            componentName = "Transform";
                            break;
                        case COMPONENT_TYPE::COMPONENT_MESH:
                            componentName = "Mesh";
                            break;
                        case COMPONENT_TYPE::COMPONENT_MATERIAL:
                            componentName = "Material";
                            break;
                        }
                        ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, componentName.c_str(), i);
                        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                            node_clicked = i;
                        if (test_drag_and_drop && ImGui::BeginDragDropSource())
                        {
                            ImGui::SetDragDropPayload("_TREENODE", NULL, 0);
                            ImGui::Text("This is a drag and drop source");
                            ImGui::EndDragDropSource();
                        }
                    }
                }
            }
            if (node_clicked != -1)
            {
                // Update selection state
                // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
                if (ImGui::GetIO().KeyCtrl)
                    selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
                else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
                    selection_mask = (1 << node_clicked);           // Click to single-select

                // Set the current selected GameObject in the Editor to be managed by the InspectorPanel
                editor->panelGameObjectInfo.currentGameObjectID = node_clicked;
            }
            if (align_label_with_current_x_position)
                ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
            ImGui::TreePop();
        }
    }

    ImGui::End();

	return true;
}

bool PanelScene::PostUpdate()
{
	return true;
}
