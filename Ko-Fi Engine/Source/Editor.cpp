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

void LoadFonts(float fontSize_ = 12.0f);
void Markdown(const std::string& markdown_);

Editor::Editor(Window* window, Renderer3D* renderer, Input* input, EngineConfig* engineConfig)
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

	LoadFonts(16);

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

bool Editor::CallMainMenuBar()
{
	bool ret = true;

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About"))
			{
				showAboutWindow = true;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (showAboutWindow) ShowAboutWindow(&showAboutWindow);

	return ret;
}

void Editor::ShowAboutWindow(bool* p_open)
{
	if (!ImGui::Begin("About Ko-Fi Engine", p_open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}
	ImGui::Text("Ko-Fi Engine 0.1 WIP");
	ImGui::Separator();
	ImGui::Text("By Alex Avila and Bosco Barber.");
	ImGui::Text("Ko-Fi Engine is licensed under the MIT License, see LICENSE for more information.");

	static bool show_config_info = false;
	ImGui::Checkbox("Config/Build Information", &show_config_info);
	if (show_config_info)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		bool copy_to_clipboard = ImGui::Button("Copy to clipboard");
		ImVec2 child_size = ImVec2(0, ImGui::GetTextLineHeightWithSpacing() * 18);
		ImGui::BeginChildFrame(ImGui::GetID("cfg_infos"), child_size, ImGuiWindowFlags_NoMove);
		if (copy_to_clipboard)
		{
			ImGui::LogToClipboard();
			ImGui::LogText("```\n"); // Back quotes will make text appears without formatting when pasting on GitHub
		}

		ImGui::Text("Ko-Fi Engine 0.1 WIP");
		ImGui::Separator();
		ImGui::Text("sizeof(size_t): %d, sizeof(ImDrawIdx): %d, sizeof(ImDrawVert): %d", (int)sizeof(size_t), (int)sizeof(ImDrawIdx), (int)sizeof(ImDrawVert));
		ImGui::Text("define: __cplusplus=%d", (int)__cplusplus);
#ifdef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_OBSOLETE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_CLIPBOARD_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_WIN32_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_WIN32_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FORMAT_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_DEFAULT_MATH_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_DEFAULT_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_FILE_FUNCTIONS
		ImGui::Text("define: IMGUI_DISABLE_FILE_FUNCTIONS");
#endif
#ifdef IMGUI_DISABLE_DEFAULT_ALLOCATORS
		ImGui::Text("define: IMGUI_DISABLE_DEFAULT_ALLOCATORS");
#endif
#ifdef IMGUI_USE_BGRA_PACKED_COLOR
		ImGui::Text("define: IMGUI_USE_BGRA_PACKED_COLOR");
#endif
#ifdef _WIN32
		ImGui::Text("define: _WIN32");
#endif
#ifdef _WIN64
		ImGui::Text("define: _WIN64");
#endif
#ifdef __linux__
		ImGui::Text("define: __linux__");
#endif
#ifdef __APPLE__
		ImGui::Text("define: __APPLE__");
#endif
#ifdef _MSC_VER
		ImGui::Text("define: _MSC_VER=%d", _MSC_VER);
#endif
#ifdef _MSVC_LANG
		ImGui::Text("define: _MSVC_LANG=%d", (int)_MSVC_LANG);
#endif
#ifdef __MINGW32__
		ImGui::Text("define: __MINGW32__");
#endif
#ifdef __MINGW64__
		ImGui::Text("define: __MINGW64__");
#endif
#ifdef __GNUC__
		ImGui::Text("define: __GNUC__=%d", (int)__GNUC__);
#endif
#ifdef __clang_version__
		ImGui::Text("define: __clang_version__=%s", __clang_version__);
#endif
#ifdef IMGUI_HAS_VIEWPORT
		ImGui::Text("define: IMGUI_HAS_VIEWPORT");
#endif
#ifdef IMGUI_HAS_DOCK
		ImGui::Text("define: IMGUI_HAS_DOCK");
#endif
		ImGui::Separator();
		ImGui::Text("io.BackendPlatformName: %s", io.BackendPlatformName ? io.BackendPlatformName : "NULL");
		ImGui::Text("io.BackendRendererName: %s", io.BackendRendererName ? io.BackendRendererName : "NULL");
		ImGui::Text("io.ConfigFlags: 0x%08X", io.ConfigFlags);
		if (io.ConfigFlags & ImGuiConfigFlags_NavEnableKeyboard)        ImGui::Text(" NavEnableKeyboard");
		if (io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad)         ImGui::Text(" NavEnableGamepad");
		if (io.ConfigFlags & ImGuiConfigFlags_NavEnableSetMousePos)     ImGui::Text(" NavEnableSetMousePos");
		if (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard)     ImGui::Text(" NavNoCaptureKeyboard");
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouse)                  ImGui::Text(" NoMouse");
		if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)      ImGui::Text(" NoMouseCursorChange");
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)            ImGui::Text(" DockingEnable");
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)          ImGui::Text(" ViewportsEnable");
		if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)  ImGui::Text(" DpiEnableScaleViewports");
		if (io.ConfigFlags & ImGuiConfigFlags_DpiEnableScaleFonts)      ImGui::Text(" DpiEnableScaleFonts");
		if (io.MouseDrawCursor)                                         ImGui::Text("io.MouseDrawCursor");
		if (io.ConfigViewportsNoAutoMerge)                              ImGui::Text("io.ConfigViewportsNoAutoMerge");
		if (io.ConfigViewportsNoTaskBarIcon)                            ImGui::Text("io.ConfigViewportsNoTaskBarIcon");
		if (io.ConfigViewportsNoDecoration)                             ImGui::Text("io.ConfigViewportsNoDecoration");
		if (io.ConfigViewportsNoDefaultParent)                          ImGui::Text("io.ConfigViewportsNoDefaultParent");
		if (io.ConfigDockingNoSplit)                                    ImGui::Text("io.ConfigDockingNoSplit");
		if (io.ConfigDockingAlwaysTabBar)                               ImGui::Text("io.ConfigDockingAlwaysTabBar");
		if (io.ConfigDockingTransparentPayload)                         ImGui::Text("io.ConfigDockingTransparentPayload");
		if (io.ConfigMacOSXBehaviors)                                   ImGui::Text("io.ConfigMacOSXBehaviors");
		if (io.ConfigInputTextCursorBlink)                              ImGui::Text("io.ConfigInputTextCursorBlink");
		if (io.ConfigWindowsResizeFromEdges)                            ImGui::Text("io.ConfigWindowsResizeFromEdges");
		if (io.ConfigWindowsMoveFromTitleBarOnly)                       ImGui::Text("io.ConfigWindowsMoveFromTitleBarOnly");
		if (io.ConfigMemoryCompactTimer >= 0.0f)                        ImGui::Text("io.ConfigMemoryCompactTimer = %.1f", io.ConfigMemoryCompactTimer);
		ImGui::Text("io.BackendFlags: 0x%08X", io.BackendFlags);
		if (io.BackendFlags & ImGuiBackendFlags_HasGamepad)             ImGui::Text(" HasGamepad");
		if (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors)        ImGui::Text(" HasMouseCursors");
		if (io.BackendFlags & ImGuiBackendFlags_HasSetMousePos)         ImGui::Text(" HasSetMousePos");
		if (io.BackendFlags & ImGuiBackendFlags_PlatformHasViewports)   ImGui::Text(" PlatformHasViewports");
		if (io.BackendFlags & ImGuiBackendFlags_HasMouseHoveredViewport)ImGui::Text(" HasMouseHoveredViewport");
		if (io.BackendFlags & ImGuiBackendFlags_RendererHasVtxOffset)   ImGui::Text(" RendererHasVtxOffset");
		if (io.BackendFlags & ImGuiBackendFlags_RendererHasViewports)   ImGui::Text(" RendererHasViewports");
		ImGui::Separator();
		ImGui::Text("io.Fonts: %d fonts, Flags: 0x%08X, TexSize: %d,%d", io.Fonts->Fonts.Size, io.Fonts->Flags, io.Fonts->TexWidth, io.Fonts->TexHeight);
		ImGui::Text("io.DisplaySize: %.2f,%.2f", io.DisplaySize.x, io.DisplaySize.y);
		ImGui::Text("io.DisplayFramebufferScale: %.2f,%.2f", io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		ImGui::Separator();
		ImGui::Text("style.WindowPadding: %.2f,%.2f", style.WindowPadding.x, style.WindowPadding.y);
		ImGui::Text("style.WindowBorderSize: %.2f", style.WindowBorderSize);
		ImGui::Text("style.FramePadding: %.2f,%.2f", style.FramePadding.x, style.FramePadding.y);
		ImGui::Text("style.FrameRounding: %.2f", style.FrameRounding);
		ImGui::Text("style.FrameBorderSize: %.2f", style.FrameBorderSize);
		ImGui::Text("style.ItemSpacing: %.2f,%.2f", style.ItemSpacing.x, style.ItemSpacing.y);
		ImGui::Text("style.ItemInnerSpacing: %.2f,%.2f", style.ItemInnerSpacing.x, style.ItemInnerSpacing.y);

		if (copy_to_clipboard)
		{
			ImGui::LogText("\n```\n");
			ImGui::LogFinish();
		}
		ImGui::EndChildFrame();
	}

	ImGui::Separator();
	Markdown("[Ko-Fi Engine v0.1](https://github.com/boscobarberesbert/Ko-Fi-Engine)");
	ImGui::Text("The next generation 3D Game Engine");
	Markdown("By [Alex Avila (Omicrxn)](https://github.com/Omicrxn) & [Bosco Barber (boscobarberesbert)](https://github.com/boscobarberesbert)");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("3rd Party Libraries used:");
	Markdown("  * [SDL 2.0.16](https://www.libsdl.org/)");
	Markdown("  * [SDL Mixer 2.0.4](https://www.libsdl.org/projects/SDL_mixer/)");
	Markdown("  * [OpenGL 4.6](https://www.opengl.org//)");
	Markdown("  * [GLEW 2.1.0](http://glew.sourceforge.net/)");
	Markdown("  * [Dear ImGui 1.84.2](https://github.com/ocornut/imgui)");
	Markdown("  * [MathGeoLib 1.5](https://github.com/juj/MathGeoLib)");
	Markdown("  * [Optick 1.3.1](https://www.optickprofiler.com/)");
	Markdown("  * [MMGR 1.0](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml)");
	Markdown("  * [Assimp 5.0.1](https://www.assimp.org/)");
	Markdown("  * [JSON for Modern C++ 3.10.2](https://github.com/nlohmann/json)");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("License:");
	ImGui::Spacing();
	ImGui::Text("MIT License");
	ImGui::Spacing();
	ImGui::Text("Copyright (c) 2021 Bosco Barber Esbert");
	ImGui::Spacing();
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files (the 'Software'), to deal");
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions:");
	ImGui::Spacing();
	ImGui::Text("The above copyright notice and this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::Spacing();
	ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR");
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE.");
	ImGui::End();
}

