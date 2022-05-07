#include "PanelResources.h"
#include "M_Editor.h"
#include "M_ResourceManager.h"
#include "Resource.h"
#include "M_ResourceManager.h"

#include "optick.h"

PanelResources::PanelResources(M_Editor* editor)
{
	panelName = "Resources";
	this->editor = editor;
	this->resourceManager = nullptr;
}

PanelResources::~PanelResources()
{
}

bool PanelResources::Awake()
{
	return true;
}

bool PanelResources::Update()
{
	OPTICK_EVENT();

	if (editor->toggleResourcesPanel) ShowResourcesWindow(&editor->toggleResourcesPanel);

	return true;
}

void PanelResources::ShowResourcesWindow(bool* toggleResourcesPanel)
{
	if (!ImGui::Begin("References", toggleResourcesPanel, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::End();
		return;
	}

	if (resourceManager == nullptr)
	{
		ImGui::End();
		return;
	}

	ImVec4 blue(0.0f, 1.0f, 1.0f, 0.8f);
	ImVec4 yellow(1.0f, 1.0f, 0.0f, 1.0f);

	uint models = 0;
	uint meshes = 0;
	uint textures = 0;
	uint animations = 0;
	uint shaders = 0;

	const std::map<UID, Resource*>* resourcesMap = resourceManager->GetResourcesMap();
	std::multimap<uint, Resource*> sorted;

	for (std::map<UID, Resource*>::const_iterator item = resourcesMap->cbegin(); item != resourcesMap->cend(); ++item)
	{
		if (item->second == nullptr)
			continue;

		switch (item->second->GetType())
		{
		case ResourceType::MODEL:
			++models;
			break;
		case ResourceType::MESH:
			++meshes;
			break;
		case ResourceType::MATERIAL:
			++shaders;
			break;
		case ResourceType::TEXTURE:
			++textures;
			break;
		case ResourceType::ANIMATION:
			++animations;
			break;
		}

		sorted.emplace((uint)item->second->GetType(), item->second);
	}

	for (const auto& rIt : sorted)
	{
		ImGui::TextColored(blue, "%s", rIt.second->GetAssetFile());
		ImGui::Text("UID:");
		ImGui::SameLine();
		ImGui::TextColored(yellow, "%lu", rIt.second->GetUID());

		std::string type;
		switch (rIt.second->GetType())
		{
		case ResourceType::MODEL:
			type = "Model";
			break;
		case ResourceType::MESH:
			type = "Mesh";
			break;
		case ResourceType::MATERIAL:
			type = "Shader";
			break;
		case ResourceType::TEXTURE:
			type = "Texture";
			break;
		case ResourceType::ANIMATION:
			type = "Animation";
			break;
		}

		ImGui::Text("Type:");
		ImGui::SameLine();
		ImGui::TextColored(yellow, "%s", type.c_str());

		ImGui::Text("References:");
		ImGui::SameLine();
		ImGui::TextColored(yellow, "%u", rIt.second->GetReferenceCount());

		ImGui::Separator();
	}

	sorted.clear();

	ImGui::Text("Total Models:");
	ImGui::SameLine();
	ImGui::TextColored(yellow, "%u", models);

	ImGui::Text("Total Meshes:");
	ImGui::SameLine();
	ImGui::TextColored(yellow, "%u", meshes);

	ImGui::Text("Total Shaders:");
	ImGui::SameLine();
	ImGui::TextColored(yellow, "%u", shaders);

	ImGui::Text("Total Textures:");
	ImGui::SameLine();
	ImGui::TextColored(yellow, "%u", textures);

	ImGui::Text("Total Animations:");
	ImGui::SameLine();
	ImGui::TextColored(yellow, "%u", animations);

	ImGui::End();
}