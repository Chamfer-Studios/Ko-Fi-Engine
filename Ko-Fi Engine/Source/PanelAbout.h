#ifndef __PANEL_ABOUT_H__
#define __PANEL_ABOUT_H__

#include "Panel.h"

class Editor;

class PanelAbout : public Panel
{
public:
	PanelAbout(Editor* editor);
	~PanelAbout();

	bool CleanUp();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void ShowAboutWindow(bool* toggleAboutPanel);

private:
	Editor* editor = nullptr;
};

#endif // !__PANEL_ABOUT_H__