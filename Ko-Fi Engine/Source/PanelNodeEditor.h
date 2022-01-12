#pragma once
#include "Panel.h"
#include <vector>
class Editor;

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
	std::vector<std::pair<int, int>> links;
	int node_hovered;

};