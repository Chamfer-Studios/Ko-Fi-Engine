#pragma once
#include "Panel.h"

class FileSystem;
#define FILE_MAX 250
class PanelChooser : public Panel
{
public:

	PanelChooser(FileSystem* fileSystem);
	~PanelChooser();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	//Handles ChooserStateHandler State
	bool IsReadyToClose();
	const char* OnChooserClosed();
	void ShowPanel(const char* path = nullptr, const char* extension = nullptr);
	void GetPath(const char* path, const char* extension);
	void OpenPanel(const char* extension = nullptr, const char* from_folder = nullptr);
private:
	enum
	{
		CLOSED,
		OPENED,
		READY_TO_CLOSE
	} chooserState = CLOSED;
	std::string fileDialogOrigin;
	FileSystem* fileSystem = nullptr;
	const char* currentExtension = "fbx";
	char selectedFile[FILE_MAX];
};