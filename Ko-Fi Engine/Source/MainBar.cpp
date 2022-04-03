#include "MainBar.h"
#include "Editor.h"
#include "Engine.h"
#include "FileSystem.h"
#include "SceneManager.h"
#include "PanelChooser.h"
#include "SDL.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "Primitive.h"
#include "Importer.h"
#include "ComponentCamera.h"
#include "ComponentCanvas.h"
#include "ComponentTransform2D.h"
#include "ComponentMaterial.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "ComponentText.h"
#include "ComponentCamera.h"
#include "Material.h"
#include "ComponentMesh.h"

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
	ChoosersListener();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Import Model"))
			{
				editor->GetPanelChooser()->OpenPanel("MainBar", "fbx", { "fbx","dae","obj","stl","gltf" });
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				saveAsSceneName = editor->engine->GetSceneManager()->GetCurrentScene()->name.c_str();
				Importer::GetInstance()->sceneImporter->Save(editor->engine->GetSceneManager()->GetCurrentScene(), saveAsSceneName.c_str());
			}
			if (ImGui::MenuItem("Save Scene As"))
			{
				saveAsSceneName = editor->engine->GetSceneManager()->GetCurrentScene()->name.c_str();
				openSaveAsPopup = true;
			}
			if (ImGui::MenuItem("Load Scene"))
			{
				editor->GetPanelChooser()->OpenPanel("LoadScene", "json", { "json" });
			}
			if (ImGui::MenuItem("Settings"))
			{
				editor->toggleSettingsPanel = true;
			}
			if (ImGui::MenuItem("Clean Models"))
			{
				editor->engine->GetSceneManager()->GetCurrentScene()->DeleteCurrentScene();
			
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
				GameObject* camera = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("camera");
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
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr,nullptr, false);
					go->SetName("Canvas");
					go->CreateComponent<ComponentCanvas>();
				}
				if (ImGui::MenuItem("Image")) {
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Image");
					go->CreateComponent<ComponentTransform2D>();
					go->CreateComponent<ComponentImage>();
					//go->CreateComponent<ComponentMaterial>();
				}
				if (ImGui::MenuItem("Button")) {
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Button");
					go->CreateComponent<ComponentTransform2D>();
					go->CreateComponent<ComponentButton>();
					//go->CreateComponent<ComponentMaterial>();
				}
				if (ImGui::MenuItem("Text")) {
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Text");
					go->CreateComponent<ComponentTransform2D>();
					go->CreateComponent<ComponentText>();
					//go->CreateComponent<ComponentMaterial>();
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
		if (openSaveAsPopup)
		{
			ImGui::OpenPopup("SaveSceneAsPopup");
		}
		if (ImGui::BeginPopupModal("SaveSceneAsPopup",&openSaveAsPopup))
		{
			
			ImGui::InputText("Scene Name", &saveAsSceneName);
			if (ImGui::Button("Save##") && !saveAsSceneName.empty()) {
				Importer::GetInstance()->sceneImporter->Save(editor->engine->GetSceneManager()->GetCurrentScene(), saveAsSceneName.c_str());
				openSaveAsPopup = false;
			}
			ImGui::EndPopup();
		}
	}

	return ret;
}

bool MainBar::PostUpdate()
{
	return true;
}

void MainBar::ChoosersListener()
{
	if (editor->GetPanelChooser()->IsReadyToClose("MainBar"))
	{
		const char* file = editor->GetPanelChooser()->OnChooserClosed();
		if (file != nullptr)
		{
			Importer::GetInstance()->sceneImporter->Import(file);
		}
	}
	if (editor->GetPanelChooser()->IsReadyToClose("LoadScene"))
	{
		const char* file = editor->GetPanelChooser()->OnChooserClosed();
		if (file != nullptr)
		{
	#pragma omp parallel private()
			{
				Importer::GetInstance()->sceneImporter->Load(editor->engine->GetSceneManager()->GetCurrentScene(), Importer::GetInstance()->GetNameFromPath(file).c_str());
			}
			
		}
	}
}

void MainBar::ThreadLoadScene()
{
}
