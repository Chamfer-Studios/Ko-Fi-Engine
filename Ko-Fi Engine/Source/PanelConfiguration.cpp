#include "PanelConfiguration.h"
#include "Window.h"
#include "Renderer3D.h"
#include "EngineConfig.h"
#include <imgui.h>
PanelConfiguration::PanelConfiguration(Window* window, Renderer3D* renderer,EngineConfig* engineConfig)
{
	panelName = "Configuration";
	this->window = window;
	this->renderer = renderer;
	this->engineConfig = engineConfig;
}

PanelConfiguration::~PanelConfiguration()
{
}

bool PanelConfiguration::Awake()
{
	return true;
}

bool PanelConfiguration::PreUpdate()
{
	return true;
}

bool PanelConfiguration::Update()
{
	ImGui::Begin(panelName.c_str());

		if (ImGui::BeginMenu("Options")) {
			if (ImGui::MenuItem("Set Defaults")) {
				printf_s("%s", "Clicked Set Defaults\n");

			}
			if (ImGui::MenuItem("Load"))
			{
				printf_s("%s", "Clicked Load\n");
			}
			if (ImGui::MenuItem("Save"))
			{
				printf_s("%s", "Clicked Save\n");
			}
			ImGui::EndMenu();
		}

	if (ImGui::CollapsingHeader("Application")) {
		static char appName[120];
		strcpy_s(appName, 120, engineConfig->title.GetString());
		ImGui::InputText("App Name",appName,120,ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		static char organization[120];
		strcpy_s(organization, 120, engineConfig->organization.GetString());
		ImGui::InputText("Organization", organization, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		
		if (ImGui::SliderInt("Max FPS", &engineConfig->maxFps, 0, 120)) {
			engineConfig->cappedMs = 1000 / engineConfig->maxFps;
		}

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", engineConfig->fpsLog[engineConfig->fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &engineConfig->fpsLog[0], engineConfig->fpsLog.size(), 0, title, 0.0f, 100, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", engineConfig->msLog[engineConfig->msLog.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &engineConfig->msLog[0], engineConfig->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Window")) {
		bool active = false;
		ImGui::Checkbox("Active",&active);

		ImGui::Text("Icon: default");
		float brightness = window->GetBrightness();
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
		{
			window->AdjustBrightness(brightness);
		}
		int width = window->GetWidth();
		int height = window->GetHeight();
		if (ImGui::SliderInt("Width", &width, 640, 4096) | ImGui::SliderInt("Height", &height, 480, 2160)) {
			SDL_SetWindowSize(window->window, width, height);
			window->SetWidth(width);
			window->SetHeight(height);
		}
		
	}
	ImGui::End();
	return true;
}

bool PanelConfiguration::PostUpdate()
{
	return true;
}
