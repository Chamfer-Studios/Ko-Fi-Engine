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
	Node() = default;

	explicit Node(const char* name, const NodeType t, const int id) : type(t), id(id), name(name), value(0.f) {}

	Node(const char* name, const NodeType t, const int id, const float v) : type(t), id(id), name(name), value(v) {}
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
	int CreateNode(NodeType type);
	void DrawNodes(Node& node);
	void RightClickListener();
	void NodeHoveringAndSelectingListener();
	void SaveNodeEditor();
	void LoadNodeEditor(const char* path);
private:
	Editor* editor;
	std::vector<Node> nodes;
	std::vector<Link> links;
	std::vector<int> selectedNodes;

	int currentId;
	int hoveredId = 0;

};