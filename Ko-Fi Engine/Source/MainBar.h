#ifndef __MAINBAR_H__
#define __MAINBAR_H__

#include "Panel.h"

class Editor;
class Importer;

class MainBar : public Panel
{
public:
	MainBar(Editor* editor);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void ImportModel();

private:
	Editor* editor;
	bool loadingModel = false;
};

#endif // !__MAINBAR_H__