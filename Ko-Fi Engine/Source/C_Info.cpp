#include "C_Info.h"

// Modules
#include "Engine.h"
#include "M_SceneManager.h"

// GameObject
#include "GameObject.h"

#include "imgui_stdlib.h"
#include "PanelChooser.h"

C_Info::C_Info(GameObject* parent) : Component(parent)
{
	type = ComponentType::INFO;
}

C_Info::~C_Info()
{}

bool C_Info::CleanUp()
{
	return true;
}

bool C_Info::Update(float dt)
{
	return true;
}

bool C_Info::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Info"))
	{
		ImGui::Text("Name:");
		ImGui::SameLine();
		std::string newName = owner->GetName();
		if (ImGui::InputText("##Name", &(newName)))
		{
			owner->SetName(newName.c_str());
		}
		if (ImGui::Checkbox("Active", &owner->active))
		{
			owner->isActiveWindow = true;
		}
		ImGui::SameLine();
		tag = (int)owner->tag;
		// Take care with the order in the combo, it has to follow the Tag enum class order
		if (ImGui::Combo("##tagcombo", &tag, "Untagged\0Player\0Enemy\0Wall\0Projectile")) // TODO: Change to BeginCombo()
		{
			if (owner->tag != (Tag)tag)
				owner->tag = (Tag)tag;
		}
		
	}

	return ret;
}

void C_Info::Save(Json& json) const
{
	json["type"] = "info";
}

void C_Info::Load(Json& json)
{
}