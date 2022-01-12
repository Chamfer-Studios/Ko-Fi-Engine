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

	//Begin editor
	ImNodes::BeginNodeEditor();
    //Check mouse clicking
    
    RightClickListener();
    //Draw all nodes in the nodes vector
    for (Node& node : nodes)
    {
        DrawNodes(node);
    }

    for (const Link& link : links)
    {
        ImNodes::Link(link.id, link.start_attr, link.end_attr);
    }
    ImNodes::MiniMap(0.2f,ImNodesMiniMapLocation_TopRight);
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

void PanelNodeEditor::CreateNode(NodeType type)
{
    const int node_id = ++current_id;
    switch (type) {
    case NodeType::SUM:
        ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
        nodes.push_back(Node("Add", type, node_id));
        break;
    case NodeType::MULTIPLY:
        ImNodes::SetNodeScreenSpacePos(node_id, ImGui::GetMousePos());
        nodes.push_back(Node("Multiply", type, node_id));
        break;
    default:
        break;
    }
}

void PanelNodeEditor::DrawNodes(Node node)
{
    ImNodes::BeginNode(node.id);

    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted(node.name);
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

void PanelNodeEditor::RightClickListener()
{
    if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
        ImGui::OpenPopup("Create Node");
    }

    if (ImGui::BeginPopup("Create Node"))
    {
        if (ImGui::MenuItem("Sum")) {
            CreateNode(NodeType::SUM);
        }
        if (ImGui::MenuItem("Multiply")) {
            CreateNode(NodeType::MULTIPLY);

        }
        ImGui::EndPopup();
    }
}
