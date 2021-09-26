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
	styleHandler.SetKoFiStyle();
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
	// Window with a button to end the program
	ImGui::Begin("End the program");
	ImGui::Text("Press the button to end the program.");
	if (ImGui::Button("Button") == true) exit(0);
	ImGui::End();

	// Window with a button to create another window
	ImGui::Begin("Create window");
	ImGui::Text("Press the button to create another window.");
	if (!newWindow && ImGui::Button("Button") == true) newWindow = true;
	if (newWindow)
	{
		ImGui::Begin("New window");
		ImGui::Text("A new window was created.");
		ImGui::End();
	}
	ImGui::End();

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

void ImGuiHandler::CreateWin(SString name, SString text, float width, float height)
{
	static bool check = true;
	static float slider = 0.0f;

	ImGui::Begin(name.GetString());
	//ImGui::SetWindowSize(ImVec2(width, height));
	ImGui::Text(text.GetString());
	if (ImGui::Button("Button")) buttonPressed = true;
	else buttonPressed = false;
	ImGui::Checkbox("Checkbox",&check);
	ImGui::SliderFloat("Slider",&slider,0.0f,10.0f);
	ImGui::End();

	ImGui::Begin("Triangle Position/Color");
	static float rotation = 0.0;
	ImGui::SliderFloat("rotation", &rotation, 0, 2);
	static float translation[] = { 0.0, 0.0 };
	ImGui::SliderFloat2("position", translation, -1.0, 1.0);
	static float color[4] = { 1.0f,1.0f,1.0f,1.0f };
	// color picker
	ImGui::ColorEdit3("color", color);
	ImGui::End();
}

void ImGuiHandler::CreateButton()
{

}