#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "Editor.h"
#include "Window.h"
#include "Renderer3D.h"

#include "PanelTest.h"
#include "PanelConfiguration.h"
Editor::Editor(Window* window, Renderer3D* renderer)
{
	this->window = window;
	this->renderer = renderer;

	panelTest = new PanelTest();
	panelConfig = new PanelConfiguration();

	AddPanel(panelTest);
	AddPanel(panelConfig);
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

bool Editor::Awake()
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
	bool ret = true;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	styleHandler.SetKoFiStyle();
	ImGui_ImplSDL2_InitForOpenGL(window->window, renderer->context);
	ImGui_ImplOpenGL3_Init();

	//Panels Awake
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

	
	//Panels PreUpdate
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
	//// Window with a button to end the program
	

	//// Window with a button to create another window
	ImGui::Begin("Create window");
	ImGui::Text("Press the button to create another window.");
	if (ImGui::Button("Button"))
	styleHandler.SetKoFiStyle();

	ImGui::End();

	//Panels Update
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

	//Panels PostUpdate
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
	bool ret = true;
	for (std::list<Panel*>::reverse_iterator item = panels.rbegin(); item != panels.rend() && ret == true; ++item)
	{
		ret = (*item)->CleanUp();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}