#include "PanelChooser.h"
#include "Engine.h"
#include "Editor.h"
#include "FileSystem.h"
#include <imgui.h>
#include <string>
#include <vector>
PanelChooser::PanelChooser(Editor* editor)
{
	this->editor = editor;
	panelName = "Chooser";
}

PanelChooser::~PanelChooser()
{
}

bool PanelChooser::Awake()
{
	return true;
}

bool PanelChooser::PreUpdate()
{
	return true;
}

bool PanelChooser::Update()
{
	if (chooserState == OPENED)
		ShowPanel("/Assets", currentExtension);
	return true;
}

bool PanelChooser::PostUpdate()
{
	return true;
}

bool PanelChooser::IsReadyToClose(std::string id)
{
	if (this->id == id) {
		return chooserState == READY_TO_CLOSE ? true : false;
	}
	return false;
}

const char* PanelChooser::OnChooserClosed()
{
	if (chooserState == READY_TO_CLOSE || chooserState == CLOSED)
	{
		chooserState = CLOSED;
		return selectedFile[0] ? selectedFile : nullptr;
	}
	return nullptr;
}
void PanelChooser::ShowPanel(const char* path,const char* extension)
{
	ImGui::OpenPopup("Chooser");
	if (ImGui::BeginPopupModal("Chooser", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::BeginChild("ChooserPanel", ImVec2(0, 300), true);
		GetPath(path, extension);
		ImGui::EndChild();
		ImGui::PushItemWidth(200.0f);
		if (ImGui::InputText("##fileName", selectedFile, FILE_MAX, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			chooserState = READY_TO_CLOSE;
		ImGui::PopItemWidth();
		ImGui::SameLine();
		const char* extensions[] = { "fbx", "bmp","png","jpg"};
		ImGui::PushItemWidth(50.0f);
		if (ImGui::BeginCombo("##combo", currentExtension)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(extensions); n++)
			{
				bool is_selected = (currentExtension == extensions[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(extensions[n], is_selected))
					currentExtension = extensions[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Open", ImVec2(50, 0)))
			chooserState = READY_TO_CLOSE;
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 0)))
		{
			chooserState = READY_TO_CLOSE;
			selectedFile[0] = '\0';
		}

		ImGui::EndPopup();
	}
}

void PanelChooser::GetPath(const char* path, const char* extension)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	std::string dir((path) ? path : "");
	dir += "/";
	editor->engine->GetFileSystem()->EnumerateFiles(dir.c_str(),files,dirs);
	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it) {
		if (ImGui::TreeNodeEx((dir + (*it)).c_str(), 0, "%s/", (*it).c_str())) {
			GetPath((dir + (*it)).c_str(), extension);
			ImGui::TreePop();
		}
	}

	std::sort(files.begin(), files.end());
	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;
		bool ret = true;
		if (extension && str.substr(str.find_last_of(".") + 1) != extension)
			ret = false;

		if(ret && ImGui::TreeNodeEx(str.c_str(), ImGuiTreeNodeFlags_Leaf))
			{
				if (ImGui::IsItemClicked()) {
					sprintf_s(selectedFile, FILE_MAX, "%s%s", dir.c_str(), str.c_str());

					if (ImGui::IsMouseDoubleClicked(0))
						chooserState = READY_TO_CLOSE;
				}

				ImGui::TreePop();
			}
	}

}

void PanelChooser::OpenPanel(std::string id ,const char* extension, const char* from_folder)
{
	this->id = id;
	selectedFile[0] = '\0';
	currentExtension = (extension) ? extension : "";
	fileDialogOrigin = (from_folder) ? from_folder : "";
	chooserState = OPENED;
}
