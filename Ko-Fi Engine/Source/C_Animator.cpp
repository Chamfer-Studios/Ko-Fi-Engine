#include "C_Animator.h"

// Modules
#include "Globals.h"
#include "Engine.h"
#include "Importer.h"
#include "M_ResourceManager.h"
#include "M_SceneManager.h"
#include "M_Renderer3D.h"
#include "M_Editor.h"

// GameObject
#include "GameObject.h"
#include "C_Mesh.h"
#include "C_Transform.h"

// Resources
#include "R_Animation.h"
#include "R_Mesh.h"

// Helpers
#include "FSDefs.h"
#include "Color.h"
#include "Channel.h"

// C/C++ Libraries
#include <vector>
#include <string>
#include <map>

// External libraries
#include "optick.h"

C_Animator::C_Animator(GameObject* parent) : Component(parent)
{
	type = ComponentType::ANIMATOR;
	typeIndex = typeid(this);

	animation = nullptr;

	createClipErrorMessage = false;
	deleteDefaultClipMessage = false;
}

C_Animator::~C_Animator()
{
	CleanUp();
}

bool C_Animator::Start()
{
	bool ret = false;

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
	if (!selectedClip)
	{
		// Creating a default clip with 10 keyframes.
		ret = CreateClip(AnimatorClip(animation, "Default clip", 0, 10, 1.0f, false));
		SetSelectedClip("Default clip");
		selectedClip->SetFinishedBool(true);
	}
	
	return ret;
}

bool C_Animator::PreUpdate()
{
	bool ret = true;

	M_SceneManager* sceneManager = owner->GetEngine()->GetSceneManager();
	GameState gameState = sceneManager->GetGameState();
	if ( gameState == GameState::PLAYING ||
		gameState == GameState::TICK)
		animTime += sceneManager->GetGameDt();

	return ret;
}

bool C_Animator::Update(float dt)
{
	return true;
}

bool C_Animator::PostUpdate()
{
	return true;
}

bool C_Animator::CleanUp()
{
	if (animation != nullptr)
		owner->GetEngine()->GetResourceManager()->FreeResource(animation->GetUID());

	for (auto it : meshesInfo)
	{
		owner->GetEngine()->GetResourceManager()->FreeResource(it.first->GetUID());
	}

	meshesInfo.clear();

	clips.clear();

	if (selectedClip)
		selectedClip = nullptr;

	if (clipToDelete)
		clipToDelete = nullptr;

	return true;
}

