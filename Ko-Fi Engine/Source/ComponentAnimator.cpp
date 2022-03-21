#include "ComponentAnimator.h"

#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include <vector>
#include "Engine.h"
#include "Animation.h"
#include "AnimatorClip.h"

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
		
		ImGui::Text(rAnim->GetName().c_str());

		static char clipName[128] = "Enter Clip Name";
		ImGuiInputTextFlags inputTxtFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
		ImGui::InputText("Clip Name", clipName, IM_ARRAYSIZE(clipName), inputTxtFlags);

		ImGui::Text("Reel selector: ");
		ImGui::SliderInt("Edit Start", &rAnim->startPoint, 0, rAnim->duration);
		ImGui::SliderInt("Edit End", &rAnim->endPoint, 0, rAnim->duration);

		if (ImGui::Button("Create Clip", ImVec2(80, 35)))
		{
			CreateClip(AnimatorClip(rAnim, clipName, rAnim->startPoint, rAnim->endPoint, 1.0f, true));
		}
			

		ImGui::Text("Select Clip");


		if (ImGui::BeginCombo("Select Clip", ((selectedClip != nullptr) ? selectedClip->GetName().c_str() : "[SELECT CLIP]"), ImGuiComboFlags_None))
		{
			for (auto clip = clips.begin(); clip != clips.end(); ++clip)
			{
				if (ImGui::Selectable(clip->second.GetName().c_str(), (&clip->second == selectedClip), ImGuiSelectableFlags_None))
				{
					selectedClip = &clip->second;

					/*strcpy(editedName, selectedClip->GetName());
					editedStart = (int)selectedClip->GetStart();
					editedEnd = (int)selectedClip->GetEnd();
					editedSpeed = selectedClip->GetSpeed();
					editedLoop = selectedClip->IsLooped();

					editedMax = (selectedAnimation != nullptr) ? selectedAnimation->GetDuration() : 0;*/
				}
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

bool ComponentAnimator::CreateClip(const AnimatorClip& clip)
{
	if (clip.GetAnimation() == nullptr)
	{
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: Clip's R_Animation* was nullptr.", clip.GetName());
		return false;
	}
	if (clips.find(clip.GetName()) != clips.end())
	{ 
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: A clip with the same name already exists.", clip.GetName().c_str());
		return false;
	}

	clips.emplace(clip.GetName(), clip);
}

bool ComponentAnimator::CreateDefaultClip(const AnimatorClip& clip)
{
	if (clip.GetAnimation() == nullptr)
	{
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: Clip's R_Animation* was nullptr.", clip.GetName());
		return false;
	}
	if (clips.find(clip.GetName()) != clips.end())
	{
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: A clip with the same name already exists.", clip.GetName().c_str());
		return false;
	}

	clips.emplace(clip.GetName(), clip);

	selectedClip = (AnimatorClip*)&clip;
}

void ComponentAnimator::SetAnim(Animation* anim)
{
	if (this->rAnim != nullptr)
		RELEASE(this->rAnim);

	this->rAnim = anim;
}

AnimatorClip* ComponentAnimator::GetSelectedClip()
{
	return selectedClip;
}

void ComponentAnimator::SetSelectedClip(AnimatorClip& animatorClip)
{
	selectedClip = &animatorClip;
}
