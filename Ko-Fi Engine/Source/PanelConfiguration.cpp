#include "PanelConfiguration.h"
#include "PanelChooser.h"
#include "Engine.h"
#include "M_Window.h"
#include "M_SceneManager.h"
#include "M_Renderer3D.h"
#include "M_Input.h"
#include "EngineConfig.h"
#include "M_Editor.h"
#include <imgui.h>
#include "glew.h"

PanelConfiguration::PanelConfiguration(M_Editor* editor, EngineConfig* engineConfig)
{
	panelName = "Configuration";
	
	this->engineConfig = engineConfig;
	this->editor = editor;
}

PanelConfiguration::~PanelConfiguration()
{
}

bool PanelConfiguration::Awake()
{
	return true;
}

bool PanelConfiguration::Update()
{
	OPTICK_EVENT();

	ImGui::Begin(panelName.c_str(),0);

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
		strcpy_s(appName, 120, engineConfig->title.c_str());
		ImGui::InputText("App Name",appName,120,ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		static char organization[120];
		strcpy_s(organization, 120, engineConfig->organization.c_str());
		ImGui::InputText("Organization", organization, 120, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

		if (ImGui::SliderInt("Max FPS", &engineConfig->maxFps, 0, 120))
			engineConfig->cappedMs = 1000 / engineConfig->maxFps;

		char title[25];
		sprintf_s(title, 25, "Framerate %.1f", engineConfig->fpsLog[engineConfig->fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &engineConfig->fpsLog[0], engineConfig->fpsLog.size(), 0, title, 0.0f, 100, ImVec2(310, 100));
		sprintf_s(title, 25, "Milliseconds %.1f", engineConfig->msLog[engineConfig->msLog.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &engineConfig->msLog[0], engineConfig->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}

	if (ImGui::CollapsingHeader("Resource Manager"))
	{
		if (ImGui::Checkbox("Show Loaded Resources", &editor->toggleResourcesPanel)) {}
	}

	if (ImGui::CollapsingHeader("Renderer"))
	{
		bool vsync = editor->engine->GetRenderer()->GetVsync();
		if (ImGui::Checkbox("VSync", &vsync))
			editor->engine->GetRenderer()->SetVsync(vsync);
		if (ImGui::Checkbox("Draw scene partition tree", &editor->engine->GetSceneManager()->GetCurrentScene()->drawSceneTree)) {}
	}

	if (ImGui::CollapsingHeader("Input"))
	{
		int mouseX = editor->engine->GetInput()->GetMouseX();
		int mouseY = editor->engine->GetInput()->GetMouseY();
		ImGui::Text("Mouse Position:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i,%i", mouseX, mouseY);

		mouseX = editor->engine->GetInput()->GetMouseXMotion();
		mouseY = editor->engine->GetInput()->GetMouseYMotion();
		ImGui::Text("Mouse Motion:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i,%i", mouseX, mouseY);

		int wheel = editor->engine->GetInput()->GetMouseZ();
		ImGui::Text("Mouse Wheel:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%i", wheel);
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		ImGui::Text("Icon:");
		ImGui::SameLine();
		if (editor->GetPanelChooser()->IsReadyToClose("PanelConfig"))
		{
			std::string file = editor->GetPanelChooser()->OnChooserClosed();
			if (!file.empty())
			{
				std::string newFile = file;
				newFile.erase(newFile.begin());
				editor->engine->GetWindow()->SetIcon(newFile.c_str());
			}
		}
		if (ImGui::Selectable(editor->engine->GetWindow()->GetIcon()))
			editor->GetPanelChooser()->OpenPanel("PanelConfig", "bmp", { "bmp" });
		float brightness = editor->engine->GetWindow()->GetBrightness();
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
			editor->engine->GetWindow()->AdjustBrightness(brightness);
		int width = editor->engine->GetWindow()->GetWidth();
		int height = editor->engine->GetWindow()->GetHeight();
		if (ImGui::SliderInt("Width", &width, 640, 4096) | ImGui::SliderInt("Height", &height, 480, 2160))
		{
			SDL_SetWindowSize(editor->engine->GetWindow()->window, width, height);
			editor->engine->GetWindow()->SetWidth(width);
			editor->engine->GetWindow()->SetHeight(height);
		}
		ImGui::Text("Refresh rate:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0),"%u", editor->engine->GetWindow()->GetRefreshRate());
		bool temp = editor->engine->GetWindow()->GetFullscreen();
		if (ImGui::Checkbox("Fullscreen",&temp))
			editor->engine->GetWindow()->SetFullscreen(temp);
		ImGui::SameLine();
		temp = editor->engine->GetWindow()->GetFullscreenDesktop();
		if (ImGui::Checkbox("Fullscreen Desktop", &temp))
			editor->engine->GetWindow()->SetFullscreenDesktop(temp);
		temp = editor->engine->GetWindow()->GetResizable();
		if (ImGui::Checkbox("Resizable", &temp))
			editor->engine->GetWindow()->SetResizable(temp);
		ImGui::SameLine();
		temp = editor->engine->GetWindow()->GetBorderless();
		if (ImGui::Checkbox("Borderless", &temp))
			editor->engine->GetWindow()->SetBorderless(temp);
	}

	if (ImGui::CollapsingHeader("Hardware")) {
		ImGui::Text("SDL Version:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%d.%d.%d", engineConfig->sdlVersion.major, engineConfig->sdlVersion.minor, engineConfig->sdlVersion.patch);
		ImGui::Separator();
		ImGui::Text("OpenGL Version:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.8196, 0.7176, 0.6078, 1.0), "%s", engineConfig->gpuVersion);
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

			bool enabled = glIsEnabled(GL_DEPTH_TEST);
			if (ImGui::Checkbox("Depth Test", &enabled))
				enabled? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

			enabled = glIsEnabled(GL_CULL_FACE);
			if (ImGui::Checkbox("Cull Face", &enabled))
				enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

			enabled = glIsEnabled(GL_LIGHTING);
			if (ImGui::Checkbox("Lighting", &enabled))
				enabled ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

			enabled = glIsEnabled(GL_COLOR_MATERIAL);
			if (ImGui::Checkbox("Color Material", &enabled))
				enabled ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

			enabled = glIsEnabled(GL_TEXTURE_2D);
			if (ImGui::Checkbox("Texture 2D", &enabled))
				enabled ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
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
