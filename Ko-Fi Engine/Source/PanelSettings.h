#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__

#include "Panel.h"

class M_Editor;

class PanelSettings : public Panel
{
public:
	PanelSettings(M_Editor* editor);
	~PanelSettings();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	bool ShowPanel(bool* toggleSettingsPanel);

private:
	M_Editor* editor = nullptr;
};



#endif // !__PANEL_SETTINGS_H__

