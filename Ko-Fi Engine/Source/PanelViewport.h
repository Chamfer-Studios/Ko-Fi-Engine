#ifndef __PANEL_VIEWPORT_H__
#define __PANEL_VIEWPORT_H__

#include "Panel.h"

class M_Editor;
class KoFiEngine;
class R_Texture;
class PanelViewport : public Panel
{
public:
	PanelViewport(M_Editor* editor, KoFiEngine* engine);
	~PanelViewport();

	bool Start();
	bool Update();
	bool IsWindowFocused();

	void SetIsFocused(bool isFocused);
	void DrawViewportBar();
private:
	bool isFocused = false;
	M_Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
	R_Texture* speedCameraIcon = nullptr;
	R_Texture* litIcon = nullptr;
	R_Texture* gizmoMoveIcon = nullptr;
	R_Texture* gizmoRotateIcon = nullptr;
	R_Texture* gizmoScaleIcon = nullptr;

	bool dragDropPopup = false;
};

#endif // !__PANEL_VIEWPORT_H__