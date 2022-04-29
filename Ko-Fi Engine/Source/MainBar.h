#ifndef __MAINBAR_H__
#define __MAINBAR_H__

#include "Panel.h"

class M_Editor;
class Importer;
class GameObject;
enum class Shape;

class MainBar : public Panel
{
public:
	MainBar(M_Editor* editor);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void ChoosersListener();

	void CreatePrimitive(Shape shape);

private:
	M_Editor* editor;
	bool loadingModel = false;
	bool openSaveAsPopup = false; //This is a temporal solution since imgui doesnt allow to call openpopup from MenuItem() yet. However we will use the panelchooser to do this task soon so just temp code.
	std::string saveAsSceneName = "Scene Name";
};

#endif // !__MAINBAR_H__