#pragma once
#include "Panel.h"

class Editor;

class PanelGameObject : public Panel
{
public:
	PanelGameObject(Editor* editor);
	~PanelGameObject();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	// Needed modules
	Editor* editor = nullptr;

	// Bools for the different options
	bool debugOptions = false;
		bool drawVertexNormals = false;
		bool drawFacesNormals = false;
};