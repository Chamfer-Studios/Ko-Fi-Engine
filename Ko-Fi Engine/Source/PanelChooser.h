#pragma once
#include "Panel.h"

#define FILE_MAX 250
class Editor;
class PanelChooser : public Panel
{
public:

	PanelChooser(Editor* editor);
	~PanelChooser();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	//Handles ChooserStateHandler State
	bool IsReadyToClose(std::string id);
	const char* OnChooserClosed();
	void ShowPanel(const char* path = nullptr, const char* extension = nullptr);
	void GetPath(const char* path, const char* extension);
	void OpenPanel(std::string id,const char* extension = nullptr, const char* from_folder = nullptr);
private:
	enum
	{
		CLOSED,
		OPENED,
		READY_TO_CLOSE
	} chooserState = CLOSED;
	std::string fileDialogOrigin;
	Editor* editor = nullptr;
	const char* currentExtension = "fbx";
	char selectedFile[FILE_MAX];
	std::string id = "";
};