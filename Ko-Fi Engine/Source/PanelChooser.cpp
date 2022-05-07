#include "PanelChooser.h"
#include "Engine.h"
#include "M_Editor.h"
#include "M_FileSystem.h"
#include "Importer.h"
#include "R_Texture.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <string>
#include <vector>

PanelChooser::PanelChooser(M_Editor* editor) : currentDir(assetsDir), fileIconPath("Assets/Icons/file_icon.png"), directoryIconPath("Assets/Icons/directory_icon.png")
{
	this->editor = editor;
	panelName = "Chooser";
	fileTexture = new R_Texture();
	directoryTexture = new R_Texture();
}

PanelChooser::~PanelChooser()
{
}

bool PanelChooser::Start()
{
	Importer::GetInstance()->textureImporter->Import(fileIconPath.c_str(),fileTexture);
	Importer::GetInstance()->textureImporter->Import(directoryIconPath.c_str(),directoryTexture);
	return true;
}

bool PanelChooser::Update()
{
	OPTICK_EVENT();

	if (chooserState == OPENED)
	{
			DrawOpenPanel(currentExtension);


	}

	return true;
}

bool PanelChooser::IsReadyToClose(std::string id)
{
	if (this->id == id) {
		return chooserState == READY_TO_CLOSE ? true : false;
	}
	return false;
}

std::string PanelChooser::OnChooserClosed()
{
	if (chooserState == READY_TO_CLOSE || chooserState == CLOSED)
	{
			chooserState = CLOSED;
			if (!selectedFile.empty())
			{
				std::string path = currentDir.string();
				std::replace(path.begin(), path.end(), '\\', '/');
				std::string returnPath = (path + "/" + selectedFile + "." + currentExtension).c_str();


				return returnPath;
			}
			else
			{
				return "";
			}


	}
	return "";
}

void PanelChooser::Save()
{
	if (chooserState == READY_TO_CLOSE || chooserState == CLOSED)
	{
		chooserState = CLOSED;
		std::string path = currentDir.string();
		std::replace(path.begin(), path.end(), '\\', '/');
		std::string returnPath = (path + "/" + selectedFile + "." + currentExtension).c_str();
		if (isSavePanel && OnSave != nullptr)
		{
			OnSave(returnPath);
			OnSave = nullptr;
		}
	}
}

void PanelChooser::DrawOpenPanel(const char* extension)
{
	ImGui::PushID(editor->idTracker++);
	std::string panelName = isSavePanel ? "Save" : "Open";
	ImGui::OpenPopup(panelName.c_str());
	if (ImGui::BeginPopupModal(panelName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "CURRENT PATH: %s", currentDir.string().c_str());
		ImGui::BeginChild("ChooserPanel", ImVec2(0, 300), true);
		GetPath(extension);
		ImGui::EndChild();
		ImGui::PushItemWidth(200.0f);
		if (ImGui::InputText("##fileName", &selectedFile, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			chooserState = READY_TO_CLOSE;
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushItemWidth(50.0f);
		if (ImGui::BeginCombo("##combo", currentExtension)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < this->extensionList.size(); n++)
			{
				bool is_selected = (currentExtension == extensionList[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(extensionList[n].c_str(), is_selected))
					currentExtension = extensionList[n].c_str();
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}
			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (!isSavePanel)
		{
			if (ImGui::Button("Open", ImVec2(50, 0)))
				chooserState = READY_TO_CLOSE;
		}
		else
		{
			if (ImGui::Button("Save", ImVec2(50, 0)))
				chooserState = READY_TO_CLOSE;
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(50, 0)))
		{
			chooserState = READY_TO_CLOSE;
			selectedFile = "";
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();
}



void PanelChooser::GetPath(const char* extension)
{
	float padding = 16.0f;
	float iconSize = 48.0f;
	float cellSize = iconSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;
	if (currentDir != std::filesystem::path(assetsDir))
	{
		if (ImGui::Button("<-"))
		{
			currentDir = currentDir.parent_path();
		}
	}
	ImGui::Columns(columnCount, 0, false);
	for (auto& directoryEntry : std::filesystem::directory_iterator(currentDir))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, assetsDir);
		std::string filenameString = relativePath.filename().string();
		if (!directoryEntry.is_directory() && extension && filenameString.substr(filenameString.find_last_of(".") + 1) != extension)
		{
			continue;
		}
			uint id = directoryEntry.is_directory() ? directoryTexture->GetTextureId() : fileTexture->GetTextureId();
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)id, { iconSize,iconSize });
			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered)
			{
				if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
				{
					if (directoryEntry.is_directory())
					{
						currentDir /= path.filename();
					}
					else
					{
						selectedFile = filenameString.substr(0, filenameString.find_last_of("."));
					}
				}
				if (ImGui::IsItemHovered() && ImGui::IsItemClicked(0))
				{
					if (!directoryEntry.is_directory())
					{
						selectedFile = filenameString.substr(0, filenameString.find_last_of("."));
					}
				}

			}
		
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

	}
	ImGui::Columns(1);

}

void PanelChooser::OpenPanel(std::string id, const char* extension, std::vector<std::string> extensionList,bool isSavePanel)
{
	this->id = id;
	selectedFile[0] = '\0';
	currentExtension = (extension) ? extension : "";
	this->extensionList = extensionList;
	chooserState = OPENED;
	this->isSavePanel = isSavePanel;
}


