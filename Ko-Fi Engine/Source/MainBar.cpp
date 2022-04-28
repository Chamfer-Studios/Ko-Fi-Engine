#include "MainBar.h"

// Modules
#include "Engine.h"
#include "M_Editor.h"
#include "M_FileSystem.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"
#include "C_Camera.h"
#include "C_Canvas.h"
#include "C_Transform2D.h"
#include "C_Material.h"
#include "C_Image.h"
#include "C_Button.h"
#include "C_Text.h"
#include "C_LightSource.h"
#include "C_Mesh.h"

#include "PanelChooser.h"
#include "SDL.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include "Primitive.h"
#include "Importer.h"
#include "R_Material.h"

MainBar::MainBar(M_Editor* editor)
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
				Importer::GetInstance()->sceneImporter->Save(editor->engine->GetSceneManager()->GetCurrentScene(), editor->engine->GetSceneManager()->GetCurrentScene()->rootGo->GetName());
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
				editor->engine->GetSceneManager()->GetCurrentScene()->DeleteCurrentScene(editor->engine->GetSceneManager()->GetCurrentScene()->rootGo->GetName());
			
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
				camera->CreateComponent<C_Camera>();
			}
			if (ImGui::BeginMenu("Lights"))
			{
				if (ImGui::MenuItem("Directional Light"))
				{
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("Directional Light");
					go->AddComponentByType(ComponentType::LIGHT_SOURCE);
				}
				if (ImGui::MenuItem("Point Light"))
				{
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("Point Light");
					C_LightSource* cLightSource =  (C_LightSource*)go->AddComponentByType(ComponentType::LIGHT_SOURCE);
					cLightSource->ChangeSourceType(SourceType::POINT);
				}
				if (ImGui::MenuItem("Focal Light"))
				{
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("Focal Light");
					C_LightSource* cLightSource = (C_LightSource*)go->AddComponentByType(ComponentType::LIGHT_SOURCE);
					cLightSource->ChangeSourceType(SourceType::FOCAL);
				}
				ImGui::EndMenu();

			}
			if (ImGui::BeginMenu("Primitive"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE, editor->engine->GetSceneIntro()->gameObjectList);
					//GameObject* obj = nullptr;
					//obj = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject("Cube");
					//obj->AddComponentByType(ComponentType::MATERIAL);
					//obj->AddComponentByType(ComponentType::MESH);
					//R_Mesh* cube = new R_Mesh(Shape::CUBE);
					//obj->GetComponent<C_Mesh>()->SetMesh(cube);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE. editor->engine->GetSceneIntro()->gameObjectList);
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
					go->CreateComponent<C_Canvas>();
				}
				if (ImGui::MenuItem("Image")) {
					//create canvas first 
					bool canvasExists = false;
					GameObject* lastCanvas = nullptr;
					for (GameObject* goit : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
					{
						if (goit->GetComponent<C_Canvas>() != nullptr)
						{
							lastCanvas = goit;
							canvasExists = true;
							break;
						}
					}

					GameObject* newCanvas = nullptr;
					if (canvasExists == false)
					{
						newCanvas = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
						newCanvas->SetName("Canvas");
						newCanvas->CreateComponent<C_Canvas>();
					}

					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Image");
					go->CreateComponent<C_Transform2D>();
					go->CreateComponent<C_Image>();

					if (canvasExists == true && lastCanvas != nullptr)
					{
						lastCanvas->AttachChild(go);
					}
					else if(newCanvas != nullptr)
					{
						newCanvas->AttachChild(go);
					}
					
					//go->CreateComponent<C_Material>();
				}
				if (ImGui::MenuItem("Button")) {

					bool canvasExists = false;
					GameObject* lastCanvas = nullptr;
					for (GameObject* goit : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
					{
						if (goit->GetComponent<C_Canvas>() != nullptr)
						{
							lastCanvas = goit;
							canvasExists = true;
							break;
						}
					}

					GameObject* newCanvas = nullptr;
					if (canvasExists == false)
					{
						newCanvas = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
						newCanvas->SetName("Canvas");
						newCanvas->CreateComponent<C_Canvas>();
					}

					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Button");
					go->CreateComponent<C_Transform2D>();
					go->CreateComponent<C_Button>();
					//go->CreateComponent<C_Material>();

					if (canvasExists == true && lastCanvas != nullptr)
					{
						lastCanvas->AttachChild(go);
					}
					else if (newCanvas != nullptr)
					{
						newCanvas->AttachChild(go);
					}
				}
				if (ImGui::MenuItem("Text")) {

					//create canvas first 
					bool canvasExists = false;
					GameObject* lastCanvas = nullptr;
					for (GameObject* goit : editor->engine->GetSceneManager()->GetCurrentScene()->gameObjectList)
					{
						if (goit->GetComponent<C_Canvas>() != nullptr)
						{
							lastCanvas = goit;
							canvasExists = true;
							break;
						}
					}

					GameObject* newCanvas = nullptr;
					if (canvasExists == false)
					{
						newCanvas = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
						newCanvas->SetName("Canvas");
						newCanvas->CreateComponent<C_Canvas>();
					}

					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
					go->SetName("Text");
					go->CreateComponent<C_Transform2D>();
					go->CreateComponent<C_Text>();

					if (canvasExists == true && lastCanvas != nullptr)
					{
						lastCanvas->AttachChild(go);
					}
					else if (newCanvas != nullptr)
					{
						newCanvas->AttachChild(go);
					}
					//go->CreateComponent<C_Material>();
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
				Importer::GetInstance()->sceneImporter->Load(editor->engine->GetSceneManager()->GetCurrentScene(), editor->engine->GetFileSystem()->GetNameFromPath(file).c_str());
			}
			
		}
	}
}