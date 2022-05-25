#ifndef __C_ANIMATOR_H__
#define __C_ANIMATOR_H__

#include "Component.h"
#include "MathGeoLib/Math/float4x4.h"
#include "AnimatorClip.h"

class GameObject;
class R_Animation;
class map;

class C_Animator : public Component
{
public:
	C_Animator(GameObject* parent);
	~C_Animator();

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

	bool CreateClip(const AnimatorClip& clip);
	void SetSelectedClip(std::string name);
	AnimatorClip* GetSelectedClip();

	bool IsCurrentClipPlaying();
	bool IsCurrentClipLooping();

	const float inline const GetAnimTime() const { return animTime; };

	// Bone transforms
	//----------------------------------------------------------------------------------------------------
	void GetBoneTransforms(float timeInSeconds, std::vector<float4x4>& transforms, GameObject* gameObject);
	void ReadNodeHeirarchy(float animationTimeTicks, const GameObject* rootNode, const float4x4& parentTransform);

	const Channel* FindNodeAnim(std::string nodeName);
	inline const std::vector<float4x4> GetLastBoneTransforms() const { return transformsAnim; }

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
	const GameObject* rootNode = nullptr;
	std::vector<float4x4> transformsAnim;
};

#endif // !__C_ANIMATOR_H__
