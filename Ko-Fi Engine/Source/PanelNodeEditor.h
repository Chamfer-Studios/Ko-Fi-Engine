#pragma once
#include "Panel.h"
#include <vector>
class Editor;

enum class NodeType
{
	add,
	multiply,
	output,
	sine,
	time,
	value
};
struct Node
{
	NodeType type;
	int id;
	float value;
	explicit Node(const NodeType t,int id) : type(t),id(id), value(0.f) {}

	Node(const NodeType t, int id,const float v) : type(t), id(id), value(v) {}
};

struct Link
{
	int id;
	int start_attr, end_attr;
};


class PanelNodeEditor : public Panel {
public:
	PanelNodeEditor(Editor* editor);
	~PanelNodeEditor();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Editor* editor;
	std::vector<Node> nodes;;
	std::vector<Link> links;
	int current_id;

};