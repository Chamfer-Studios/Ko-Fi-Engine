#ifndef __PANEL_ABOUT_H__
#define __PANEL_ABOUT_H__

#include "Panel.h"

class M_Editor;

class PanelAbout : public Panel
{
public:
	PanelAbout(M_Editor* editor);
	~PanelAbout();

	bool Awake();
	bool Update();

	void ShowAboutWindow(bool* toggleAboutPanel);

private:
	M_Editor* editor = nullptr;
};

#endif // !__PANEL_ABOUT_H__