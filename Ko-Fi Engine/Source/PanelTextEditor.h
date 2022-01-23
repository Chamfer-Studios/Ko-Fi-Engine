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
	void RenderWindow(bool* toggleEditText);
	void LoadFile(std::string path = "");
	void SaveFile(std::string path);
	void Focus();
private:
	void ChooserListener();
private:
	Editor* editor;
	TextEditor textEditor;
	std::string filePath;
};