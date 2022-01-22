#pragma once

#include "Node.h"
#include "Vec1Node.h"
class AdditionNode : public Node {

public:
	AdditionNode(const char* name, const NodeType t, const int id) : Node(name, t, id) {}
	~AdditionNode() {}
	void Update() {
	
		if (inputs_vec.size() > 0 && inputs_vec[0]) {
			a = ((Vec1Node*)inputs_vec[0])->number;
			
		}
		if (inputs_vec.size() > 1 && inputs_vec[1]) {
			b = ((Vec1Node*)inputs_vec[1])->number;

		}
		result = a + b;
	}
	void Render() {
		ImNodes::BeginNode(this->id);
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("AdditionNode");
		ImNodes::EndNodeTitleBar();
		ImNodes::BeginInputAttribute(this->id << 8,ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("A");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(this->id << 16, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("B");
		ImNodes::EndInputAttribute();
		ImNodes::BeginOutputAttribute(this->id << 32, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("Result: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0,1,1,1),"%.2f",result);
		ImNodes::EndOutputAttribute();
		ImNodes::EndNode();

	}
private:
	float a = 0;
	float b = 0;
public:
	float result = 0;

};