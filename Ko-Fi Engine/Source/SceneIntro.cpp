#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "SceneIntro.h"
#include "Log.h"
#include "Camera3D.h"
#include "Renderer3D.h"
#include "Window.h"
#include "Primitive.h"


SceneIntro::SceneIntro(Camera3D* camera,Window* window,Renderer3D* renderer) : Module()
{
	this->camera = camera;
	this->window = window;
	this->renderer = renderer;
	check = true;
	random = 0;

}

SceneIntro::~SceneIntro()
{}

// Load assets
bool SceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	camera->Move(vec3(1.0f, 1.0f, 0.0f));
	camera->LookAt(vec3(0, 0, 0));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(window->window,renderer->context);
	ImGui_ImplOpenGL3_Init();
	return ret;
}

// Load assets
bool SceneIntro::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	LOG("Unloading Intro scene");

	return true;
}

// Update
bool SceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	LCG randomGenerator;
	if (check) {
		random = randomGenerator.Int(5, 10);
		check = false;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window->window);
	ImGui::NewFrame();

	ImGui::Begin("Test Window");

	ImGui::Text("Random Number: %d",random);

	ImGui::Checkbox("Ray: ", &check);
	ImGui::End();
	ImGui::Begin("Test 2");
	ImGui::Text("Bosco madafaka");
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	return true;
}

void SceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}

