#ifndef __PANEL_RUNTIME_STATE_H__
#define __PANEL_RUNTIME_STATE_H__

#include "Panel.h"

class M_Editor;
class KoFiEngine;

class PanelRuntimeState : public Panel
{
public:
	PanelRuntimeState(M_Editor* editor, KoFiEngine* engine);
	~PanelRuntimeState();

	bool Awake();
	bool Update();

	void DrawRuntimePanel();

private:
	M_Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};

#endif // !__PANEL_RUNTIME_STATE_H__