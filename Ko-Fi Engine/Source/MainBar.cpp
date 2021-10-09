#include "MainBar.h"
#include "Editor.h"

#include <imgui.h>
MainBar::MainBar(Editor* editor)
{
	this->editor = editor;
}

MainBar::~MainBar()
{
}

bool MainBar::Awake()
{
	return true;
}

bool MainBar::PreUpdate()
{
	return true;
}

bool MainBar::Update()
{
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				editor->toggleAboutPanel = !editor->toggleAboutPanel;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	return ret;
}

bool MainBar::PostUpdate()
{
	return true;
}
