#ifndef __PANEL_CHOOSER_H__
#define __PANEL_CHOOSER_H__

#include "Panel.h"
#include <vector>
#include <string>

#define FILE_MAX 250
class M_Editor;

class PanelChooser : public Panel
{
public:
	PanelChooser(M_Editor* editor);
	~PanelChooser();

	bool Awake();
	bool Update();

	//Handles ChooserStateHandler State
	bool IsReadyToClose(std::string id);
	const char* OnChooserClosed();
	void ShowPanel(const char* path = nullptr, const char* extension = nullptr);
	void GetPath(const char* path, const char* extension);
	void OpenPanel(std::string id,const char* extension, std::vector<std::string> extensionList, const char* from_folder = nullptr);

private:
	enum
	{
		CLOSED,
		OPENED,
		READY_TO_CLOSE
	} chooserState = CLOSED;
	std::string fileDialogOrigin;
	M_Editor* editor = nullptr;
	const char* currentExtension = "fbx";
	std::vector<std::string> extensionList;
	char selectedFile[FILE_MAX];
	std::string id = "";
};

#endif // !__PANEL_CHOOSER_H__