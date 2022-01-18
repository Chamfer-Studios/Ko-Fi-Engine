#pragma once
#include "Panel.h"
#include <vector>
class Editor;

#include "Node.h"

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
	void DrawNodes(Node* node);
	void RightClickListener();
	void NodeHoveringAndSelectingListener();
	void SaveNodeEditor();
	void LoadNodeEditor(const char* path);
private:
	Editor* editor;
	std::vector<Node*> nodes;
	std::vector<Link> links;
	std::vector<int> selectedNodes;

	int currentId;
	int hoveredId = 0;

};