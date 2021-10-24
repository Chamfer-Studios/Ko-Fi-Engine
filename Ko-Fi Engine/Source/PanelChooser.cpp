#include "PanelChooser.h"
#include "Editor.h"
#include "FileSystem.h"
#include <imgui.h>
#include <string>
#include <vector>
PanelChooser::PanelChooser(FileSystem* fileSystem)
{
	this->fileSystem = fileSystem;
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
	if (fileDialog == opened)
		ShowPanel("/Assets", current_extension);
	return true;
}

bool PanelChooser::PostUpdate()
{
	return true;
}
bool PanelChooser::FileDialog(const char* extension, const char* from_folder)
{
	bool ret = true;

	switch (fileDialog)
	{
	case closed:
		selected_file[0] = '\0';
		fileDialogFilter = (extension) ? extension : "";
		fileDialogOrigin = (from_folder) ? from_folder : "";
		fileDialog = opened;
	case opened:
		ret = false;
		break;
	}

	return ret;
}

const char* PanelChooser::CloseFileDialog()
{
	if (fileDialog == ready_to_close)
	{
		fileDialog = closed;
		return selected_file[0] ? selected_file : nullptr;
	}
	return nullptr;
}
void PanelChooser::ShowPanel(const char* path,const char* extension)
{
	ImGui::OpenPopup("File Chooser");
	if (ImGui::BeginPopupModal("File Chooser", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::BeginChild("File Browser", ImVec2(0, 300), true);
		GetPath(path, extension);
		ImGui::EndChild();
		ImGui::PushItemWidth(200.0f);
		if (ImGui::InputText("##file_selector", selected_file, FILE_MAX, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			fileDialog = ready_to_close;
		ImGui::PopItemWidth();
		ImGui::SameLine();
		const char* extensions[] = { "fbx", "bmp" };
		ImGui::PushItemWidth(50.0f);
		if (ImGui::BeginCombo("##combo", current_extension)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(extensions); n++)
			{
				bool is_selected = (current_extension == extensions[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(extensions[n], is_selected))
					current_extension = extensions[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button("Open", ImVec2(50, 0)))
			fileDialog = ready_to_close;
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 0)))
		{
			fileDialog = ready_to_close;
			selected_file[0] = '\0';
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
	fileSystem->EnumerateFiles(dir.c_str(),files,dirs);
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
					sprintf_s(selected_file, FILE_MAX, "%s%s", dir.c_str(), str.c_str());

					if (ImGui::IsMouseDoubleClicked(0))
						fileDialog = ready_to_close;
				}

				ImGui::TreePop();
			}
	}

}