bool C_Animator::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Animator", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;

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
		int newStartFrame = animation->startFrame;
		if (ImGui::DragInt("Edit Start", &newStartFrame, 0, animation->duration))
			animation->startFrame = newStartFrame;
		int newEndFrame = animation->endFrame;
		if (ImGui::DragInt("Edit End", &newEndFrame, 0, animation->duration))
			animation->endFrame = newEndFrame;

		if (ImGui::Button("Create Clip ##", ImVec2(80, 35)))
		{
			if (animation->endFrame > animation->startFrame)
			{
				CreateClip(AnimatorClip(animation, clipName, animation->startFrame, animation->endFrame, 1.0f, false));
				createClipErrorMessage = false;
			}
			else
				createClipErrorMessage = true;
		}

		if (createClipErrorMessage)
			ImGui::TextColored(Red.ToImVec4(), "Please, select a valid clip interval.");

		ImGui::Text("Select Clip");
		if (ImGui::BeginCombo("Select Clip ##", ((selectedClip != nullptr) ? selectedClip->GetName().c_str() : "[SELECT CLIP]"), ImGuiComboFlags_None))
		{
			for (auto clip = clips.begin(); clip != clips.end(); ++clip)
			{
				ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

				if (ImGui::Selectable(clip->second.GetName().c_str(), (&clip->second == selectedClip), ImGuiSelectableFlags_None))
				{
					selectedClip = &clip->second;

					Reset();

					/*strcpy(editedName, selectedClip->GetName());
					editedStart = (int)selectedClip->GetStart();
					editedEnd = (int)selectedClip->GetEnd();
					editedSpeed = selectedClip->GetSpeed();
					editedLoop = selectedClip->IsLooped();

					editedMax = (selectedAnimation != nullptr) ? selectedAnimation->GetDuration() : 0;*/
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}

		ImGui::Text("Delete Clip");
		if (ImGui::BeginCombo("Delete Clip ##", ((clipToDelete != nullptr) ? clipToDelete->GetName().c_str() : "[DELETE CLIP]"), ImGuiComboFlags_None))
		{
			for (auto clip = clips.begin(); clip != clips.end(); ++clip)
			{
				ImGui::PushID(owner->GetEngine()->GetEditor()->idTracker++);

				if (ImGui::Selectable(clip->second.GetName().c_str(), (&clip->second == clipToDelete), ImGuiSelectableFlags_None))
				{
					if (clip->second.GetName() != "Default clip")
					{
						clipToDelete = &clip->second;
						deleteDefaultClipMessage = false;
					}
					else
						deleteDefaultClipMessage = true;
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}

		if (deleteDefaultClipMessage)
		{
			ImGui::TextColored(Red.ToImVec4(), "The default clip can't be deleted!");
			ImGui::TextColored(Red.ToImVec4(), "A clip is needed for the animation...");
		}

		if (ImGui::Button("Delete"))
		{
			if (clipToDelete == selectedClip)
				SetSelectedClip(std::string("Default clip"));

			if (clipToDelete)
			{
				clips.erase(clipToDelete->GetName().c_str());
				clipToDelete = nullptr;
			}
		}

		ImGui::Text("Clip Options: ");
		if (ImGui::Checkbox("Loop ##", &selectedClip->GetLoopBool())) {}

		//ImGui::SameLine();
		if (ImGui::Button("Restart", ImVec2(70, 18)))
			Reset();
	}
	else
		DrawDeleteButton(owner, this);

	return ret;
}

void C_Animator::Save(Json& json) const
{
	json["type"] = (int)type;

	if (animation != nullptr)
	{
		json["animation"]["uid"] = animation->GetUID();
		json["animation"]["asset_path"] = animation->GetAssetPath();

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

		Json jsonMeshes;
		for (auto mesh : meshesInfo)
		{
			jsonMeshes["uid"] = mesh.first->GetUID();
			jsonMeshes["asset_path"] = mesh.first->GetAssetPath();

			json["meshes"].push_back(jsonMeshes);
		}
	}
}

void C_Animator::Load(Json& json)
{
	if (!json.empty())
	{
		RELEASE(animation);

		Json jsonAnimation = json.at("animation");

		UID uid = jsonAnimation.at("uid");
		owner->GetEngine()->GetResourceManager()->LoadResource(uid, jsonAnimation.at("asset_path").get<std::string>().c_str());
		animation = (R_Animation*)owner->GetEngine()->GetResourceManager()->RequestResource(uid);

		if (animation == nullptr)
			KOFI_ERROR(" Component Animation: could not load resource from library.");
		else
		{
			for (const auto& clip : json.at("clips").items())
			{
				std::string key = clip.value().at("mapString");
				AnimatorClip c;
				c.SetName(clip.value().at("clipName").get<std::string>().c_str());
				c.SetStartFrame(clip.value().at("clipStartFrame"));
				c.SetEndFrame(clip.value().at("clipEndFrame"));
				c.SetDuration(clip.value().at("clipDuration"));
				c.SetDurationInSeconds(clip.value().at("clipDurationInSeconds"));
				c.SetLoopBool(clip.value().at("clipLoop"));
				c.SetFinishedBool(clip.value().at("clipFinished"));
				c.SetAnimation(animation);
				clips[key] = c;
			}
			SetSelectedClip(json.at("selectedClip"));
			selectedClip->SetFinishedBool(true);

			if (json.contains("meshes"))
			{
				for (const auto& mesh : json.at("meshes").items())
				{
					UID uid = mesh.value().at("uid");
					std::string assetPath = mesh.value().at("asset_path").get<std::string>();
					owner->GetEngine()->GetResourceManager()->LoadResource(uid, assetPath.c_str());
					R_Mesh* rMesh = (R_Mesh*)owner->GetEngine()->GetResourceManager()->RequestResource(uid);
					SetMeshInfo(rMesh);
				}
			}
		}
	}
}

bool C_Animator::CreateClip(const AnimatorClip& clip)
{
	if (clip.GetAnimation() == nullptr)
	{
		KOFI_ERROR(" Animator Component: Could not Add Clip { %s }! Error: Clip's R_Animation* was nullptr.", clip.GetName());
		return false;
	}
	if (clips.find(clip.GetName()) != clips.end())
	{ 
		KOFI_ERROR(" Animator Component: Could not Add Clip { %s }! Error: A clip with the same name already exists.", clip.GetName().c_str());
		return false;
	}

	clips.emplace(clip.GetName(), clip);

	return true;
}

void C_Animator::Reset()
{
	// Set the animation to the initial time value
	animTime = 0.0f;

	selectedClip->SetFinishedBool(false);
}

void C_Animator::SetAnimation(R_Animation* anim)
{
	if (this->animation != nullptr)
	{
		owner->GetEngine()->GetResourceManager()->FreeResource(this->animation->GetUID());
		this->animation = nullptr;
	}

	this->animation = anim;
}

void C_Animator::SetMeshInfo(R_Mesh* mesh)
{
	meshesInfo.emplace(mesh, MeshInfo(mesh->boneInfo, std::vector<float4x4>()));
}

AnimatorClip* C_Animator::GetSelectedClip()
{
	return selectedClip;
}

std::string C_Animator::GetSelectedClipName()
{
	return selectedClip->GetName();
}

bool C_Animator::IsCurrentClipPlaying()
{
	return !GetSelectedClip()->GetFinishedBool();
}

bool C_Animator::IsCurrentClipLooping()
{
	return GetSelectedClip()->GetLoopBool();
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

	Reset();
}

void C_Animator::GetBoneTransforms(float timeInSeconds, std::vector<float4x4>& transforms, GameObject* gameObject)
{
	OPTICK_EVENT();

	// REVISE!
	// This two variables should not be there. Manage them the proper way when possible.
	//----------------------------------------------------------------------------------------------------
	rootNode = gameObject->GetParent();
	mesh = gameObject->GetComponent<C_Mesh>()->GetMesh();
	//----------------------------------------------------------------------------------------------------

	if (meshesInfo.find(mesh) == meshesInfo.end())
		return;

	if (!owner->GetEngine()->GetRenderer()->isFirstPass)
	{
		transforms.resize(meshesInfo.find(mesh)->second.transformsAnim.size());

		for (uint i = 0; i < meshesInfo.find(mesh)->second.transformsAnim.size(); i++)
		{
			transforms[i] = meshesInfo.find(mesh)->second.transformsAnim[i];
		}

		return;
	}

	float4x4 identity = float4x4::identity;

	float ticksPerSecond = (float)(animation->GetTicksPerSecond() != 0 ? animation->GetTicksPerSecond() : 25.0f);
	float timeInTicks = timeInSeconds * ticksPerSecond;

	float startFrame, endFrame, animDur;
	if (selectedClip != nullptr)
	{
		startFrame = selectedClip->GetStartFrame();
		endFrame = selectedClip->GetEndFrame();
		animDur = endFrame - startFrame;
	}
	else
	{
		startFrame = 0.0f;
		endFrame = 0.0f;
		animDur = animation->GetDuration();
	}

	float animationTimeTicks = fmod(timeInTicks, (float)animDur); // This divides the whole animation into segments of animDur.

	// Checking if the animation has finished (the animation time ticks is equal to the duration time ticks).
	float animationSeconds = fmod(timeInSeconds, (float)selectedClip->GetDurationInSeconds());
	if ((selectedClip->GetDurationInSeconds() - animationSeconds) <= 0.1f && timeInSeconds != 0.0f)
	{
		if (!selectedClip->GetLoopBool())
			selectedClip->SetFinishedBool(true);
	}

	ReadNodeHeirarchy(animationTimeTicks + startFrame, gameObject->GetParent(), identity); // We add startFrame as an offset to the duration.
	transforms.resize(meshesInfo.find(mesh)->second.boneInfo.size());
	meshesInfo.find(mesh)->second.transformsAnim.resize(meshesInfo.find(mesh)->second.boneInfo.size());

	for (uint i = 0; i < meshesInfo.find(mesh)->second.boneInfo.size(); i++)
	{
		transforms[i] = meshesInfo.find(mesh)->second.boneInfo[i].finalTransformation;
		meshesInfo.find(mesh)->second.transformsAnim[i] = meshesInfo.find(mesh)->second.boneInfo[i].finalTransformation;
	}
}

void C_Animator::ReadNodeHeirarchy(float animationTimeTicks, const GameObject* pNode, const float4x4& parentTransform)
{
	OPTICK_EVENT();

	if (pNode->GetComponent<C_Mesh>() != nullptr)
		return;

	std::string nodeName = pNode->GetName();
	std::size_t start = nodeName.find_last_of("(");
	if (start != std::string::npos)
		nodeName = nodeName.substr(0, start - 1);

	float4x4 nodeTransformation(pNode->GetTransform()->GetLocalTransform());

	const Channel* pNodeAnim = FindNodeAnim(nodeName);

	if (pNodeAnim && pNodeAnim->rotationKeyframes.size() && pNodeAnim->scaleKeyframes.size() && pNodeAnim->positionKeyframes.size())
	{
		// Interpolate scaling and generate scaling transformation matrix
		float3 scaling;
		CalcInterpolatedScaling(scaling, animationTimeTicks, pNodeAnim);
		float4x4 scalingM = InitScaleTransform(scaling.x, scaling.y, scaling.z);

		// Interpolate rotation and generate rotation transformation matrix
		Quat rotationQ;
		CalcInterpolatedRotation(rotationQ, animationTimeTicks, pNodeAnim);
		float4x4 rotationM = GetMatrixFromQuat(rotationQ).Transposed();

		// Interpolate translation and generate translation transformation matrix
		float3 translation;
		CalcInterpolatedPosition(translation, animationTimeTicks, pNodeAnim);
		float4x4 translationM = InitTranslationTransform(translation.x, translation.y, translation.z);

		// Combine the above transformations
		nodeTransformation = translationM * rotationM * scalingM;
	}

	float4x4 globalTransformation = parentTransform * nodeTransformation;

	if (mesh->boneNameToIndexMap.contains(nodeName))
	{
		uint boneIndex = mesh->boneNameToIndexMap[nodeName];
		float4x4 globalInversedTransform = rootNode->GetTransform()->GetGlobalTransform().Inverted();
		float4x4 delta = globalInversedTransform * globalTransformation * meshesInfo.find(mesh)->second.boneInfo[boneIndex].offsetMatrix;
		meshesInfo.find(mesh)->second.boneInfo[boneIndex].finalTransformation = delta.Transposed();
	}

	for (uint i = 0; i < pNode->GetChildren().size(); i++)
	{
		ReadNodeHeirarchy(animationTimeTicks, pNode->GetChildren().at(i), globalTransformation);
	}
}

const Channel* C_Animator::FindNodeAnim(std::string nodeName)
{
	OPTICK_EVENT();

	return &animation->channels[nodeName];
}

const std::vector<float4x4> C_Animator::GetLastBoneTransforms(R_Mesh* mesh) const
{
	return meshesInfo.find(mesh)->second.transformsAnim;
}

uint C_Animator::FindPosition(float AnimationTimeTicks, const Channel* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->positionKeyframes.size() - 1; i++) {
		float t = (float)pNodeAnim->positionKeyframes.at(i + 1).time;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void C_Animator::CalcInterpolatedPosition(float3& Out, float AnimationTimeTicks, const Channel* pNodeAnim)
{
	OPTICK_EVENT();

	// we need at least two values to interpolate...
	if (pNodeAnim->positionKeyframes.size() == 1)
	{
		Out = pNodeAnim->positionKeyframes.at(0).value;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTimeTicks, pNodeAnim);
	uint NextPositionIndex = PositionIndex + 1;
	assert(NextPositionIndex < pNodeAnim->positionKeyframes.size());
	float t1 = (float)pNodeAnim->positionKeyframes.at(PositionIndex).time;
	float t2 = (float)pNodeAnim->positionKeyframes.at(NextPositionIndex).time;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const float3& Start = pNodeAnim->positionKeyframes.at(PositionIndex).value;
	const float3& End = pNodeAnim->positionKeyframes.at(NextPositionIndex).value;
	float3 Delta = End - Start;
	Out = Start + Factor * Delta;
}

uint C_Animator::FindRotation(float AnimationTimeTicks, const Channel* pNodeAnim)
{
	assert(pNodeAnim->rotationKeyframes.size() > 0);

	for (uint i = 0; i < pNodeAnim->rotationKeyframes.size() - 1; i++) {
		float t = (float)pNodeAnim->rotationKeyframes.at(i + 1).time;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void C_Animator::CalcInterpolatedRotation(Quat& Out, float AnimationTimeTicks, const Channel* pNodeAnim)
{
	OPTICK_EVENT();

	// we need at least two values to interpolate...
	if (pNodeAnim->rotationKeyframes.size() == 1) {
		Out = pNodeAnim->rotationKeyframes.at(0).value;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTimeTicks, pNodeAnim);
	uint NextRotationIndex = RotationIndex + 1;
	assert(NextRotationIndex < pNodeAnim->rotationKeyframes.size());
	float t1 = (float)pNodeAnim->rotationKeyframes.at(RotationIndex).time;
	float t2 = (float)pNodeAnim->rotationKeyframes.at(NextRotationIndex).time;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const Quat& StartRotationQ = pNodeAnim->rotationKeyframes.at(RotationIndex).value;
	const Quat& EndRotationQ = pNodeAnim->rotationKeyframes.at(NextRotationIndex).value;
	StartRotationQ.Slerp(EndRotationQ, Factor);
	Out = StartRotationQ;
	Out.Normalize();
}


uint C_Animator::FindScaling(float AnimationTimeTicks, const Channel* pNodeAnim)
{
	assert(pNodeAnim->scaleKeyframes.size() > 0);

	for (uint i = 0; i < pNodeAnim->scaleKeyframes.size() - 1; i++) {
		float t = (float)pNodeAnim->scaleKeyframes.at(i + 1).time;
		if (AnimationTimeTicks < t) {
			return i;
		}
	}

	return 0;
}

void C_Animator::CalcInterpolatedScaling(float3& Out, float AnimationTimeTicks, const Channel* pNodeAnim)
{
	OPTICK_EVENT();

	// we need at least two values to interpolate...
	if (pNodeAnim->scaleKeyframes.size() == 1) {
		Out = pNodeAnim->scaleKeyframes.at(0).value;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTimeTicks, pNodeAnim);
	uint NextScalingIndex = ScalingIndex + 1;
	assert(NextScalingIndex < pNodeAnim->scaleKeyframes.size());
	float t1 = (float)pNodeAnim->scaleKeyframes.at(ScalingIndex).time;
	float t2 = (float)pNodeAnim->scaleKeyframes.at(NextScalingIndex).time;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTimeTicks - (float)t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const float3& Start = pNodeAnim->scaleKeyframes.at(ScalingIndex).value;
	const float3& End = pNodeAnim->scaleKeyframes.at(NextScalingIndex).value;
	float3 Delta = End - Start;
	Out = Start + Factor * Delta;
}

float4x4 C_Animator::InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ)
{
	float4x4 m;

	m[0][0] = ScaleX; m[0][1] = 0.0f;   m[0][2] = 0.0f;   m[0][3] = 0.0f;
	m[1][0] = 0.0f;   m[1][1] = ScaleY; m[1][2] = 0.0f;   m[1][3] = 0.0f;
	m[2][0] = 0.0f;   m[2][1] = 0.0f;   m[2][2] = ScaleZ; m[2][3] = 0.0f;
	m[3][0] = 0.0f;   m[3][1] = 0.0f;   m[3][2] = 0.0f;   m[3][3] = 1.0f;

	return m;
}

float4x4 C_Animator::InitRotateTransform(const aiQuaternion& quat)
{
	float4x4 m;

	float yy2 = 2.0f * quat.y * quat.y;
	float xy2 = 2.0f * quat.x * quat.y;
	float xz2 = 2.0f * quat.x * quat.z;
	float yz2 = 2.0f * quat.y * quat.z;
	float zz2 = 2.0f * quat.z * quat.z;
	float wz2 = 2.0f * quat.w * quat.z;
	float wy2 = 2.0f * quat.w * quat.y;
	float wx2 = 2.0f * quat.w * quat.x;
	float xx2 = 2.0f * quat.x * quat.x;
	m[0][0] = -yy2 - zz2 + 1.0f;
	m[0][1] = xy2 + wz2;
	m[0][2] = xz2 - wy2;
	m[0][3] = 0;
	m[1][0] = xy2 - wz2;
	m[1][1] = -xx2 - zz2 + 1.0f;
	m[1][2] = yz2 + wx2;
	m[1][3] = 0;
	m[2][0] = xz2 + wy2;
	m[2][1] = yz2 - wx2;
	m[2][2] = -xx2 - yy2 + 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1.0f;

	return m;
}

float4x4 C_Animator::InitTranslationTransform(float x, float y, float z)
{
	float4x4 m;

	m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = x;
	m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = y;
	m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = z;
	m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;

	return m;
}

float4x4 C_Animator::GetMatrixFromQuat(Quat quat)
{
	float4x4 m;

	float yy2 = 2.0f * quat.y * quat.y;
	float xy2 = 2.0f * quat.x * quat.y;
	float xz2 = 2.0f * quat.x * quat.z;
	float yz2 = 2.0f * quat.y * quat.z;
	float zz2 = 2.0f * quat.z * quat.z;
	float wz2 = 2.0f * quat.w * quat.z;
	float wy2 = 2.0f * quat.w * quat.y;
	float wx2 = 2.0f * quat.w * quat.x;
	float xx2 = 2.0f * quat.x * quat.x;
	m[0][0] = -yy2 - zz2 + 1.0f;
	m[0][1] = xy2 + wz2;
	m[0][2] = xz2 - wy2;
	m[0][3] = 0;
	m[1][0] = xy2 - wz2;
	m[1][1] = -xx2 - zz2 + 1.0f;
	m[1][2] = yz2 + wx2;
	m[1][3] = 0;
	m[2][0] = xz2 + wy2;
	m[2][1] = yz2 - wx2;
	m[2][2] = -xx2 - yy2 + 1.0f;
	m[2][3] = 0.0f;
	m[3][0] = m[3][1] = m[3][2] = 0;
	m[3][3] = 1.0f;

	return m;
}