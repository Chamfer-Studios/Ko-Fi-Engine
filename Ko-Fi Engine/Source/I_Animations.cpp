#include "I_Animations.h"
#include "Globals.h"
#include "Log.h"
#include "Animation.h"

I_Animations::I_Animations()
{
}

I_Animations::~I_Animations()
{
}

bool I_Animations::Import(const aiAnimation* aiAnimation, Animation* anim, const aiScene* assimpScene)
{
	if (anim == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Animation! Error: R_Anim* was nullptr.");
		return false;
	}
	if (aiAnimation == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Animation! Error: Animation* was nullptr.");
		return false;
	}

	anim->name = aiAnimation->mName.C_Str();
	anim->ticksPerSecond = aiAnimation->mTicksPerSecond;
	anim->duration = aiAnimation->mDuration;
	
	return true;
}

