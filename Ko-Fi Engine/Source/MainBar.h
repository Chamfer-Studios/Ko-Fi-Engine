#pragma once
#include "Panel.h"
class Editor;
class MainBar : public Panel {
public:
	MainBar(Editor* editor);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
private:
	Editor* editor;
};