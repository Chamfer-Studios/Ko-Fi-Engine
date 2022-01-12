#include "PanelNodeEditor.h"
#include <imgui.h>

#include <imnodes.h>
PanelNodeEditor::PanelNodeEditor(Editor* editor)
{
	this->editor = editor;
	panelName = "NodeEditor";
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
	int hardcoded_node_id = 1;
	//Beggin window
	ImGui::Begin(panelName.c_str(),0);
	//Beggin node editor
	imnodes::BeginNodeEditor();
	//Create a node
	imnodes::BeginNode(1);
	//the node has a title bar
	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output Node");
	imnodes::EndNodeTitleBar();
	//The output pin of the node
	 int output_attr_id = 2;
	imnodes::BeginOutputAttribute(2);
	ImGui::Text("B");
	imnodes::EndOutputAttribute();
	 int input_attr_id = 3;
	//the input pin of the node
	imnodes::BeginInputAttribute(3);
	ImGui::Text("A");
	imnodes::EndInputAttribute();
	imnodes::EndNode();


	imnodes::PushColorStyle(
		imnodes::ColorStyle_TitleBar, IM_COL32(255, 50,50, 255));
	imnodes::PushColorStyle(
		imnodes::ColorStyle_TitleBarSelected, IM_COL32(150, 50, 50, 255));
	imnodes::PushColorStyle(
		imnodes::ColorStyle_TitleBarHovered, IM_COL32(150, 50, 50, 255));
	//Create a node
	imnodes::BeginNode(4);

	//the node has a title bar
	imnodes::BeginNodeTitleBar();
	ImGui::TextUnformatted("Output Node 2");
	imnodes::EndNodeTitleBar();
	//The output pin of the node
	imnodes::BeginOutputAttribute(5);
	ImGui::Text("B");
	imnodes::EndOutputAttribute();
	//the input pin of the node
	imnodes::BeginInputAttribute(6);
	ImGui::Text("A");
	imnodes::EndInputAttribute();
	imnodes::EndNode();

	imnodes::PopColorStyle();
	imnodes::PopColorStyle();
	imnodes::PopColorStyle();
	for (int i = 0; i < links.size(); ++i)
	{
		const std::pair<int, int> p = links[i];
		// in this case, we just use the array index of the link
		// as the unique identifier
		imnodes::Link(i, p.first, p.second);
	}
	
	imnodes::EndNodeEditor();
	int start_attr, end_attr;
	if (imnodes::IsLinkCreated(&start_attr, &end_attr))
	{
		links.push_back(std::make_pair(start_attr, end_attr));
	}
    ImGui::End();
	return true;
}

bool PanelNodeEditor::PostUpdate()
{
	return true;
}
