#ifndef __PANEL_GAME_H__
#define __PANEL_GAME_H__

#include "Panel.h"

class Editor;

class PanelGame : public Panel
{
public:

	PanelGame(Editor* editor);
	~PanelGame();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:

	Editor* editor = nullptr;
};

#endif // !__PANEL_GAME_H__