#ifndef __PANEL_VIEWPORT_H__
#define __PANEL_VIEWPORT_H__

#include "Panel.h"

class Editor;
class KoFiEngine;

class PanelViewport : public Panel
{
public:
	PanelViewport(Editor* editor, KoFiEngine* engine);
	~PanelViewport();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool IsWindowFocused();
private:
	Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
	bool isFocused = false;
};

#endif // !__PANEL_VIEWPORT_H__