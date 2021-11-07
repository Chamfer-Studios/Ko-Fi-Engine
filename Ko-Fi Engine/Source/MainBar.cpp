#include "MainBar.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "SceneIntro.h"
#include "PanelChooser.h"
#include "SDL.h"
#include <imgui.h>
#include "Primitive.h"

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
	ImportModel();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Model"))
			{
				editor->GetPanelChooser()->OpenPanel("fbx");
			}
			if (ImGui::MenuItem("Clean Models"))
			{
				editor->engine->GetSceneIntro()->gameObjectList.clear();
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
				if (ImGui::MenuItem("Cube"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CYLINDER, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Line"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_LINE, editor->engine->GetSceneIntro()->gameObjectList);

				}
				if (ImGui::MenuItem("Plane"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_PLANE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Pyramid"))
				{
					editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_PYRAMID, editor->engine->GetSceneIntro()->gameObjectList);
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


	if (editor->GetPanelChooser()->IsReadyToClose())
	{
		const char* file = editor->GetPanelChooser()->OnChooserClosed();
		if (file != nullptr)
		{
			std::string newFile = file;
			newFile.erase(newFile.begin());
			editor->engine->GetFileSystem()->GameObjectFromMesh(newFile.c_str(), editor->engine->GetSceneIntro()->gameObjectList);
		}
	}
}
