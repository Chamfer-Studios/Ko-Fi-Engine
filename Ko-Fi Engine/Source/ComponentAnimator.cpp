#include "ComponentAnimator.h"

#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include <vector>
#include "Engine.h"

ComponentAnimator::ComponentAnimator(GameObject* parent) : Component(parent)
{
	type = ComponentType::ANIMATOR;
	playing = true;

}

ComponentAnimator::~ComponentAnimator()
{
}

bool ComponentAnimator::Start()
{
	return true;
}

bool ComponentAnimator::Update(float dt)
{
	return true;
}

bool ComponentAnimator::CleanUp()
{
	return true;
}

bool ComponentAnimator::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Animator"))
	{
		ImGui::Text("Current Time: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		//if (ImGui::Selectable(mesh->path.c_str())) {}
		ImGui::PopStyleColor();

		ImGui::Text("Number of ticks: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", 22);

		ImGui::Text("Animation State: ");
		if (ImGui::Checkbox("Playing", &playing))
			if(playing == true){}
			else{}

		// -- CLIP CREATOR
		ImGui::Text("Select Animation");

		if (ImGui::BeginCombo(" ", "[SELECT ANIMATION]", ImGuiComboFlags_None))
		{
			//for (uint i = 0; i < animations->size(); ++i)
			{
				//R_Animation* rAnimation = animations->at(i);
				//if (rAnimation == nullptr)
					//continue;

				/*if (ImGui::Selectable(rAnimation->GetName(), (rAnimation == selectedAnimation), ImGuiSelectableFlags_None))
				{
					selectedAnimation = rAnimation;

					newClipEnd = selectedAnimation->GetDuration();
					newClipMax = selectedAnimation->GetDuration();
				}*/
			}

			ImGui::EndCombo();
		}

		int value = 5;
		ImGui::Text("Reel selector: ");
		ImGui::SliderInt("Edit Start", &value, 0, 10);
		ImGui::SliderInt("Edit End", &value, 0, 10);
		if (ImGui::Button("Create Clip", ImVec2(80, 35)))
			Save();

		ImGui::Text("Select Clip");

		if (ImGui::BeginCombo(" ", "[SELECT CLIP]", ImGuiComboFlags_None))
		{
			//for (uint i = 0; i < animations->size(); ++i)
			{
				//R_Animation* rAnimation = animations->at(i);
				//if (rAnimation == nullptr)
					//continue;

				/*if (ImGui::Selectable(rAnimation->GetName(), (rAnimation == selectedAnimation), ImGuiSelectableFlags_None))
				{
					selectedAnimation = rAnimation;

					newClipEnd = selectedAnimation->GetDuration();
					newClipMax = selectedAnimation->GetDuration();
				}*/
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Delete Clip");

		if (ImGui::BeginCombo(" ", "[DELETE CLIP]", ImGuiComboFlags_None))
		{
			//for (uint i = 0; i < animations->size(); ++i)
			{
				//R_Animation* rAnimation = animations->at(i);
				//if (rAnimation == nullptr)
					//continue;

				/*if (ImGui::Selectable(rAnimation->GetName(), (rAnimation == selectedAnimation), ImGuiSelectableFlags_None))
				{
					selectedAnimation = rAnimation;

					newClipEnd = selectedAnimation->GetDuration();
					newClipMax = selectedAnimation->GetDuration();
				}*/
			}

			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Button("Delete", ImVec2(50, 30));

		ImGui::Text("Clip Options: ");
		if (ImGui::Checkbox("Loop", &playing))
			if (playing == true) {}
			else {}
		ImGui::SameLine();
		if (ImGui::Button("Restart", ImVec2(70, 18)))
			Reset();

	}
	return ret;
}

void ComponentAnimator::Reset()
{
	//set the animation to the initial time value
}

void ComponentAnimator::Save()
{
	//set the initial and final animation internal time value through which the animation will loop 
	//use the values of the slider above
}
