
#pragma once
#include "Node.h"
class Vec1Node : public Node {

public:
	Vec1Node(const char* name, const NodeType t, const int id) : Node(name, t, id) {
		inputPins.push_back(Pin(this->id << 8, PinType::FLOAT));
	}
	~Vec1Node() {}
	void Update() {
		
	}
	void Render() {
		ImNodes::BeginNode(this->id);
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Vec1");
		ImNodes::EndNodeTitleBar();
		ImNodes::BeginOutputAttribute(this->id << 8, ImNodesPinShape_TriangleFilled);
		ImGui::PushItemWidth(45);
		ImGui::InputFloat("Number",&number);
		ImGui::PopItemWidth();
		ImNodes::EndOutputAttribute();
		ImNodes::EndNode();

	}
public:
	
	float number = 0;

};