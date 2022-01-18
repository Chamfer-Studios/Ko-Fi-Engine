#pragma once
#include "Panel.h"
class Editor;
#include "TextEditor.h"
class PanelTextEditor : public Panel {
public:
	PanelTextEditor(Editor* editor);
	PanelTextEditor(Editor* editor,const char* path);
	~PanelTextEditor();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Editor* editor;
	TextEditor textEditor;
};