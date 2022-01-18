#pragma once
#include <ImNodes.h>
enum class NodeType
{
	MATERIAL,
	TEXTURE,
	COLOR,

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
	const char* name;
	int id;
	

};