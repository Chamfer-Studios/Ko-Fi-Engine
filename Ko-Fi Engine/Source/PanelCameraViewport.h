#ifndef __PANEL_CAMERA_VIEWPORT_H__
#define __PANEL_CAMERA_VIEWPORT_H__

#include "Panel.h"

class M_Editor;
class KoFiEngine;

class PanelCameraViewport : public Panel
{
public:
	PanelCameraViewport(M_Editor* editor, KoFiEngine* engine);
	~PanelCameraViewport();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool RenderPanel(bool* showPanel);

private:

	M_Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};

#endif // !__PANEL_CAMERA_VIEWPORT_H__