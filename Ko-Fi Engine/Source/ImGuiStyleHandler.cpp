#include "ImGuiStyleHandler.h"
#include "imgui.h"

ImGuiStyleHandler::ImGuiStyleHandler()
{
	styleJson = jsonHandler.LoadJson("EngineConfig/style.json");
}

ImGuiStyleHandler::~ImGuiStyleHandler()
{
}

ImGuiStyle* ImGuiStyleHandler::SetKoFiStyle()
{

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowPadding = ImVec2(styleJson.at("Window").at("WindowPadding").at("x"), styleJson.at("Window").at("WindowPadding").at("y"));
	style->WindowRounding = styleJson.at("Window").at("WindowRounding");
	style->FramePadding = ImVec2(styleJson.at("Window").at("FramePadding").at("x"), styleJson.at("Window").at("FramePadding").at("y"));
	style->FrameRounding = styleJson.at("Window").at("FrameRounding");
	style->ItemSpacing = ImVec2(styleJson.at("Window").at("ItemSpacing").at("x"), styleJson.at("Window").at("ItemSpacing").at("y"));
	style->ItemInnerSpacing = ImVec2(styleJson.at("Window").at("ItemInnerSpacing").at("x"), styleJson.at("Window").at("ItemInnerSpacing").at("y"));
	style->IndentSpacing = styleJson.at("Window").at("IndentSpacing");
	style->ScrollbarSize = styleJson.at("Window").at("ScrollbarSize");
	style->ScrollbarRounding = styleJson.at("Window").at("ScrollbarRounding");
	style->GrabMinSize = styleJson.at("Window").at("GrabMinSize");
	style->GrabRounding = styleJson.at("Window").at("GrabRounding");

	style->Colors[ImGuiCol_Text] = ImVec4(
		styleJson.at("Colors").at("Text").at("x"),
		styleJson.at("Colors").at("Text").at("y"), 
		styleJson.at("Colors").at("Text").at("z"),
		styleJson.at("Colors").at("Text").at("w")
	);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(
		styleJson.at("Colors").at("TextSelectedBg").at("x"),
		styleJson.at("Colors").at("TextSelectedBg").at("y"),
		styleJson.at("Colors").at("TextSelectedBg").at("z"),
		styleJson.at("Colors").at("TextSelectedBg").at("w")
	);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(
		styleJson.at("Colors").at("TextDisabled").at("x"),
		styleJson.at("Colors").at("TextDisabled").at("y"),
		styleJson.at("Colors").at("TextDisabled").at("z"),
		styleJson.at("Colors").at("TextDisabled").at("w")
	);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(
		styleJson.at("Colors").at("WindowBg").at("x"),
		styleJson.at("Colors").at("WindowBg").at("y"),
		styleJson.at("Colors").at("WindowBg").at("z"),
		styleJson.at("Colors").at("WindowBg").at("w")
	);
	//Window bg when docking
	style->Colors[ImGuiCol_ChildBg] = ImVec4(
		styleJson.at("Colors").at("ChildWindowBg").at("x"),
		styleJson.at("Colors").at("ChildWindowBg").at("y"),
		styleJson.at("Colors").at("ChildWindowBg").at("z"),
		styleJson.at("Colors").at("ChildWindowBg").at("w")
	);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(
		styleJson.at("Colors").at("PopupBg").at("x"),
		styleJson.at("Colors").at("PopupBg").at("y"),
		styleJson.at("Colors").at("PopupBg").at("z"),
		styleJson.at("Colors").at("PopupBg").at("w")
	);
	style->Colors[ImGuiCol_Border] = ImVec4(
		styleJson.at("Colors").at("Border").at("x"),
		styleJson.at("Colors").at("Border").at("y"),
		styleJson.at("Colors").at("Border").at("z"),
		styleJson.at("Colors").at("Border").at("w")
	);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(
		styleJson.at("Colors").at("BorderShadow").at("x"),
		styleJson.at("Colors").at("BorderShadow").at("y"),
		styleJson.at("Colors").at("BorderShadow").at("z"),
		styleJson.at("Colors").at("BorderShadow").at("w")
	);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(
		styleJson.at("Colors").at("FrameBg").at("x"),
		styleJson.at("Colors").at("FrameBg").at("y"),
		styleJson.at("Colors").at("FrameBg").at("z"),
		styleJson.at("Colors").at("FrameBg").at("w")
	);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(
		styleJson.at("Colors").at("FrameBgHovered").at("x"),
		styleJson.at("Colors").at("FrameBgHovered").at("y"),
		styleJson.at("Colors").at("FrameBgHovered").at("z"),
		styleJson.at("Colors").at("FrameBgHovered").at("w")
	);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(
		styleJson.at("Colors").at("FrameBgActive").at("x"),
		styleJson.at("Colors").at("FrameBgActive").at("y"),
		styleJson.at("Colors").at("FrameBgActive").at("z"),
		styleJson.at("Colors").at("FrameBgActive").at("w")
	);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(
		styleJson.at("Colors").at("TitleBg").at("x"),
		styleJson.at("Colors").at("TitleBg").at("y"),
		styleJson.at("Colors").at("TitleBg").at("z"),
		styleJson.at("Colors").at("TitleBg").at("w")
	);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(
		styleJson.at("Colors").at("TitleBgCollapsed").at("x"),
		styleJson.at("Colors").at("TitleBgCollapsed").at("y"),
		styleJson.at("Colors").at("TitleBgCollapsed").at("z"),
		styleJson.at("Colors").at("TitleBgCollapsed").at("w")
	);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(
		styleJson.at("Colors").at("TitleBgActive").at("x"),
		styleJson.at("Colors").at("TitleBgActive").at("y"),
		styleJson.at("Colors").at("TitleBgActive").at("z"),
		styleJson.at("Colors").at("TitleBgActive").at("w")
	);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(
		styleJson.at("Colors").at("MenuBarBg").at("x"),
		styleJson.at("Colors").at("MenuBarBg").at("y"),
		styleJson.at("Colors").at("MenuBarBg").at("z"),
		styleJson.at("Colors").at("MenuBarBg").at("w")
	);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(
		styleJson.at("Colors").at("ScrollbarBg").at("x"),
		styleJson.at("Colors").at("ScrollbarBg").at("y"),
		styleJson.at("Colors").at("ScrollbarBg").at("z"),
		styleJson.at("Colors").at("ScrollbarBg").at("w")
	);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(
		styleJson.at("Colors").at("ScrollbarGrab").at("x"),
		styleJson.at("Colors").at("ScrollbarGrab").at("y"),
		styleJson.at("Colors").at("ScrollbarGrab").at("z"),
		styleJson.at("Colors").at("ScrollbarGrab").at("w")
	);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(
		styleJson.at("Colors").at("ScrollbarGrabHovered").at("x"),
		styleJson.at("Colors").at("ScrollbarGrabHovered").at("y"),
		styleJson.at("Colors").at("ScrollbarGrabHovered").at("z"),
		styleJson.at("Colors").at("ScrollbarGrabHovered").at("w")
	);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(
		styleJson.at("Colors").at("ScrollbarGrabActive").at("x"),
		styleJson.at("Colors").at("ScrollbarGrabActive").at("y"),
		styleJson.at("Colors").at("ScrollbarGrabActive").at("z"),
		styleJson.at("Colors").at("ScrollbarGrabActive").at("w")
	);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(
		styleJson.at("Colors").at("CheckMark").at("x"),
		styleJson.at("Colors").at("CheckMark").at("y"),
		styleJson.at("Colors").at("CheckMark").at("z"),
		styleJson.at("Colors").at("CheckMark").at("w")
	);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(
		styleJson.at("Colors").at("SliderGrab").at("x"),
		styleJson.at("Colors").at("SliderGrab").at("y"),
		styleJson.at("Colors").at("SliderGrab").at("z"),
		styleJson.at("Colors").at("SliderGrab").at("w")
	);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(
		styleJson.at("Colors").at("SliderGrabActive").at("x"),
		styleJson.at("Colors").at("SliderGrabActive").at("y"),
		styleJson.at("Colors").at("SliderGrabActive").at("z"),
		styleJson.at("Colors").at("SliderGrabActive").at("w")
	);
	style->Colors[ImGuiCol_Button] = ImVec4(
		styleJson.at("Colors").at("Button").at("x"),
		styleJson.at("Colors").at("Button").at("y"),
		styleJson.at("Colors").at("Button").at("z"),
		styleJson.at("Colors").at("Button").at("w")
	);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(
		styleJson.at("Colors").at("ButtonHovered").at("x"),
		styleJson.at("Colors").at("ButtonHovered").at("y"),
		styleJson.at("Colors").at("ButtonHovered").at("z"),
		styleJson.at("Colors").at("ButtonHovered").at("w")
	);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(
		styleJson.at("Colors").at("ButtonActive").at("x"),
		styleJson.at("Colors").at("ButtonActive").at("y"),
		styleJson.at("Colors").at("ButtonActive").at("z"),
		styleJson.at("Colors").at("ButtonActive").at("w")
	);
	style->Colors[ImGuiCol_Header] = ImVec4(
		styleJson.at("Colors").at("Header").at("x"),
		styleJson.at("Colors").at("Header").at("y"),
		styleJson.at("Colors").at("Header").at("z"),
		styleJson.at("Colors").at("Header").at("w")
	);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(
		styleJson.at("Colors").at("PopupBg").at("x"),
		styleJson.at("Colors").at("PopupBg").at("y"),
		styleJson.at("Colors").at("PopupBg").at("z"),
		styleJson.at("Colors").at("PopupBg").at("w")
	);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(
		styleJson.at("Colors").at("HeaderHovered").at("x"),
		styleJson.at("Colors").at("HeaderHovered").at("y"),
		styleJson.at("Colors").at("HeaderHovered").at("z"),
		styleJson.at("Colors").at("HeaderHovered").at("w")
	);
	style->Colors[ImGuiCol_Separator] = ImVec4(
		styleJson.at("Colors").at("HeaderActive").at("x"),
		styleJson.at("Colors").at("HeaderActive").at("y"),
		styleJson.at("Colors").at("HeaderActive").at("z"),
		styleJson.at("Colors").at("HeaderActive").at("w")
	);
	style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(
		styleJson.at("Colors").at("SeparatorHovered").at("x"),
		styleJson.at("Colors").at("SeparatorHovered").at("y"),
		styleJson.at("Colors").at("SeparatorHovered").at("z"),
		styleJson.at("Colors").at("SeparatorHovered").at("w")
	);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(
		styleJson.at("Colors").at("SeparatorActive").at("x"),
		styleJson.at("Colors").at("SeparatorActive").at("y"),
		styleJson.at("Colors").at("SeparatorActive").at("z"),
		styleJson.at("Colors").at("SeparatorActive").at("w")
	);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(
		styleJson.at("Colors").at("ResizeGrip").at("x"),
		styleJson.at("Colors").at("ResizeGrip").at("y"),
		styleJson.at("Colors").at("ResizeGrip").at("z"),
		styleJson.at("Colors").at("ResizeGrip").at("w")
	);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(
		styleJson.at("Colors").at("ResizeGripHovered").at("x"),
		styleJson.at("Colors").at("ResizeGripHovered").at("y"),
		styleJson.at("Colors").at("ResizeGripHovered").at("z"),
		styleJson.at("Colors").at("ResizeGripHovered").at("w")
	);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(
		styleJson.at("Colors").at("ResizeGripActive").at("x"),
		styleJson.at("Colors").at("ResizeGripActive").at("y"),
		styleJson.at("Colors").at("ResizeGripActive").at("z"),
		styleJson.at("Colors").at("ResizeGripActive").at("w")
	);
	style->Colors[ImGuiCol_Tab] = ImVec4(
		styleJson.at("Colors").at("Tab").at("x"),
		styleJson.at("Colors").at("Tab").at("y"),
		styleJson.at("Colors").at("Tab").at("z"),
		styleJson.at("Colors").at("Tab").at("w")
	);
	//Tab label color when hover on label
	style->Colors[ImGuiCol_TabHovered] = ImVec4(
		styleJson.at("Colors").at("TabHovered").at("x"),
		styleJson.at("Colors").at("TabHovered").at("y"),
		styleJson.at("Colors").at("TabHovered").at("z"),
		styleJson.at("Colors").at("TabHovered").at("w")
	);
	//Tab label color when tab active
	style->Colors[ImGuiCol_TabActive] = ImVec4(
		styleJson.at("Colors").at("TabActive").at("x"),
		styleJson.at("Colors").at("TabActive").at("y"),
		styleJson.at("Colors").at("TabActive").at("z"),
		styleJson.at("Colors").at("TabActive").at("w")
	);
	//TabLabel color when unfocused on docking
	style->Colors[ImGuiCol_TabUnfocused] = ImVec4(
		styleJson.at("Colors").at("TabUnfocused").at("x"),
		styleJson.at("Colors").at("TabUnfocused").at("y"),
		styleJson.at("Colors").at("TabUnfocused").at("z"),
		styleJson.at("Colors").at("TabUnfocused").at("w")
	);
	//TabLabel color when unfocused & active on docking
	style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(
		styleJson.at("Colors").at("TabUnfocusedActive").at("x"),
		styleJson.at("Colors").at("TabUnfocusedActive").at("y"),
		styleJson.at("Colors").at("TabUnfocusedActive").at("z"),
		styleJson.at("Colors").at("TabUnfocusedActive").at("w")
	);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(
		styleJson.at("Colors").at("PlotLines").at("x"),
		styleJson.at("Colors").at("PlotLines").at("y"),
		styleJson.at("Colors").at("PlotLines").at("z"),
		styleJson.at("Colors").at("PlotLines").at("w")
	);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(
		styleJson.at("Colors").at("PlotLinesHovered").at("x"),
		styleJson.at("Colors").at("PlotLinesHovered").at("y"),
		styleJson.at("Colors").at("PlotLinesHovered").at("z"),
		styleJson.at("Colors").at("PlotLinesHovered").at("w")
	);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(
		styleJson.at("Colors").at("PlotHistogram").at("x"),
		styleJson.at("Colors").at("PlotHistogram").at("y"),
		styleJson.at("Colors").at("PlotHistogram").at("z"),
		styleJson.at("Colors").at("PlotHistogram").at("w")
	);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(
		styleJson.at("Colors").at("PlotHistogramHovered").at("x"),
		styleJson.at("Colors").at("PlotHistogramHovered").at("y"),
		styleJson.at("Colors").at("PlotHistogramHovered").at("z"),
		styleJson.at("Colors").at("PlotHistogramHovered").at("w")
	);
	style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(
		styleJson.at("Colors").at("ModalWindowDimBg").at("x"),
		styleJson.at("Colors").at("ModalWindowDimBg").at("y"),
		styleJson.at("Colors").at("ModalWindowDimBg").at("z"),
		styleJson.at("Colors").at("ModalWindowDimBg").at("w")
	);
	return style;
}
