#ifndef __PANEL_CHOOSER_H__
#define __PANEL_CHOOSER_H__

#include "Panel.h"
#include <vector>
#include <string>
#include <tuple>
#include <filesystem>
#include <functional>

#define FILE_MAX 250
class M_Editor;
class R_Texture;
class PanelChooser : public Panel
{
public:
	PanelChooser(M_Editor* editor);
	~PanelChooser();

	bool Start();
	bool Update();

	//Handles ChooserStateHandler State
	bool IsReadyToClose(std::string id);
	std::string OnChooserClosed();
	void Save();
	void DrawOpenPanel(const char* extension = nullptr);
	void GetPath(const char* extension);
	void OpenPanel(std::string id,const char* extension, std::vector<std::string> extensionList,bool isSavePanel=false);
	std::function<void(std::string path)>OnSave = nullptr;

private:
	enum
	{
		CLOSED,
		OPENED,
		READY_TO_CLOSE
	} chooserState = CLOSED;
	M_Editor* editor = nullptr;
	const char* currentExtension = "";
	std::vector<std::string> extensionList;
	std::string selectedFile = "";
	std::string saveFileName;
	std::string id = "";
	bool isSavePanel = false;
	const std::filesystem::path assetsDir = "Assets";
	std::filesystem::path currentDir = "Assets";
	R_Texture* fileTexture;
	R_Texture* directoryTexture;
	std::string fileIconPath, directoryIconPath;
};

#endif // !__PANEL_CHOOSER_H__