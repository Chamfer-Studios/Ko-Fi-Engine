#include "PanelConfiguration.h"
#include "Window.h"
#include "Renderer3D.h"
#include "Input.h"
#include "EngineConfig.h"
#include "Editor.h"
#include <imgui.h>
#include "glew.h"

PanelConfiguration::PanelConfiguration(Window* window, Renderer3D* renderer, Input* input, EngineConfig* engineConfig, Editor* editor)
{
	panelName = "Configuration";
	this->window = window;
	this->renderer = renderer;
	this->engineConfig = engineConfig;
	this->editor = editor;
	this->input = input;
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

	if (ImGui::BeginMenu("Options"))
	{
		if (ImGui::MenuItem("Set Defaults"))
		{
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

	if (ImGui::CollapsingHeader("Application"))
	{
		static char appName[120];
		strcpy_s(appName, 120, engineConfig->title.GetString());
		ImGui::InputText("App Name",appName,120,ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		static char organization[120];
		strcpy_s(organization, 120, engineConfig->organization.GetString());
		ImGui::InputText("Organization", organization, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::SliderInt("Max FPS", &engineConfig->maxFps, 0, 120))
			engineConfig->cappedMs = 1000 / engineConfig->maxFps;

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", engineConfig->fpsLog[engineConfig->fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &engineConfig->fpsLog[0], engineConfig->fpsLog.size(), 0, title, 0.0f, 100, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", engineConfig->msLog[engineConfig->msLog.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &engineConfig->msLog[0], engineConfig->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Renderer"))
	{
		bool vsync = renderer->GetVsync();
		if (ImGui::Checkbox("VSync", &vsync))
			renderer->SetVsync(vsync);
	}

	if (ImGui::CollapsingHeader("Input")) {
		int mouseX = input->GetMouseX();
		int mouseY = input->GetMouseY();
		ImGui::Text("Mouse Position:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i,%i", mouseX, mouseY);

		mouseX = input->GetMouseXMotion();
		mouseY = input->GetMouseYMotion();
		ImGui::Text("Mouse Motion:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i,%i", mouseX, mouseY);

		int wheel = input->GetMouseZ();
		ImGui::Text("Mouse Wheel:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i", wheel);
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Icon:");
		ImGui::SameLine();
		if (ImGui::Selectable(window->GetIcon()))
			loadingIcon = true;
		float brightness = window->GetBrightness();
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
			window->AdjustBrightness(brightness);
		int width = window->GetWidth();
		int height = window->GetHeight();
		if (ImGui::SliderInt("Width", &width, 640, 4096) | ImGui::SliderInt("Height", &height, 480, 2160))
		{
			SDL_SetWindowSize(window->window, width, height);
			window->SetWidth(width);
			window->SetHeight(height);
		}
		ImGui::Text("Refresh rate:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0),"%u",window->GetRefreshRate());
		bool temp = window->GetFullscreen();
		if (ImGui::Checkbox("Fullscreen",&temp))
			window->SetFullscreen(temp);
		ImGui::SameLine();
		temp = window->GetFullscreenDesktop();
		if (ImGui::Checkbox("Fullscreen Desktop", &temp))
			window->SetFullscreenDesktop(temp);
		temp = window->GetResizable();
		if (ImGui::Checkbox("Resizable", &temp))
			window->SetResizable(temp);
		ImGui::SameLine();
		temp = window->GetBorderless();
		if (ImGui::Checkbox("Borderless", &temp))
			window->SetBorderless(temp);
	}

	if (ImGui::CollapsingHeader("Hardware")) {
		ImGui::Text("SDL Version:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%d.%d.%d", engineConfig->sdlVersion.major, engineConfig->sdlVersion.minor, engineConfig->sdlVersion.patch);
		ImGui::Separator();
		ImGui::Text("CPUs:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0),"%d", engineConfig->cpuCores);
		ImGui::Text("System RAM:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%d", engineConfig->RAM);
		ImGui::Text("Caps:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%s%s%s%s%s%s", 
			engineConfig->hasAVX ?"AVX,":"",
			engineConfig->hasAVX2 ?"AVX2,":"",
			engineConfig->hasAltiVec ?"AltiVec,":"",
			engineConfig->hasMMX ?"MMX,":"",
			engineConfig->hasRDTSC ?"RDTSC,":"",
			engineConfig->has3DNow ? "3DNow," : "");
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%s%s%s%s%s",
			engineConfig->hasSSE ? "SSE," : "",
			engineConfig->hasSSE2 ? "SSE2," : "",
			engineConfig->hasSSE3 ? "SSE3," : "",
			engineConfig->hasSSE41 ? "SSE41," : "",
			engineConfig->hasSSE42 ? "SSE42," : "");
		ImGui::Separator();
		ImGui::Text("GPU:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%s", engineConfig->gpuRenderer);
		ImGui::Text("Brand:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%s", engineConfig->gpuVendor);
		ImGui::Text("VRAM Budget:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%.1f Mb", engineConfig->vramBudget * (1.0 / 1024.0));
		ImGui::Text("VRAM Usage:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%.1f Mb", engineConfig->vramUsage * (1.0 / 1024.0));
		ImGui::Text("VRAM Available:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%.1f Mb", engineConfig->vramAvailable * (1.0 / 1024.0));
		ImGui::Text("VRAM Reserved:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%.1f Mb", engineConfig->vramReserved * (1.0 / 1024.0));
	}

	if (ImGui::CollapsingHeader("OpenGL"))
	{
		if (!modifyAttributesMenu)
		{
			if (ImGui::Button("Modify attributes") == true)
				modifyAttributesMenu = true;
		}

		if (modifyAttributesMenu)
		{
			if (ImGui::Button("Back") == true)
				modifyAttributesMenu = false;

			ImGui::Text("Depth Test");
			if (ImGui::Button("Enable") == true) glEnable(GL_DEPTH_TEST);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_DEPTH_TEST);
			ImGui::Spacing();
			ImGui::Text("Cull Face");
			if (ImGui::Button("Enable") == true) glEnable(GL_CULL_FACE);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_CULL_FACE);
			ImGui::Text("Lighting");
			if (ImGui::Button("Enable") == true) glEnable(GL_LIGHTING);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_LIGHTING);
			ImGui::Text("Color Material");
			if (ImGui::Button("Enable") == true) glEnable(GL_COLOR_MATERIAL);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_COLOR_MATERIAL);
			ImGui::Text("Texture 2D");
			if (ImGui::Button("Enable") == true) glEnable(GL_TEXTURE_2D);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_TEXTURE_2D);
			ImGui::Text("Triangles");
			if (ImGui::Button("Enable") == true) glEnable(GL_TRIANGLES);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_TRIANGLES);
			ImGui::Text("Quads");
			if (ImGui::Button("Enable") == true) glEnable(GL_QUADS);
			ImGui::SameLine();
			if (ImGui::Button("Disable") == true) glDisable(GL_QUADS);

			if (ImGui::Button("Back") == true)
				modifyAttributesMenu = false;
		}

		if (ImGui::Checkbox("Wireframe mode", &wireframe))
		{
			if (wireframe)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	ImGui::End();

	return true;
}

bool PanelConfiguration::PostUpdate()
{
	return true;
}
