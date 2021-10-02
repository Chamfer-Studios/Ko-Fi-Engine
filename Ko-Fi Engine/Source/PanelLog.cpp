#include "PanelLog.h"
#include "ImGuiAppLog.h"

PanelLog::PanelLog()
{
}

PanelLog::~PanelLog()
{
}

bool PanelLog::Update()
{
	appLog->Draw("Log");

	return true;
}