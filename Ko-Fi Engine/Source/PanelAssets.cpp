#include "PanelAssets.h"
#include "M_FileSystem.h"
#include "PanelTextEditor.h"
#include "Engine.h"
#include "FSDefs.h"
#include <imgui.h>
#include "M_Editor.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>

PanelAssets::PanelAssets(M_Editor* editor) : currentDir(assetsDir), fileIcon(ICONS_DIR + std::string("file_icon.png")), directoryIcon(ICONS_DIR + std::string("directory_icon.png"))
{
	panelName = "Assets";
	this->editor = editor;

}

PanelAssets::~PanelAssets()
{
}

bool PanelAssets::Start()
{
	LoadIcons(fileTexture, fileIcon.c_str());
	LoadIcons(directoryTexture, directoryIcon.c_str());
	return true;
}

bool PanelAssets::Update()
{
	OPTICK_EVENT();

#ifdef KOFI_GAME
	return true;
#endif

	ImGui::Begin("Assets");
	float padding = 16.0f;
	float iconSize = 96.0f;
	float cellSize = iconSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;
	//Back button when we enter on a folder, however we dont want it to go further back if we are on the assets folder or the
	//user could access system files which is not right
	if (currentDir != std::filesystem::path(assetsDir)) {
		if (ImGui::Button("<-")) {
			//setting current directory to its parent (the one before)
			currentDir = currentDir.parent_path();
		}
	}
	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(currentDir))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, assetsDir);

		std::string filenameString = relativePath.filename().string();

		if (relativePath.extension().string() != META_EXTENSION)
		{
			uint id = directoryEntry.is_directory() ? directoryTexture.id : fileTexture.id;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton((ImTextureID)id, { iconSize,iconSize });
			ImGui::PopStyleColor();

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
			{
				if (directoryEntry.is_directory())
				{
					currentDir /= path.filename();
				}
				else
				{
					std::string ext = path.extension().string();
					if (ext == ".milk" || ext == ".cream" || ext == ".txt" || ext == ".glsl" || ext == ".mat" || ext == ".lua")
					{
						editor->OpenTextEditor(path.string(), ext.c_str());
						PanelTextEditor* panel = editor->GetPanel<PanelTextEditor>();
						panel->Focus();
					}
				}

			}
			if (ImGui::IsItemHovered() && (ImGui::IsMouseClicked(1) || ImGui::IsMouseClicked(0)))
			{
				if (!directoryEntry.is_directory())
				{
					selectedFile = path.string();

				}
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				std::string itemPath = selectedFile;

				if (itemPath.find_last_of('.') != std::string::npos)
				{
					ImGui::SetDragDropPayload("ASSETS_ITEM", itemPath.c_str(), itemPath.size() * sizeof(const char*));
					ImGui::Text(itemPath.c_str());
				}
				ImGui::EndDragDropSource();
			}
			ImGui::TextWrapped(filenameString.c_str());
			ImGui::NextColumn();
		}
	}
	if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("File Handle");
	}
	ImGui::Columns(1);
	if (ImGui::BeginPopup("File Handle"))
	{
		if (ImGui::BeginMenu("Create Smth"))
		{
			if (ImGui::MenuItem("New File"))
			{
				std::filesystem::create_directory(currentDir.string());
			}
			if (ImGui::MenuItem("Scene"))
			{
				std::string fileName = FileExistsScene("/scene.json", 1);
				std::string path = currentDir.string() + fileName;
				editor->engine->GetFileSystem()->CreateScene(path.c_str(),fileName.c_str());
			}
			if (ImGui::MenuItem("Material"))
			{
				std::string fileName =  FileExistsMaterial("/material.milk",1);
			
				std::string path = currentDir.string() + fileName;
				editor->engine->GetFileSystem()->CreateMaterial(path.c_str());
			}
			if (ImGui::MenuItem("Shader"))
			{
				std::string fileName = FileExsistsShader("/shader.glsl", 1);

				std::string path = currentDir.string() + fileName;
				editor->engine->GetFileSystem()->CreateShader(path.c_str());
			}
			ImGui::EndMenu();
		}

		if (ImGui::MenuItem("Delete"))
		{
			std::filesystem::remove(selectedFile);
			selectedFile = "";
		}


		ImGui::EndPopup();

	}
	ImGui::End();
	return true;
}

void PanelAssets::LoadIcons(TextureIcon& texture, const char* path)
{
	unsigned char* pixels = stbi_load(path, &texture.width, &texture.height, &texture.nrChannels, STBI_rgb_alpha);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture.id);
	glBindTexture(GL_TEXTURE_2D, texture.id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(pixels);
}

std::string PanelAssets::FileExistsScene(std::string fileName, int i)
{
	int j = i;
	std::string name = "";
	std::string number = "";
	std::string ext = "";
	if (std::filesystem::exists(currentDir.string() + fileName))
	{
		name = "/scene";
		number = std::to_string(j);
		ext = ".json";
		return FileExistsMaterial(name + " (" + number + ") " + ext, j + 1);
	}
	else
	{
		return fileName;
	}
	return name + number + ext;
}

std::string PanelAssets::FileExistsMaterial(std::string fileName,int i)
{
	int j = i;
	std::string name = "";
	std::string number = "";
	std::string ext = "";
	if (std::filesystem::exists(currentDir.string() + fileName))
	{
		name = "/material";
		number = std::to_string(j);
		ext = ".milk";
		return FileExistsMaterial(name + " (" + number + ") " + ext, j + 1);
	}
	else
	{
		return fileName;
	}
	return name + number + ext;
}

std::string PanelAssets::FileExsistsShader(std::string fileName, int i)
{
	int j = i;
	std::string name = "";
	std::string number = "";
	std::string ext = "";
	if (std::filesystem::exists(currentDir.string() + fileName))
	{
		name = "/shader";
		number = std::to_string(j);
		ext = ".glsl";
		return FileExsistsShader(name + " (" + number + ") " + ext, j + 1);
	}
	else
		return fileName;

	return name + number + ext;
}