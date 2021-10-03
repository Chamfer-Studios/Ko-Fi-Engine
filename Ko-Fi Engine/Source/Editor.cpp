#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Editor.h"
#include "Window.h"
#include "Renderer3D.h"
#include "Input.h"
#include "EngineConfig.h"
#include "PanelTest.h"
#include "PanelConfiguration.h"
#include "PanelLog.h"
#include "ImGuiAppLog.h"

Editor::Editor(Window* window, Renderer3D* renderer,Input* input, EngineConfig* engineConfig)
{
	name = "Editor";
	this->window = window;
	this->renderer = renderer;
	this->engine = engine;

	panelTest = new PanelTest();
	panelConfig = new PanelConfiguration(window,renderer,input,engineConfig,this);
	panelLog = new PanelLog();

	AddPanel(panelTest);
	AddPanel(panelConfig);
	AddPanel(panelLog);
}

Editor::~Editor()
{
	for (std::list<Panel*>::reverse_iterator item = panels.rbegin(); item != panels.rend(); ++item)
	{
		RELEASE(*item);
	}
	panels.clear();
}

void Editor::AddPanel(Panel* panel)
{
	panel->Init();
	panels.push_back(panel);
}

bool Editor::Awake(Json configModule)
{
	bool ret = true;
	//Panels Awake
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Awake();
			item++;
		}
	}

	return ret;
}

bool Editor::Start()
{
	appLog->AddLog("Starting panel editor\n");
	bool ret = true;

	// Initializing ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	styleHandler.SetKoFiStyle();
	ImGui_ImplSDL2_InitForOpenGL(window->window, renderer->context);
	ImGui_ImplOpenGL3_Init();

	// Panels Awake
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Start();
			item++;
		}
	}

	return ret;
}

bool Editor::PreUpdate(float dt)
{
	bool ret = true;
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window->window);
	ImGui::NewFrame();

	// Panels PreUpdate
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->PreUpdate();
			item++;
		}
	}

	return true;
}

bool Editor::Update(float dt)
{
	bool ret = true;
	//Creating Main Menu Bar
	CallMainMenuBar();

	// Window with a button to create another window
	ImGui::Begin("Create window");
	ImGui::Text("Press the button to create another window.");
	if (ImGui::Button("Button"))
	styleHandler.SetKoFiStyle();
	ImGui::End();

	// Panels Update
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->Update();
			item++;
		}
	}

	return ret;
}

bool Editor::PostUpdate(float dt)
{
	bool ret = true;
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Panels PostUpdate
	if (ret == true)
	{
		std::list<Panel*>::iterator item = panels.begin();;

		while (item != panels.end() && ret)
		{
			ret = (*item)->PostUpdate();
			item++;
		}
	}
	return true;
}

bool Editor::CleanUp()
{
	appLog->AddLog("Cleaning panel editor\n");
	bool ret = true;

	// Cleaning panels
	for (std::list<Panel*>::reverse_iterator item = panels.rbegin(); item != panels.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

bool Editor::CallMainMenuBar() {
	bool ret = true;
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("Info")) {
			if (ImGui::MenuItem("Repository")) {
				ShellExecute(NULL, "open", "https://github.com/boscobarberesbert/Ko-Fi-Engine", NULL, NULL, SW_SHOWNORMAL);
			}
			if (ImGui::MenuItem("About")) {
				ShellExecute(NULL, "open", "https://github.com/boscobarberesbert/", NULL, NULL, SW_SHOWNORMAL);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	return ret;
}

