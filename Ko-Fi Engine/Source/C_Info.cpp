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
	showPopup = false;
}

C_Info::~C_Info()
{
}

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
		if (ImGui::Combo("##tagcombo", &tag, "Untagged\0Player\0Enemy\0Floor\0DecorationFloor\0PickUp\0Corpse\0Dialogue")) // TODO: Change to BeginCombo()
		{
			if (owner->tag != (Tag)tag)
				owner->tag = (Tag)tag;
		}

		// ImGui button
		if (ImGui::Button("Propagate Tag"))
			showPopup = true;

		// titlePopup is used for the title that the pop up window has
		std::string titlePopup = "Do you want to set this tag for all children?";

		// Whenever showPopup bool is true, the popups keeps open
		if (showPopup)
			ImGui::OpenPopup(titlePopup.c_str());

		// PopUp
		if (ImGui::BeginPopupModal(titlePopup.c_str()))
		{
			if (ImGui::Button("YES"))
			{
				for (auto it : owner->children)
					it->tag = owner->tag;

				showPopup = false;
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("CANCEL"))
			{
				showPopup = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	return ret;
}

void C_Info::Save(Json& json) const
{
	json["type"] = (int)type;
}

void C_Info::Load(Json& json)
{
}