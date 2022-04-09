#ifndef __PANEL_VIEWPORT_H__
#define __PANEL_VIEWPORT_H__

#include "Panel.h"

class M_Editor;
class KoFiEngine;

class PanelViewport : public Panel
{
public:
	PanelViewport(M_Editor* editor, KoFiEngine* engine);
	~PanelViewport();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool IsWindowFocused();

	void SetIsFocused(bool isFocused);
	
private:
	bool isFocused = false;
	M_Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};

#endif // !__PANEL_VIEWPORT_H__