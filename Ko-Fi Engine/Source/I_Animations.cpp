#include "I_Animations.h"
#include "Globals.h"
#include "Log.h"
#include "R_Animation.h"
#include "Channel.h"

#include <map>

I_Animations::I_Animations()
{
}

I_Animations::~I_Animations()
{
}

bool I_Animations::Import(const aiAnimation* assimpAnimation, R_Animation* rAnimation)
{
	if (rAnimation == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Animation! Error: R_Anim* was nullptr.");
		return false;
	}
	if (assimpAnimation == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Animation! Error: Animation* was nullptr.");
		return false;
	}

	rAnimation->SetName(assimpAnimation->mName.C_Str());
	rAnimation->SetDuration(assimpAnimation->mDuration);
	rAnimation->SetTicksPerSecond(assimpAnimation->mTicksPerSecond);
	rAnimation->SetStartFrame(0);
	rAnimation->SetEndFrame(assimpAnimation->mDuration);

	std::multimap<std::string, Channel> tmp;

	for (uint i = 0; i < assimpAnimation->mNumChannels; ++i)
	{
		aiNodeAnim* aiChannel = assimpAnimation->mChannels[i];

		Channel rChannel = Channel(aiChannel->mNodeName.C_Str());
		GetPositionKeys(aiChannel, rChannel);
		GetRotationKeys(aiChannel, rChannel);
		GetScaleKeys(aiChannel, rChannel);

		ValidateChannel(rChannel);

		auto existingChannel = tmp.find(rChannel.name);											// There can exists multiple channels with the same name. Fuse them as done with Transforms.
		if (existingChannel != tmp.end())
		{
			FuseChannels(rChannel, existingChannel->second);
		}
		else
		{
			tmp.emplace(rChannel.name, rChannel);
		}
	}

	for (auto channel = tmp.begin(); channel != tmp.end(); ++channel)
	{
		rAnimation->channels.push_back(channel->second);
	}

	tmp.clear();
	
	return true;
}

void I_Animations::GetPositionKeys(const aiNodeAnim* aiChannel, Channel& rChannel)
{
	for (uint i = 0; i < aiChannel->mNumPositionKeys; ++i)
	{
		aiVectorKey pk = aiChannel->mPositionKeys[i];

		double time = pk.mTime;
		float3 position = float3(pk.mValue.x, pk.mValue.y, pk.mValue.z);

		rChannel.positionKeyframes.push_back(PositionKeyframe(time, position));
	}
}

void I_Animations::GetRotationKeys(const aiNodeAnim* aiChannel, Channel& rChannel)
{
	for (uint i = 0; i < aiChannel->mNumRotationKeys; ++i)
	{
		aiQuatKey rk = aiChannel->mRotationKeys[i];

		double time = rk.mTime;
		Quat rotation = Quat(rk.mValue.x, rk.mValue.y, rk.mValue.z, rk.mValue.w);

		rChannel.rotationKeyframes.push_back(RotationKeyframe(time, rotation));
	}
}

void I_Animations::GetScaleKeys(const aiNodeAnim* aiChannel, Channel& rChannel)
{
	for (uint i = 0; i < aiChannel->mNumScalingKeys; ++i)
	{
		aiVectorKey sk = aiChannel->mScalingKeys[i];

		double time = sk.mTime;
		float3 scale = float3(sk.mValue.x, sk.mValue.y, sk.mValue.z);

		rChannel.scaleKeyframes.push_back(ScaleKeyframe(time, scale));
	}
}

void I_Animations::ValidateChannel(Channel& rChannel)
{
	if (strstr(rChannel.name.c_str(), "_$AssimpFbx$_") != nullptr)
	{
		if (strstr(rChannel.name.c_str(), "_$AssimpFbx$_Translation") != nullptr)
		{
			rChannel.rotationKeyframes.clear();
			rChannel.rotationKeyframes.push_back(RotationKeyframe(-1.0, Quat::identity));

			rChannel.scaleKeyframes.clear();
			rChannel.scaleKeyframes.push_back(ScaleKeyframe(-1.0, float3::zero));

		}
		else if (strstr(rChannel.name.c_str(), "_$AssimpFbx$_Rotation") != nullptr)
		{
			rChannel.positionKeyframes.clear();
			rChannel.positionKeyframes.push_back(PositionKeyframe(-1.0, float3::zero));

			rChannel.scaleKeyframes.clear();
			rChannel.scaleKeyframes.push_back(ScaleKeyframe(-1.0, float3::zero));
		}
		else if (strstr(rChannel.name.c_str(), "_$AssimpFbx$_Scaling") != nullptr)
		{
			rChannel.positionKeyframes.clear();
			rChannel.positionKeyframes.push_back(PositionKeyframe(-1.0, float3::zero));

			rChannel.rotationKeyframes.clear();
			rChannel.rotationKeyframes.push_back(RotationKeyframe(-1.0, Quat::identity));
		}

		uint pos = rChannel.name.find_first_of("$");
		if (pos != rChannel.name.npos)
		{
			rChannel.name = rChannel.name.substr(0, pos - 1);
		}
	}
}

void I_Animations::FuseChannels(const Channel& newChannel, Channel& existingChannel)
{
	if (newChannel.HasPositionKeyframes())
	{
		if (!existingChannel.HasPositionKeyframes())
		{
			existingChannel.positionKeyframes.clear();	// Erasing the [-1.0] item that identifies an invalid keyframe channel.
		}

		uint i = 0;
		for (auto pk = newChannel.positionKeyframes.begin(); pk != newChannel.positionKeyframes.end(); ++pk)
		{
			//existingChannel.positionKeyframes[pk->first] = pk->second;
			existingChannel.positionKeyframes.at(i).value = pk->value;
			i++;
		}
	}
	if (newChannel.HasRotationKeyframes())
	{
		if (!existingChannel.HasRotationKeyframes())
		{
			existingChannel.rotationKeyframes.clear();
		}

		uint i = 0;
		for (auto rk = newChannel.rotationKeyframes.begin(); rk != newChannel.rotationKeyframes.end(); ++rk)
		{
			//existingChannel.rotationKeyframes[rk->first] = rk->second;
			existingChannel.rotationKeyframes.at(i).value = rk->value;
			i++;
		}
	}
	if (newChannel.HasScaleKeyframes())
	{
		if (!existingChannel.HasScaleKeyframes())
		{
			existingChannel.scaleKeyframes.clear();
		}

		uint i = 0;
		for (auto sk = newChannel.scaleKeyframes.begin(); sk != newChannel.scaleKeyframes.end(); ++sk)
		{
			//existingChannel.scaleKeyframes[sk->first] = sk->second;
			existingChannel.scaleKeyframes.at(i).value = sk->value;
			i++;
		}
	}
}