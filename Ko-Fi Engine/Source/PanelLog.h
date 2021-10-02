#pragma once
#ifndef PANELLOG_H
#define PANELLOG_H
#include "Panel.h"

class PanelLog : public Panel
{
public:

	PanelLog();
	~PanelLog();

	bool Update();
};

#endif PANELLOG_H // PANELLOG_H