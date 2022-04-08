#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__

#include "Panel.h"

class M_Editor;

class PanelGame : public Panel
{
public:

	PanelGame(M_Editor* editor);
	~PanelGame();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:

	M_Editor* editor = nullptr;
};

#endif // !__PANEL_GAME_H__