#include "ImGui.h"                // https://github.com/ocornut/imgui
#include "imgui_markdown.h"       // https://github.com/juliettef/imgui_markdown
#include "IconsFontAwesome5.h"    // https://github.com/juliettef/IconFontCppHeaders

// Following includes for Windows LinkCallback
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Shellapi.h"
#include <string>

void LinkCallback(ImGui::MarkdownLinkCallbackData data_);
inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_);

static ImFont* H1 = NULL;
static ImFont* H2 = NULL;
static ImFont* H3 = NULL;

static ImGui::MarkdownConfig mdConfig;


void LinkCallback(ImGui::MarkdownLinkCallbackData data_)
{
	std::string url(data_.link, data_.linkLength);
	if (!data_.isImage)
	{
		ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}

inline ImGui::MarkdownImageData ImageCallback(ImGui::MarkdownLinkCallbackData data_)
{
	// In your application you would load an image based on data_ input. Here we just use the imgui font texture.
	ImTextureID image = ImGui::GetIO().Fonts->TexID;
	// > C++14 can use ImGui::MarkdownImageData imageData{ true, false, image, ImVec2( 40.0f, 20.0f ) };
	ImGui::MarkdownImageData imageData;
	imageData.isValid = true;
	imageData.useLinkCallback = false;
	imageData.user_texture_id = image;
	imageData.size = ImVec2(40.0f, 20.0f);

	// For image resize when available size.x > image width, add
	ImVec2 const contentSize = ImGui::GetContentRegionAvail();
	if (imageData.size.x > contentSize.x)
	{
		float const ratio = imageData.size.y / imageData.size.x;
		imageData.size.x = contentSize.x;
		imageData.size.y = contentSize.x * ratio;
	}

	return imageData;
}

void LoadFonts(float fontSize_/* = 12.0f*/)
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->Clear();
	// Base font
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Regular.ttf", fontSize_);
	// Bold headings H2 and H3
	H2 = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Bold.ttf", fontSize_);
	H3 = mdConfig.headingFormats[1].font;
	// bold heading H1
	float fontSizeH1 = fontSize_ * 1.1f;
	H1 = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Bold.ttf", fontSizeH1);
}

