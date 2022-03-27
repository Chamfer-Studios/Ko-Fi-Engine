#include "ComponentAnimator.h"

#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "Engine.h"
#include "R_Animation.h"
#include "AnimatorClip.h"

#include <vector>
#include <string>

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
		ImGui::SliderInt("Edit Start", &rAnim->startFrame, 0, rAnim->duration);
		ImGui::SliderInt("Edit End", &rAnim->endFrame, 0, rAnim->duration);

		if (ImGui::Button("Create Clip", ImVec2(80, 35)))
		{
			CreateClip(AnimatorClip(rAnim, clipName, rAnim->startFrame, rAnim->endFrame, 1.0f, true));
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

void ComponentAnimator::Save(Json& json) const
{
	json["type"] = "animator";

	Json jsonClips;
	for (auto clip : clips)
	{
		jsonClips["mapString"] = clip.first.c_str();
		jsonClips["clipName"] = clip.second.GetName().c_str();
		jsonClips["clipStartFrame"] = clip.second.GetStartFrame();
		jsonClips["clipEndFrame"] = clip.second.GetEndFrame();
		jsonClips["clipDuration"] = clip.second.GetDuration();

		json["clips"].push_back(jsonClips);
	}

	json["selectedClip"] = selectedClip->GetName();
}

void ComponentAnimator::Load(Json& json)
{
	if (selectedClip == nullptr)
		selectedClip = new AnimatorClip();

	if (!json.empty())
	{
		AnimatorClip animatorClip;
		for (const auto& clip : json.at("clips").items())
		{
			animatorClip.SetName(clip.value().at("clipName").get<std::string>().c_str());
			animatorClip.SetStartFrame(clip.value().at("clipStartFrame"));
			animatorClip.SetEndFrame(clip.value().at("clipEndFrame"));
			animatorClip.SetDuration(clip.value().at("clipDuration"));

			clips.emplace(clip.value().at("mapString"), animatorClip);
		}

		SetSelectedClip(json.at("selectedClip"));
	}
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

bool ComponentAnimator::CreateDefaultClip(AnimatorClip* clip)
{
	if (clip->GetAnimation() == nullptr)
	{
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: Clip's R_Animation* was nullptr.", clip->GetName());
		return false;
	}
	if (clips.find(clip->GetName()) != clips.end())
	{
		CONSOLE_LOG("[ERROR] Animator Component: Could not Add Clip { %s }! Error: A clip with the same name already exists.", clip->GetName().c_str());
		return false;
	}

	clips.emplace(clip->GetName(), *clip);

	selectedClip = clip;
}

void ComponentAnimator::SetAnim(R_Animation* anim)
{
	if (this->rAnim != nullptr)
		RELEASE(this->rAnim);

	this->rAnim = anim;
}

AnimatorClip* ComponentAnimator::GetSelectedClip()
{
	return selectedClip;
}

void ComponentAnimator::SetSelectedClip(std::string name)
{
	std::map<std::string, AnimatorClip>::iterator mapIt;
	for (auto clip = clips.begin(); clip != clips.end(); ++clip)
	{
		if ((*clip).first == name)
		{
			selectedClip = &clip->second;
		}
	}
}
