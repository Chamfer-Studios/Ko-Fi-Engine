#include "MainBar.h"
#include "Editor.h"
#include "FileSystem.h"
#include "PanelChooser.h"
#include "SDL.h"
#include <imgui.h>
#include "Primitive.h"

MainBar::MainBar(Editor* editor,FileSystem* filesystem)
{
	this->editor = editor;
	this->filesystem = filesystem;
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
	ImportModel();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Model"))
			{
				loadingModel = true;
			}
			if (ImGui::MenuItem("Clean Models"))
			{
				editor->gameObjects.clear();
				editor->panelGameObjectInfo.currentGameObjectID = -1;
			}
			if (ImGui::MenuItem("Quit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::BeginMenu("Primitive"))
			{
				if (ImGui::MenuItem("Sphere"))
				{
					
				}
				if (ImGui::MenuItem("Cube"))
				{
					
				}
				if (ImGui::MenuItem("Plane"))
				{
					
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
				editor->toggleAboutPanel = !editor->toggleAboutPanel;

			if (ImGui::MenuItem("Repository"))
				ShellExecuteA(NULL, "open", "https://github.com/boscobarberesbert/Ko-Fi-Engine", NULL, NULL, SW_SHOWNORMAL);

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

void MainBar::ImportModel() {
	if (loadingModel == true && editor->GetPanelChooser()->FileDialog("fbx"))
	{
		const char* file = editor->GetPanelChooser()->CloseFileDialog();
		if (file != nullptr)
		{
			std::string newFile = file;
			newFile.erase(newFile.begin());
			filesystem->GameObjectFromMesh(newFile.c_str(), editor->gameObjects);
		}
		loadingModel = false;
	}
}