void ExampleMarkdownFormatCallback(const ImGui::MarkdownFormatInfo& markdownFormatInfo_, bool start_)
{
	// Call the default first so any settings can be overwritten by our implementation.
	// Alternatively could be called or not called in a switch statement on a case by case basis.
	// See defaultMarkdownFormatCallback definition for furhter examples of how to use it.
	ImGui::defaultMarkdownFormatCallback(markdownFormatInfo_, start_);

	switch (markdownFormatInfo_.type)
	{
		// example: change the colour of heading level 2
	case ImGui::MarkdownFormatType::HEADING:
	{
		if (markdownFormatInfo_.level == 2)
		{
			if (start_)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
			}
			else
			{
				ImGui::PopStyleColor();
			}
		}
		break;
	}
	default:
	{
		break;
	}
	}
}

void Markdown(const std::string& markdown_)
{
	// You can make your own Markdown function with your prefered string container and markdown config.
	// > C++14 can use ImGui::MarkdownConfig mdConfig{ LinkCallback, NULL, ImageCallback, ICON_FA_LINK, { { H1, true }, { H2, true }, { H3, false } }, NULL };
	mdConfig.linkCallback = LinkCallback;
	mdConfig.tooltipCallback = NULL;
	mdConfig.imageCallback = ImageCallback;
	mdConfig.linkIcon = ICON_FA_LINK;
	mdConfig.headingFormats[0] = { H1, true };
	mdConfig.headingFormats[1] = { H2, true };
	mdConfig.headingFormats[2] = { H3, false };
	mdConfig.userData = NULL;
	mdConfig.formatCallback = ExampleMarkdownFormatCallback;
	ImGui::Markdown(markdown_.c_str(), markdown_.length(), mdConfig);
}

void MarkdownExample()
{
	const std::string markdownText = u8R"(
# H1 Header: Text and Links
You can add [links like this one to enkisoftware](https://www.enkisoftware.com/) and lines will wrap well.
You can also insert images ![image alt text](image identifier e.g. filename)
Horizontal rules:
***
___
*Emphasis* and **strong emphasis** change the appearance of the text.
## H2 Header: indented text.
  This text has an indent (two leading spaces).
    This one has two.
### H3 Header: Lists
  * Unordered lists
    * Lists can be indented with two extra spaces.
  * Lists can have [links like this one to Avoyd](https://www.avoyd.com/) and *emphasized text*
)";
	Markdown(markdownText);
}
