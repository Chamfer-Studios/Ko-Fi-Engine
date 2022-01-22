#pragma once
#include <ImNodes.h>
enum class NodeType
{
	MATERIAL,
	TEXTURE,
	COLOR,
	VEC1,
	ADD,

};
enum class PinType {
	BOOL,
	INT,
	FLOAT,
	STRING,
	VEC4,
	UINT
};

class Node;
struct Pin
{
	int id;
	Node* node;
	PinType type;
	Pin(int id, PinType type):id(id),node(nullptr),type(type){}
};

class Node
{
public:
	Node() = default;

	explicit Node(const char* name, const NodeType t, const int id) : type(t), id(id), name(name) {}

	Node(const char* name, const NodeType t, const int id, const float v) : type(t), id(id), name(name) {}
	virtual void Update() {};
	virtual void Render() {};
public:
	NodeType type;
	std::vector<Node*> inputs_vec;
	std::vector<Node*> outputs_vec;
	
	std::vector<Pin> inputPins;
	std::vector<Pin> outputPins;
	const char* name;
	int id;
};

