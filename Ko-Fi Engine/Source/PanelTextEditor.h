#ifndef __PANEL_TEXT_EDITOR_H__
#define __PANEL_TEXT_EDITOR_H__

#include "Panel.h"
class M_Editor;
#include "TextEditor.h"

class PanelTextEditor : public Panel
{
public:
	PanelTextEditor(M_Editor* editor);
	PanelTextEditor(M_Editor* editor,const char* path);
	~PanelTextEditor();

	bool Awake();
	bool Update();
	void RenderWindow(bool* toggleEditText);
	void LoadFile(std::string path = "",std::string ext = nullptr);
	void SaveFile(std::string path);
	void Focus();
private:
	void ChooserListener();
private:
	M_Editor* editor;
	TextEditor textEditor;
	std::string filePath;
};

#endif // !__PANEL_TEXT_EDITOR_H__