#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "Panel.h"

class M_Editor;
class M_ResourceManager;

class PanelResources : public Panel
{
public:

	PanelResources(M_Editor* editor);
	~PanelResources();

	inline void SetResourceManager(M_ResourceManager* resourceManager) { this->resourceManager = resourceManager; }

	bool Awake();
	bool Update();

	void ShowResourcesWindow(bool* toggleResourcesPanel);

private:
	M_Editor* editor = nullptr;
	M_ResourceManager* resourceManager = nullptr;
};

#endif __PANEL_RESOURCES_H__ // !__PANEL_RESOURCES_H__