#ifndef __PANEL_LOG_H__
#define __PANEL_LOG_H__

#include "Panel.h"

class PanelLog : public Panel
{
public:

	PanelLog();
	~PanelLog();

	bool Update();
};

#endif PANELLOG_H // !__PANEL_LOG_H__