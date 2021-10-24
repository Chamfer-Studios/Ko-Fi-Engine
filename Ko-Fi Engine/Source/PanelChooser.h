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

	bool FileDialog(const char* extension=nullptr, const char* from_folder=nullptr);
	const char* CloseFileDialog();
	void ShowPanel(const char* path = nullptr, const char* extension = nullptr);
	void GetPath(const char* path, const char* extension);

private:
	enum
	{
		closed,
		opened,
		ready_to_close
	} fileDialog = closed;
	std::string fileDialogFilter;
	std::string fileDialogOrigin;
	FileSystem* fileSystem = nullptr;
	const char* current_extension = "fbx";
	char selected_file[FILE_MAX];
};