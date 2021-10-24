#pragma once
#include "Panel.h"
class Editor;
class FileSystem;
class MainBar : public Panel {
public:
	MainBar(Editor* editor,FileSystem* filesystem);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void ImportModel();
private:
	Editor* editor;
	FileSystem* filesystem;
	bool loadingModel = false;
};