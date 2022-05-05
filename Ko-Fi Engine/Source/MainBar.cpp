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

// Resources
#include "R_Material.h"
#include "R_Mesh.h"

#include "PanelChooser.h"
#include "Primitive.h"
#include "Importer.h"
#include "SDL.h"
#include <imgui.h>
#include <imgui_stdlib.h>

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
				editor->GetPanelChooser()->OpenPanel("MainBar", "fbx", { "fbx" });
			}
			if (ImGui::MenuItem("Save Scene"))
			{
				Importer::GetInstance()->sceneImporter->SaveScene(editor->engine->GetSceneManager()->GetCurrentScene(), editor->engine->GetSceneManager()->GetCurrentScene()->rootGo->GetName());
			}
			if (ImGui::MenuItem("Save Scene As"))
			{
				editor->GetPanelChooser()->OpenPanel("SaveSceneAs", "json", { "json" }, true);
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
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Save Window Layout"))
			{
				editor->GetPanelChooser()->OpenPanel("SaveLayout", "ini", { "ini" }, true);
			}
			if (ImGui::MenuItem("Load Window Layout"))
			{
				editor->GetPanelChooser()->OpenPanel("LoadLayout", "ini", { "ini" }, true);
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
					C_LightSource* cLightSource = (C_LightSource*)go->AddComponentByType(ComponentType::LIGHT_SOURCE);
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
					CreatePrimitive(Shape::CUBE);
				}
				if (ImGui::MenuItem("Sphere"))
				{
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE. editor->engine->GetSceneIntro()->gameObjectList);
					CreatePrimitive(Shape::SPHERE);
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					CreatePrimitive(Shape::CYLINDER);
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_CYLINDER, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Line"))
				{
					
					//editor->engine->GetFileSystem()->GameObjectFromPrimitive(COMPONENT_SUBTYPE::COMPONENT_MESH_LINE, editor->engine->GetSceneIntro()->gameObjectList);
				}
				if (ImGui::MenuItem("Plane"))
				{
					CreatePrimitive(Shape::PLANE);
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
					GameObject* go = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(nullptr, nullptr, false);
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
					else if (newCanvas != nullptr)
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
	}

	return ret;
}

bool MainBar::PostUpdate()
{
	return true;
}

void MainBar::ChoosersListener()
{
	// TODO
	if (editor->GetPanelChooser()->IsReadyToClose("MainBar"))
	{
		std::string file = editor->GetPanelChooser()->OnChooserClosed();
		if (!file.empty())
		{
			Importer::GetInstance()->sceneImporter->LoadScene(editor->engine->GetSceneManager()->GetCurrentScene(), file.c_str());
		}
	}
	if (editor->GetPanelChooser()->IsReadyToClose("LoadScene"))
	{
		std::string file = editor->GetPanelChooser()->OnChooserClosed();
		if (!file.empty())
		{
#pragma omp parallel private()
			{
				Importer::GetInstance()->sceneImporter->LoadScene(editor->engine->GetSceneManager()->GetCurrentScene(), editor->engine->GetFileSystem()->GetNameFromPath(file).c_str());
			}

		}
	}
	if (editor->GetPanelChooser()->IsReadyToClose("SaveLayout"))
	{
		editor->GetPanelChooser()->OnSave = [&](std::string path) {
			editor->iniToSave = path;
		};
		editor->GetPanelChooser()->Save();

	}	
	if (editor->GetPanelChooser()->IsReadyToClose("LoadLayout"))
	{
		std::string file = editor->GetPanelChooser()->OnChooserClosed();
		if (!file.empty())
		{
			editor->iniToLoad = file;
		}

	}
	if (editor->GetPanelChooser()->IsReadyToClose("SaveSceneAs"))
	{
		editor->GetPanelChooser()->OnSave = [&](std::string path) {
			auto pos1 = path.find_last_of("/");
			auto pos2 = path.find_last_of(".");
			std::string sceneName = path.substr(pos1+1, pos2-pos1-1);
			Importer::GetInstance()->sceneImporter->SaveScene(editor->engine->GetSceneManager()->GetCurrentScene(), sceneName.c_str());
		};
		editor->GetPanelChooser()->Save();
	}
}

void MainBar::CreatePrimitive(Shape shape)
{
	std::string name;
	switch (shape)
	{
	case Shape::CUBE:
		name = "Cube";
		break;
	case Shape::SPHERE:
		name = "Sphere";
		break;
	case Shape::CYLINDER:
		name = "Cylinder";
		break;
	case Shape::TORUS:
		name = "Torus";
		break;
	case Shape::PLANE:
		name = "Plane";
		break;
	case Shape::CONE:
		name = "Cone";
		break;
	};

	GameObject* c = editor->engine->GetSceneManager()->GetCurrentScene()->CreateEmptyGameObject(name.c_str());
	
	// Mesh
	R_Mesh* rMesh = new R_Mesh(shape);
	rMesh->SetUpMeshBuffers();
	C_Mesh* m = (C_Mesh*)c->AddComponentByType(ComponentType::MESH);
	m->SetMesh(rMesh);


	// Material
	C_Material *mat = (C_Material*)c->AddComponentByType(ComponentType::MATERIAL);
	R_Material *material = new R_Material();
	Importer::GetInstance()->materialImporter->LoadAndCreateShader(material->GetShaderPath(), material);
	mat->SetMaterial(material);
}
