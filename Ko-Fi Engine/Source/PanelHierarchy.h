#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include "EditorStyleHandler.h"

class Editor;
class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(Editor* editor);
	~PanelHierarchy();

	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	void DisplayTree(GameObject* go, int flags);
	void DragNDrop(GameObject* go);
	GameObject* GetSelectedGameObject() { return selectedGameObject; };
private:
	EditorStyleHandler styleHandler;

	// Needed modules
	Editor* editor = nullptr;
	GameObject* selectedGameObject = nullptr;
	GameObject* destinationGameObject = nullptr;
	bool is_selected = false;
};

#endif // !__PANEL_HIERARCHY_H__