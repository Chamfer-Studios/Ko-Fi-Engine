#ifndef __C_ANIMATOR_H__
#define __C_ANIMATOR_H__

#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"
#include "AnimatorClip.h"
#include "Assimp.h"

#include <string>

class GameObject;
class R_Animation;
class R_Mesh;
class map;
struct Channel;
struct BoneInfo;

struct MeshInfo
{
	MeshInfo(std::vector<BoneInfo> boneInfo, std::vector<float4x4> transformsAnim) : boneInfo(boneInfo), transformsAnim(transformsAnim) {}
	~MeshInfo() {}
	std::vector<BoneInfo> boneInfo;
	std::vector<float4x4> transformsAnim;
};

class C_Animator : public Component
{
public:
	C_Animator(GameObject* parent);
	virtual ~C_Animator();

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	bool InspectorDraw(PanelChooser* chooser);

	void Save(Json& json) const override;
	void Load(Json& json) override;

	void Reset();

	void SetAnimation(R_Animation* anim);
	void SetMeshInfo(R_Mesh* mesh);

	bool CreateClip(const AnimatorClip& clip);
	void SetSelectedClip(std::string name);
	AnimatorClip* GetSelectedClip();
	std::string GetSelectedClipName();

	bool IsCurrentClipPlaying();
	bool IsCurrentClipLooping();

	const float inline const GetAnimTime() const { return animTime; };

	// Bone transforms
	//----------------------------------------------------------------------------------------------------
	void GetBoneTransforms(float timeInSeconds, std::vector<float4x4>& transforms, GameObject* gameObject);
	void ReadNodeHeirarchy(float animationTimeTicks, const GameObject* rootNode, const float4x4& parentTransform);
	const Channel* FindNodeAnim(std::string nodeName);

	const std::vector<float4x4> GetLastBoneTransforms(R_Mesh* mesh) const;
	const inline std::map<R_Mesh*, MeshInfo> GetMeshesInfo() { return meshesInfo; }

	uint FindPosition(float AnimationTimeTicks, const Channel* pNodeAnim);
	void CalcInterpolatedPosition(float3& Out, float AnimationTimeTicks, const Channel* pNodeAnim);

	uint FindRotation(float AnimationTimeTicks, const Channel* pNodeAnim);
	void CalcInterpolatedRotation(Quat& Out, float AnimationTimeTicks, const Channel* pNodeAnim);

	uint FindScaling(float AnimationTimeTicks, const Channel* pNodeAnim);
	void CalcInterpolatedScaling(float3& Out, float AnimationTimeTicks, const Channel* pNodeAnim);

	float4x4 InitScaleTransform(float ScaleX, float ScaleY, float ScaleZ);
	float4x4 InitRotateTransform(const aiQuaternion& quat);
	float4x4 InitTranslationTransform(float x, float y, float z);

	float4x4 GetMatrixFromQuat(Quat quat);
	//----------------------------------------------------------------------------------------------------

public:
	R_Animation* animation = nullptr;

private:
	std::map<std::string, AnimatorClip> clips;
	AnimatorClip* selectedClip = nullptr;
	AnimatorClip* clipToDelete = nullptr;

	bool createClipErrorMessage = false;
	bool deleteDefaultClipMessage = false;

	float animTime = 0.0f; // Total animation time

	// Bone transforms
	std::map<R_Mesh*, MeshInfo> meshesInfo;

	// This two variables should not be there. Manage them the proper way when possible.
	const GameObject* rootNode = nullptr;
	R_Mesh* mesh = nullptr;
};

#endif // !__C_ANIMATOR_H__
