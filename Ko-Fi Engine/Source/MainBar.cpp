#include "MainBar.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include "PanelChooser.h"
#include "SDL.h"
#include <imgui.h>
#include "Primitive.h"
#include "Importer.h"
#include "ComponentCamera.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentMaterial.h"
#include "ComponentImage.h"

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
				editor->GetPanelChooser()->OpenPanel("MainBar", "fbx");
			}
			if (ImGui::MenuItem("Clean Models"))
			{
				std::vector<GameObject*> gameObjectList = editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList;
				for (GameObject* gameObject : gameObjectList)
				{
					RELEASE(gameObject);
				}
				editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList.clear();
				editor->panelGameObjectInfo.selectedGameObjectID = -1;
				editor->engine->GetSceneManager()->GetCurrentScene()->rootGo = new GameObject(-1, editor->engine, "Root");
				editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList.push_back(editor->engine->GetSceneManager()->GetCurrentScene()->rootGo);
			}
			if (ImGui::MenuItem("Quit"))
			{
				ret = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Create Game Object"))
			{

				editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();

			}
			if (ImGui::MenuItem("Create Camera"))
			{

				GameObject* camera = Importer::GetInstance()->ImportModel("Assets/Models/camera.fbx");
				camera->CreateComponent<ComponentCamera>();

			}
			if (ImGui::BeginMenu("Primitive"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CYLINDER, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Line"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_LINE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Plane"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_PLANE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Pyramid"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_PYRAMID, editor->engine->GetSceneIntro()->gameObjectList);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("UI"))
			{
				if (ImGui::MenuItem("Canvas")) {
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
					go->SetName("Canvas");
					go->CreateComponent<ComponentCanvas>();
				}
				if (ImGui::MenuItem("Image")) {
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject();
					go->SetName("Image");
					go->CreateComponent<ComponentTransform2D>();
					go->CreateComponent<ComponentMaterial>();
					go->CreateComponent<ComponentImage>();
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


	if (editor->GetPanelChooser()->IsReadyToClose("MainBar"))
	{
		const char* file = editor->GetPanelChooser()->OnChooserClosed();
		if (file != nullptr)
		{
			Importer::GetInstance()->ImportModel(file);
			//editor->engine->GetFileSystem()->GameObjectFromMesh(newFile.c_str(), editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList);
		}
	}
}
