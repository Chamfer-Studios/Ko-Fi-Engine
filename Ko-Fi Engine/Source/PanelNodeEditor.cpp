#include "PanelNodeEditor.h"
#include "Editor.h"
#include "Engine.h"
#include "Input.h"

#include <ImNodes.h>
PanelNodeEditor::PanelNodeEditor(Editor* editor)
{
	this->editor = editor;
	panelName = "Node Editor";
}

PanelNodeEditor::~PanelNodeEditor()
{
}

bool PanelNodeEditor::Awake()
{
	return true;
}

bool PanelNodeEditor::PreUpdate()
{
	return true;
}

bool PanelNodeEditor::Update()
{
	//Begin window
	ImGui::Begin(panelName.c_str(), 0);
	ImGui::TextUnformatted("Press A to add a node");
	//Begin editor
	ImNodes::BeginNodeEditor();
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) &&
		ImNodes::IsEditorHovered() && editor->engine->GetInput()->GetKey(SDL_SCANCODE_A) == KEY_UP )
	{
		const int node_id = ++current_id;
		ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
		nodes.push_back(Node(NodeType::add, node_id));
	}

    for (Node& node : nodes)
    {
        ImNodes::BeginNode(node.id);

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted("node");
        ImNodes::EndNodeTitleBar();

        ImNodes::BeginInputAttribute(node.id << 8);
        ImGui::TextUnformatted("input");
        ImNodes::EndInputAttribute();

        ImNodes::BeginStaticAttribute(node.id << 16);
        ImGui::PushItemWidth(120.f);
        ImGui::DragFloat("value", &node.value, 0.01f);
        ImGui::PopItemWidth();
        ImNodes::EndStaticAttribute();

        ImNodes::BeginOutputAttribute(node.id << 24);
        const float text_width = ImGui::CalcTextSize("output").x;
        ImGui::Indent(120.f + ImGui::CalcTextSize("value").x - text_width);
        ImGui::TextUnformatted("output");
        ImNodes::EndOutputAttribute();

        ImNodes::EndNode();
    }

    for (const Link& link : links)
    {
        ImNodes::Link(link.id, link.start_attr, link.end_attr);
    }
	ImNodes::EndNodeEditor();

    Link link;
    if (ImNodes::IsLinkCreated(&link.start_attr, &link.end_attr))
    {
        link.id = ++current_id;
        links.push_back(link);
    }

    int link_id;
    if (ImNodes::IsLinkDestroyed(&link_id))
    {
        auto iter =
            std::find_if(links.begin(), links.end(), [link_id](const Link& link) -> bool {
            return link.id == link_id;
                });
        assert(iter != links.end());
        links.erase(iter);
    }
	ImGui::End();
	return true;
}

bool PanelNodeEditor::PostUpdate()
{
	return true;
}
