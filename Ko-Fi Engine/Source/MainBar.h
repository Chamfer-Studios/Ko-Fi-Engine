#ifndef __MAINBAR_H__
#define __MAINBAR_H__

#include "Panel.h"

class Editor;
class Importer;
class GameObject;

class MainBar : public Panel
{
public:
	MainBar(Editor* editor);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	void ThreadLoadScene();
	void SafeUIPlacing(GameObject* go);

	void ChoosersListener();

private:
	Editor* editor;
	bool loadingModel = false;
	bool openSaveAsPopup = false; //This is a temporal solution since imgui doesnt allow to call openpopup from MenuItem() yet. However we will use the panelchooser to do this task soon so just temp code.
	std::string saveAsSceneName = "Scene Name";
};

#endif // !__MAINBAR_H__