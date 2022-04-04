#ifndef __NODE_H__
#define __NODE_H__

#include <ImNodes.h>
#include <vector>
#include "Globals.h"

enum class NodeType
{
	MATERIAL,
	TEXTURE,
	COLOR,
	VEC1,
	ADD,
	NONE,

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
	Node* node = nullptr;
	PinType type;
	Pin(int id, PinType type):id(id),node(nullptr),type(type){}
	~Pin() { RELEASE(node); }
};

class Node
{
public:
	Node() = default;
	~Node() {
		for (std::vector<Node*>::const_iterator i = inputs_vec.begin(); i != inputs_vec.end(); ++i)
		{
			inputs_vec.erase(i);
		}
		inputs_vec.clear();
		inputs_vec.shrink_to_fit();

		for (std::vector<Node*>::const_iterator i = outputs_vec.begin(); i != outputs_vec.end(); ++i)
		{
			outputs_vec.erase(i);
		}

		outputs_vec.clear();
		outputs_vec.shrink_to_fit();

		for (std::vector<Pin>::const_iterator i = inputPins.begin(); i != inputPins.end(); ++i)
		{
			inputPins.erase(i);
		}
		inputPins.clear();
		inputPins.shrink_to_fit();

		for (std::vector<Pin>::const_iterator i = outputPins.begin(); i != outputPins.end(); ++i)
		{
			outputPins.erase(i);
		}
		outputPins.clear();
		outputPins.shrink_to_fit();

		name = nullptr;

	}
	explicit Node(const char* name, const NodeType t, const int id) : type(t), id(id), name(name) {}

	Node(const char* name, const NodeType t, const int id, const float v) : type(t), id(id), name(name) {}
	virtual void Update() {};
	virtual void Render() {};

public:
	NodeType type = NodeType::NONE;
	std::vector<Node*> inputs_vec;
	std::vector<Node*> outputs_vec;
	
	std::vector<Pin> inputPins;
	std::vector<Pin> outputPins;
	const char* name = nullptr;
	int id = 0;
};

#endif // !__NODE_H__