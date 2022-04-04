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

	bool CleanUp();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool IsWindowFocused();

	void SetIsFocused(bool isFocused);
	
private:
	bool isFocused = false;
	Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};

#endif // !__PANEL_VIEWPORT_H__