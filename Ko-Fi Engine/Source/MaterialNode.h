#ifndef __MATERIAL_NODE_H__
#define __MATERIAL_NODE_H__

#include "Node.h"
#include "MathGeoLib/Math/float4.h"

class MaterialNode : public Node
{
public: 
	MaterialNode(const char* name, const NodeType t, const int id) : Node(name,t,id) {}
	~MaterialNode(){}
	void Update() {
		
	}
	void Render(){
		ImNodes::BeginNode(this->id);
		ImNodes::BeginNodeTitleBar();
		ImGui::TextUnformatted("Material");
		ImNodes::EndNodeTitleBar();

		ImNodes::BeginInputAttribute(this->id << 8,ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("Base Color");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(this->id << 16, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("Metallic");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(this->id << 24, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("Roughness");
		ImNodes::EndInputAttribute();
		ImNodes::BeginInputAttribute(this->id << 64, ImNodesPinShape_TriangleFilled);
		ImGui::TextUnformatted("Ambient Occlusion");
		ImNodes::EndInputAttribute();
		ImNodes::EndNode();
	}

private:
	float4 albedoColor = { 1.0f,1.0f,1.0f,1.0f };
	float metallic = 0;
	float roughness = 0;
	float ambientOcclusion = 0;
};

#endif // !__MATERIAL_NODE_H__