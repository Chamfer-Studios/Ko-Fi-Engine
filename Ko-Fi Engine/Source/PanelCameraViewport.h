#ifndef __PANEL_CAMERA_VIEWPORT_H__
#define __PANEL_CAMERA_VIEWPORT_H__

#include "Panel.h"

class Editor;
class KoFiEngine;

class PanelCameraViewport : public Panel
{
public:
	PanelCameraViewport(Editor* editor, KoFiEngine* engine);
	~PanelCameraViewport();

	bool CleanUp();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool RenderPanel(bool* showPanel);

private:

	Editor* editor = nullptr;
	KoFiEngine* engine = nullptr;
};

#endif // !__PANEL_CAMERA_VIEWPORT_H__