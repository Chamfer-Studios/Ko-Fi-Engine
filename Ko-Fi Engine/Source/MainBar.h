#ifndef __MAINBAR_H__
#define __MAINBAR_H__

#include "Panel.h"

class M_Editor;
class Importer;
class GameObject;

class MainBar : public Panel
{
public:
	MainBar(M_Editor* editor);
	~MainBar();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();

	void ChoosersListener();

private:
	M_Editor* editor;
	bool loadingModel = false;

};

#endif // !__MAINBAR_H__