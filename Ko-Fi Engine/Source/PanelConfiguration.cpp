#include "PanelConfiguration.h"
#include <imgui.h>
PanelConfiguration::PanelConfiguration()
{
	panelName = "Configuration";
	json = jsonHandler.LoadJson("EngineConfig/config.json");
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
		strcpy_s(appName, 120, json.at("Engine").at("Title").dump(4).c_str());
		ImGui::InputText("App Name",appName,120,ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		static char organization[120];
		strcpy_s(organization, 120, json.at("Engine").at("Organization").dump(4).c_str());
		ImGui::InputText("Organization", organization, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		int maxFps = 60;
		ImGui::SliderInt("Max FPS", &maxFps, 0, 120);
	}

	if (ImGui::CollapsingHeader("Window")) {
		bool active = false;
		ImGui::Checkbox("Active",&active);

		ImGui::Text("Icon: default");
		int brightness = 60;
		ImGui::SliderInt("Brightness", &brightness, 0, 120);	
		int width = 60;
		ImGui::SliderInt("Width", &width, 0, 120);
		int height = 60;
		ImGui::SliderInt("Height", &height, 0, 120);
	}
	ImGui::End();
	return true;
}

bool PanelConfiguration::PostUpdate()
{
	return true;
}
