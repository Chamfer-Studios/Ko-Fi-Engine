#include "C_Animator.h"

// Modules
#include "Globals.h"
#include "Engine.h"
#include "Importer.h"

// GameObject
#include "GameObject.h"
#include "C_Mesh.h"

// Resources
#include "R_Animation.h"
#include "AnimatorClip.h"

#include "FSDefs.h"
#include "Color.h"

#include <vector>
#include <string>
#include <map>

C_Animator::C_Animator(GameObject* parent) : Component(parent)
{
	type = ComponentType::ANIMATOR;
}

C_Animator::~C_Animator()
{
}

bool C_Animator::Start()
{
	// Setting default animation values for a GameObject without animation.
	if (animation == nullptr)
	{
		animation = new R_Animation();
		animation->SetName("Default animation");
		animation->SetDuration(10);
		animation->SetTicksPerSecond(24);
		animation->SetStartFrame(0);
		animation->SetEndFrame(10);
	}
	if (selectedClip == nullptr)
	{
		AnimatorClip* animClip = new AnimatorClip(animation, "Default clip", 0, 10, 1.0f, true);
		CreateDefaultClip(animClip);
	}
	
	return true;
}

bool C_Animator::Update(float dt)
{
	return true;
}

bool C_Animator::CleanUp()
{
	return true;
}

bool C_Animator::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		ImGui::Text("Current Time: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		//if (ImGui::Selectable(mesh->path.c_str())) {}
		ImGui::PopStyleColor();

		ImGui::Text("Number of ticks: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", 22);

		/*ImGui::Text("Animation State: ");
		if (ImGui::Checkbox("Playing", &playing))
			if(playing == true){}
			else{}*/

		// -- CLIP CREATOR
		ImGui::Text("Select Animation");
		
		ImGui::Text(animation->GetName().c_str());

		static char clipName[128] = "Enter Clip Name";
		ImGuiInputTextFlags inputTxtFlags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll;
		ImGui::InputText("Clip Name", clipName, IM_ARRAYSIZE(clipName), inputTxtFlags);

		ImGui::Text("Reel selector: ");
		ImGui::DragInt("Edit Start", &animation->startFrame, 0, animation->duration);
		ImGui::DragInt("Edit End", &animation->endFrame, 0, animation->duration);

		if (ImGui::Button("Create Clip", ImVec2(80, 35)))
		{
			if (animation->endFrame > animation->startFrame)
			{
				CreateClip(AnimatorClip(animation, clipName, animation->startFrame, animation->endFrame, 1.0f, true));
				createClipErrorMessage = false;
			}
			else
				createClipErrorMessage = true;
		}

		if (createClipErrorMessage)
			ImGui::TextColored(Red.ToImVec4(), "Please, select a valid clip interval.");

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

		if (ImGui::BeginCombo("Delete Clip", ((clipToDelete != nullptr) ? clipToDelete->GetName().c_str() : "[DELETE CLIP]"), ImGuiComboFlags_None))
		{
			for (auto clip = clips.begin(); clip != clips.end(); ++clip)
			{
				if (ImGui::Selectable(clip->second.GetName().c_str(), (&clip->second == clipToDelete), ImGuiSelectableFlags_None))
				{
					clipToDelete = &clip->second;
				}
			}

			ImGui::EndCombo();
		}

		if (ImGui::Button("Delete"))
		{
			clips.erase(clipToDelete->GetName().c_str());
			clipToDelete = nullptr;
		}

		ImGui::Text("Clip Options: ");
		if (ImGui::Checkbox("Loop", &selectedClip->GetLoopBool())) {}

		/*ImGui::SameLine();
		if (ImGui::Button("Restart", ImVec2(70, 18)))
			Reset();*/
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

void C_Animator::Save(Json& json) const
{
	json["type"] = "animator";

	std::string name = owner->GetName();
	animation->path = ANIMATIONS_DIR + name + ANIMATION_EXTENSION;
	Importer::GetInstance()->animationImporter->Save(animation, animation->path.c_str());

	json["path"] = animation->path;
	Json jsonClips;
	for (auto clip : clips)
	{
		jsonClips["mapString"] = clip.first.c_str();

		jsonClips["clipName"] = clip.second.GetName().c_str();
		jsonClips["clipStartFrame"] = clip.second.GetStartFrame();
		jsonClips["clipEndFrame"] = clip.second.GetEndFrame();
		jsonClips["clipDuration"] = clip.second.GetDuration();
		jsonClips["clipDurationInSeconds"] = clip.second.GetDurationInSeconds();
		jsonClips["clipLoop"] = clip.second.GetLoopBool();
		jsonClips["clipFinished"] = clip.second.GetFinishedBool();

		json["clips"].push_back(jsonClips);
	}
	json["selectedClip"] = selectedClip->GetName();
}

void C_Animator::Load(Json& json)
{
	if (animation)
		RELEASE(animation);
	animation = new R_Animation();

	if (json.contains("path"))
	{
		std::string path = json.at("path");
		Importer::GetInstance()->animationImporter->Load(path.c_str(), animation);
		C_Mesh* cMesh = owner->GetComponent<C_Mesh>();

		if (cMesh != nullptr && cMesh->GetMesh()->IsAnimated())
			owner->GetComponent<C_Mesh>()->GetMesh()->SetAnimation(animation);

		if (!json.empty())
		{
			AnimatorClip animatorClip;
			for (const auto& clip : json.at("clips").items())
			{
				animatorClip.SetName(clip.value().at("clipName").get<std::string>().c_str());
				animatorClip.SetStartFrame(clip.value().at("clipStartFrame"));
				animatorClip.SetEndFrame(clip.value().at("clipEndFrame"));
				animatorClip.SetDuration(clip.value().at("clipDuration"));
				animatorClip.SetDurationInSeconds(clip.value().at("clipDurationInSeconds"));
				animatorClip.SetLoopBool(clip.value().at("clipLoop"));
				animatorClip.SetFinishedBool(clip.value().at("clipFinished"));

				animatorClip.SetAnimation(animation);

				clips[clip.value().at("mapString")] = animatorClip;
			}
			SetSelectedClip(json.at("selectedClip"));
		}
	}

	/*if (selectedClip)
	{
		selectedClip = nullptr;
	}*/

	/*if (!selectedClip)
	{*/
		/*selectedClip = new AnimatorClip();*/
	/*}*/
}

void C_Animator::Reset()
{
	//set the animation to the initial time value
}

bool C_Animator::CreateClip(const AnimatorClip& clip)
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

bool C_Animator::CreateDefaultClip(AnimatorClip* clip)
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

void C_Animator::SetAnim(R_Animation* anim)
{
	if (this->animation != nullptr)
		RELEASE(this->animation);

	this->animation = anim;
}

AnimatorClip* C_Animator::GetSelectedClip()
{
	return selectedClip;
}

void C_Animator::SetSelectedClip(std::string name)
{
	for (std::map<std::string, AnimatorClip>::iterator clip = clips.begin(); clip != clips.end(); ++clip)
	{
		if ((*clip).first == name)
		{
			selectedClip = &clip->second;
			break;
		}
	}
}
