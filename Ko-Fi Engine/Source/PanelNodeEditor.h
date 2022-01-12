#pragma once
#include "Panel.h"
#include <vector>
class Editor;

enum class NodeType
{
	SUM,
	MULTIPLY,

};
struct Node
{
	NodeType type;
	const char* name;
	int id;
	float value;
	explicit Node(const char* name,const NodeType t,int id) : type(t),id(id), name(name), value(0.f) {}

	Node(const char* name,const NodeType t, int id,const float v) : type(t), id(id), name(name), value(v) {}
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
	//Node editor functions
	void CreateNode(NodeType type);
	void DrawNodes(Node node);
	void RightClickListener();
	
private:
	Editor* editor;
	std::vector<Node> nodes;;
	std::vector<Link> links;
	int current_id;

};