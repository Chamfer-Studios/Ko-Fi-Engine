#include "UI.h"
#include "glew.h"
#include "MathGeoLib/Math/float2.h"

#include "Engine.h"
#include "GameObject.h"
#include "Editor.h"
#include "ImGuiAppLog.h"
#include "Camera3D.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Window.h"
#include "FileSystem.h"

#include <string>

#include "ComponentMesh.h"
#include "ComponentTransform2D.h"

#include "SDL.h"

#include <queue>

UI::UI(KoFiEngine* engine) : Module()
{
	name = "UI";
	this->engine = engine;
	Uint32 flags = SDL_RENDERER_ACCELERATED;
	renderer = SDL_CreateRenderer(engine->GetWindow()->window, -1, flags);
	TTF_Init();
	rubik = TTF_OpenFont("Assets/Fonts/Rubik_Mono_One/RubikMonoOne-Regular.ttf", 60);
}

UI::~UI()
{

}

bool UI::Start()
{



	return true;
}

bool UI::PreUpdate(float dt)
{
	return true;
}

bool UI::Update(float dt) {

	return true;
}

bool UI::PostUpdate(float dt)
{

	return true;
}

bool UI::CleanUp()
{
	TTF_CloseFont(rubik);
	return true;
}

float2 UI::GetUINormalizedMousePosition()
{
	float mouseX = engine->GetEditor()->mouseScenePosition.x;
	float mouseY = engine->GetEditor()->mouseScenePosition.y;

	return { mouseX, mouseY };
}

void UI::OnGui()
{
}

/*void ModuleUI::OnLoad(const JSONReader& reader)
{
}

void ModuleUI::OnSave(JSONWriter& writer) const
{
}*/