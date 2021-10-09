#include "PanelAbout.h"
#include <imgui.h>
#include "Editor.h"

PanelAbout::PanelAbout(Editor* editor)
{
	this->editor = editor;
}

PanelAbout::~PanelAbout()
{
}

bool PanelAbout::Awake()
{
	return true;
}

bool PanelAbout::PreUpdate()
{
	return true;
}

bool PanelAbout::Update()
{
	if(editor->toggleAboutPanel) ShowAboutWindow(&editor->toggleAboutPanel);

	return true;
}

bool PanelAbout::PostUpdate()
{
	return true;
}

void PanelAbout::ShowAboutWindow(bool* toggleAboutPanel)
{
	if (!ImGui::Begin("About Ko-Fi Engine", toggleAboutPanel, ImGuiWindowFlags_AlwaysAutoResize))
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
	editor->Markdown("[Ko-Fi Engine v0.1](https://github.com/boscobarberesbert/Ko-Fi-Engine)");
	ImGui::Text("The next generation 3D Game Engine");
	editor->Markdown("By [Alex Avila (Omicrxn)](https://github.com/Omicrxn) & [Bosco Barber (boscobarberesbert)](https://github.com/boscobarberesbert)");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("3rd Party Libraries used:");
	editor->Markdown("  * [SDL 2.0.16](https://www.libsdl.org/)");
	editor->Markdown("  * [SDL Mixer 2.0.4](https://www.libsdl.org/projects/SDL_mixer/)");
	editor->Markdown("  * [OpenGL 4.6](https://www.opengl.org//)");
	editor->Markdown("  * [GLEW 2.1.0](http://glew.sourceforge.net/)");
	editor->Markdown("  * [Dear ImGui 1.84.2](https://github.com/ocornut/imgui)");
	editor->Markdown("  * [MathGeoLib 1.5](https://github.com/juj/MathGeoLib)");
	editor->Markdown("  * [Optick 1.3.1](https://www.optickprofiler.com/)");
	editor->Markdown("  * [MMGR 1.0](https://www.flipcode.com/archives/Presenting_A_Memory_Manager.shtml)");
	editor->Markdown("  * [Assimp 5.0.1](https://www.assimp.org/)");
	editor->Markdown("  * [JSON for Modern C++ 3.10.2](https://github.com/nlohmann/json)");
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
