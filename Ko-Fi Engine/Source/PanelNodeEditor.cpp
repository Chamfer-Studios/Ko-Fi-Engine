#include "PanelNodeEditor.h"
#include "M_Editor.h"
#include <fstream>
#include <ios> // for std::streamsize
#include "PanelChooser.h"

//Nodes
#include "MaterialNode.h"
#include "Vec1Node.h"
#include "AdditionNode.h"

#include <ImNodes.h>
PanelNodeEditor::PanelNodeEditor(M_Editor* editor)
{
	this->editor = editor;
	panelName = "Node M_Editor";
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
    if (ImGui::Button("Save")) {
        SaveNodeEditor();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        editor->GetPanelChooser()->OpenPanel("NodeEditor", "ini", {"ini"});
    }
    if (editor->GetPanelChooser()->IsReadyToClose("NodeEditor"))
    {
        const char* path = editor->GetPanelChooser()->OnChooserClosed();
        if (path != nullptr)
        {
            LoadNodeEditor(path);
        }
    }
	//Begin editor
	ImNodes::BeginNodeEditor();
    //Check mouse clicking
    RightClickListener();

   
    //Draw all nodes in the nodes vector
    for (Node* node : nodes)
    {
        node->Update();
        DrawNodes(node);
    }

    for (const Link& link : links)
    {
        ImNodes::Link(link.id, link.start_attr, link.end_attr);
    }
    ImNodes::MiniMap(0.2f,ImNodesMiniMapLocation_TopRight);
	ImNodes::EndNodeEditor();

    NodeHoveringAndSelectingListener();
    Link link;
    if (ImNodes::IsLinkCreated(&link.start_node,&link.start_attr,&link.end_node, &link.end_attr))
    {
        link.id = ++currentId;
        links.push_back(link);
        Node* inputNode = nullptr;
        Node* outputNode = nullptr;
        for (Node* node : nodes) {
            if (node->id == link.start_node) {
                inputNode = node;
            }
            if (node->id == link.end_node) {
                outputNode = node;
            }
        }
        if (inputNode != nullptr && outputNode != nullptr) {
            inputNode->inputs_vec.push_back(outputNode);
            outputNode->inputs_vec.push_back(inputNode);
            
        }
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

int PanelNodeEditor::CreateNode(NodeType type)
{
    const int nodeId = ++currentId;
    switch (type) {
    case NodeType::MATERIAL:
        ImNodes::SetNodeScreenSpacePos(nodeId, ImGui::GetMousePos());
        nodes.push_back((Node*) new MaterialNode("Material",NodeType::MATERIAL,nodeId));
        return nodeId;
    case NodeType::VEC1:
        ImNodes::SetNodeScreenSpacePos(nodeId, ImGui::GetMousePos());
        nodes.push_back((Node*) new Vec1Node("Vec1", NodeType::VEC1, nodeId));        
        return nodeId;
    case NodeType::ADD:
        ImNodes::SetNodeScreenSpacePos(nodeId, ImGui::GetMousePos());
        nodes.push_back((Node*) new AdditionNode("Add", NodeType::VEC1, nodeId));
        return nodeId;
    default:
        break;
    }
}

void PanelNodeEditor::DrawNodes(Node* node)
{
        node->Render();
   
}

void PanelNodeEditor::RightClickListener()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.f, 8.f));

    if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered()) {
        //if (hoveredId!=0)
        //{
        //    ImGui::OpenPopup("Node Options");
        //}

        //else {
        //    ImGui::OpenPopup("Create Node");
        //}
        ImGui::OpenPopup("Create Node");
    }

    if (ImGui::BeginPopup("Create Node"))
    {
        const ImVec2 click_pos = ImGui::GetMousePosOnOpeningCurrentPopup();
        if (ImGui::MenuItem("Material")) {
            CreateNode(NodeType::MATERIAL);
        }
        if (ImGui::MenuItem("Vec1")) {
            CreateNode(NodeType::VEC1);
        }
        if (ImGui::MenuItem("Add")) {
            CreateNode(NodeType::ADD);
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Node Options"))
    {
        if (ImGui::MenuItem("Delete")) {
            
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

}
void PanelNodeEditor::NodeHoveringAndSelectingListener()
{
    //Checking if some node has been hovered
    int node_id;
    if (ImNodes::IsNodeHovered(&node_id))
    {
        hoveredId = node_id;
    }

    //Checking if any node or nodes have been hovered
    const int num_selected_nodes = ImNodes::NumSelectedNodes();
    if (num_selected_nodes > 0)
    {
        selectedNodes.resize(num_selected_nodes);
        ImNodes::GetSelectedNodes(selectedNodes.data());
    }
}

void PanelNodeEditor::SaveNodeEditor()
{
    ImNodes::SaveCurrentEditorStateToIniFile("Assets/Nodes/testNodes.ini");
    std::fstream fout("Assets/Nodes/save_load.bytes", std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
    const size_t num_nodes = nodes.size();
    //save the number of nodes
    fout.write(
        reinterpret_cast<const char*>(&num_nodes),
        static_cast<std::streamsize>(sizeof(size_t))
    );
    //save the nodes data
    fout.write(
        reinterpret_cast<const char*>(nodes.data()),
        static_cast<std::streamsize>(sizeof(Node) * num_nodes));

    //save the number of links
    const size_t num_links = links.size();
    fout.write(
        reinterpret_cast<const char*>(&num_links),
        static_cast<std::streamsize>(sizeof(size_t)));
    //save the links data
    fout.write(
        reinterpret_cast<const char*>(links.data()),
        static_cast<std::streamsize>(sizeof(Link) * num_links));
    // save the currentId
    fout.write(
        reinterpret_cast<const char*>(&currentId), static_cast<std::streamsize>(sizeof(int)));
}



void PanelNodeEditor::LoadNodeEditor(const char* path)
{
    // Load the internal imnodes state
    ImNodes::LoadCurrentEditorStateFromIniFile(path);

    // Load our editor state into memory

    std::fstream fin("Assets/Nodes/save_load.bytes", std::ios_base::in | std::ios_base::binary);

    if (!fin.is_open())
    {
        return;
    }

    // copy nodes into memory
    size_t num_nodes;
    fin.read(reinterpret_cast<char*>(&num_nodes), static_cast<std::streamsize>(sizeof(size_t)));
    nodes.resize(num_nodes);
    fin.read(
        reinterpret_cast<char*>(nodes.data()),
        static_cast<std::streamsize>(sizeof(Node) * num_nodes));

    // copy links into memory
    size_t num_links;
    fin.read(reinterpret_cast<char*>(&num_links), static_cast<std::streamsize>(sizeof(size_t)));
    links.resize(num_links);
    fin.read(
        reinterpret_cast<char*>(links.data()),
        static_cast<std::streamsize>(sizeof(Link) * num_links));

    // copy currentId into memory
    fin.read(reinterpret_cast<char*>(&currentId), static_cast<std::streamsize>(sizeof(int)));
}


