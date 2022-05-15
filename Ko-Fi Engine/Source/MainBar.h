#ifndef __MAINBAR_H__
#define __MAINBAR_H__

#include "Panel.h"

class M_Editor;
class Importer;
class GameObject;
enum class Shape;

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

	void CreatePrimitive(Shape shape);

	
	bool CloseAppPopup();

private:
	M_Editor* editor;
	bool loadingModel = false;

};

#endif // !__MAINBAR_H__