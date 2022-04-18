#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
#include "EditorStyleHandler.h"

class M_Editor;
class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy(M_Editor* editor);
	~PanelHierarchy();

	bool Awake();
	bool Update();
	bool CleanUp();
	void DisplayTree(GameObject* go, int flags, int& id);
	void DragNDrop(GameObject* go);
	GameObject* GetSelectedGameObject() { return selectedGameObject; };
	void ReorderElement(std::vector<GameObject*>& list, int insertIndex, int moveIndex);
private:
	EditorStyleHandler styleHandler;

	// Needed modules
	M_Editor* editor = nullptr;
	GameObject* selectedGameObject = nullptr;
	GameObject* destinationGameObject = nullptr;
	bool is_selected = false;
	bool dragging = false;
};

#endif // !__PANEL_HIERARCHY_H__