#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "ImGUIHandler.h"
#include "Window.h"
#include "Renderer3D.h"

ImGuiHandler::ImGuiHandler(Window* window, Renderer3D* renderer)
{
	this->window = window;
	this->renderer = renderer;
}

ImGuiHandler::~ImGuiHandler()
{
}

bool ImGuiHandler::Awake()
{
	return true;
}

bool ImGuiHandler::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window->window, renderer->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

bool ImGuiHandler::PreUpdate(float dt)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window->window);
	ImGui::NewFrame();

	return true;
}

bool ImGuiHandler::Update(float dt)
{
	return true;
}

bool ImGuiHandler::PostUpdate(float dt)
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return true;
}

bool ImGuiHandler::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

void ImGuiHandler::CreateWin(SString name, SString text)
{
	ImGui::Begin(name.GetString());
	ImGui::Text(text.GetString());
	ImGui::End();
}

void ImGuiHandler::CreateButton()
{

}