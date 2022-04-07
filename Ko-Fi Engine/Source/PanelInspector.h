#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

class M_Editor;

class PanelInspector : public Panel
{
public:
	PanelInspector(M_Editor* editor);
	~PanelInspector();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	// Needed modules
	M_Editor* editor = nullptr;
	int componentType;

	// Bools for the different options
	bool debugOptions = false;
	bool drawVertexNormals = false;
	bool drawFacesNormals = false;


};

#endif // !__PANEL_INSPECTOR_H__
