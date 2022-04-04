#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__

#include "Panel.h"

class Editor;

class PanelSettings : public Panel
{
public:
	PanelSettings(Editor* editor);
	~PanelSettings();

	bool CleanUp();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	bool ShowPanel(bool* toggleSettingsPanel);

private:
	Editor* editor = nullptr;
};



#endif // !__PANEL_SETTINGS_H__

