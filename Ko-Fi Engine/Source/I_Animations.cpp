#include "I_Animations.h"
#include "Globals.h"
#include "Log.h"
#include "R_Animation.h"
#include "Engine.h"
#include "Channel.h"
#include "M_FileSystem.h"
#include "FSDefs.h"

#include "MathGeoLib/Math/float3.h"

#include <map>
#include <fstream>

I_Animations::I_Animations(KoFiEngine* engine) : engine(engine)
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
		rAnimation->channels.emplace(std::make_pair(channel->second.name, channel->second));
	}

	tmp.clear();
	
	return true;
}

bool I_Animations::Save(const R_Animation* animation, const char* path)
{
	if (engine->GetFileSystem()->CheckDirectory(ANIMATIONS_DIR))
	{
		std::ofstream file;
		file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
		if (file.is_open())
		{
			// HEADER
			// Writing each variable size bytes into the header of the file.
			uint nameSizeBytes = animation->name.length();
			uint durationSizeBytes = sizeof(float);
			uint ticksPerSecondSizeBytes = sizeof(float);
			uint numChannelsSizeBytes = sizeof(uint);
			uint channelsSizeBytes = GetChannelsDataSize(animation);
			file.write((char*)&nameSizeBytes, sizeof(unsigned));									// Name size bytes
			file.write((char*)&durationSizeBytes, sizeof(unsigned));								// Duration size bytes
			file.write((char*)&ticksPerSecondSizeBytes, sizeof(unsigned));							// Ticks per second size bytes
			file.write((char*)&numChannelsSizeBytes, sizeof(unsigned));								// Number of channels size bytes
			file.write((char*)&channelsSizeBytes, sizeof(unsigned));								// Channels size bytes

			// BODY
			// Writing each variable into the file.
			file.write((char*)animation->name.data(), nameSizeBytes);								// Name
			file.write((char*)&animation->duration, durationSizeBytes);								// Duration
			file.write((char*)&animation->ticksPerSecond, ticksPerSecondSizeBytes);					// Ticks per second
			uint numChannels = animation->channels.size();
			file.write((char*)&numChannels, numChannelsSizeBytes);									// Number of channels

			for (auto channel = animation->channels.begin();
				channel != animation->channels.end(); channel++)									// Channels
			{
				// Writing each channel variable size bytes.
				uint channelNameSizeBytes = channel->second.name.length();
				uint vecKeySize = sizeof(double) + (sizeof(float) * 3);								// float3 is composed by 3 floats.
				uint quatKeySize = sizeof(double) + (sizeof(float) * 4);							// Quat is composed by 4 floats.
				uint positionKeyframesSizeBytes = channel->second.positionKeyframes.size() * vecKeySize;
				uint rotationKeyframesSizeBytes = channel->second.rotationKeyframes.size() * quatKeySize;
				uint scaleKeyframesSizeBytes = channel->second.scaleKeyframes.size() * vecKeySize;
				file.write((char*)&channelNameSizeBytes, sizeof(unsigned));							// Channel name size bytes
				file.write((char*)&positionKeyframesSizeBytes, sizeof(unsigned));					// Position keyframes size bytes
				file.write((char*)&rotationKeyframesSizeBytes, sizeof(unsigned));					// Rotation keyframes size bytes
				file.write((char*)&scaleKeyframesSizeBytes, sizeof(unsigned));						// Scale keyframes size bytes

				// Writing each channel variable into the file.
				file.write((char*)channel->second.name.data(), channelNameSizeBytes);				// Channel name
				std::vector<PositionKeyframe> positionKeyframes = channel->second.positionKeyframes;
				for (std::vector<PositionKeyframe>::iterator it = positionKeyframes.begin();
					it != positionKeyframes.end(); it++)											// Position keyframes
				{
					PositionKeyframe posKF = (*it);
					file.write((char*)&posKF.time, sizeof(double));
					file.write((char*)&posKF.value.x, sizeof(float));
					file.write((char*)&posKF.value.y, sizeof(float));
					file.write((char*)&posKF.value.z, sizeof(float));
				}
				std::vector<RotationKeyframe> rotationKeyframes = channel->second.rotationKeyframes;
				for (std::vector<RotationKeyframe>::iterator it = rotationKeyframes.begin();
					it != rotationKeyframes.end(); it++)											// Rotation keyframes
				{
					RotationKeyframe rotKF = (*it);
					file.write((char*)&rotKF.time, sizeof(double));
					file.write((char*)&rotKF.value.x, sizeof(float));
					file.write((char*)&rotKF.value.y, sizeof(float));
					file.write((char*)&rotKF.value.z, sizeof(float));
					file.write((char*)&rotKF.value.w, sizeof(float));
				}
				std::vector<ScaleKeyframe> scaleKeyframes = channel->second.scaleKeyframes;
				for (std::vector<ScaleKeyframe>::iterator it = scaleKeyframes.begin();
					it != scaleKeyframes.end(); it++)												// Scale keyframes
				{
					ScaleKeyframe scaleKF = (*it);
					file.write((char*)&scaleKF.time, sizeof(double));
					file.write((char*)&scaleKF.value.x, sizeof(float));
					file.write((char*)&scaleKF.value.y, sizeof(float));
					file.write((char*)&scaleKF.value.z, sizeof(float));
				}
			}

			file.close();

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Animation Save: directory %s couldn't be accessed.", ANIMATIONS_DIR);

	return false;
}

bool I_Animations::Load(const char* path, R_Animation* animation)
{
	if (engine->GetFileSystem()->CheckDirectory(ANIMATIONS_DIR))
	{
		std::ifstream file;
		file.open(path, std::ios::binary);
		if (file.is_open())
		{
			// HEADER
			// Reading each variable size bytes from the header of the file.
			uint nameSizeBytes = 0;
			uint durationSizeBytes = 0;
			uint ticksPerSecondSizeBytes = 0;
			uint numChannelsSizeBytes = 0;
			uint channelsSizeBytes = 0;
			file.read((char*)&nameSizeBytes, sizeof(unsigned));										// Name size bytes
			file.read((char*)&durationSizeBytes, sizeof(unsigned));									// Duration size bytes
			file.read((char*)&ticksPerSecondSizeBytes, sizeof(unsigned));							// Ticks per second
			file.read((char*)&numChannelsSizeBytes, sizeof(unsigned));								// Number of channels
			file.read((char*)&channelsSizeBytes, sizeof(unsigned));									// Channels

			// BODY
			// Reading each variable from the file.
			std::string name;
			float duration = 0.0f;
			float ticksPerSecond = 0.0f;
			uint numChannels = 0;
			name.resize(nameSizeBytes);
			file.read((char*)(name.data()), nameSizeBytes);											// Name
			file.read((char*)&duration, durationSizeBytes);											// Duration
			file.read((char*)&ticksPerSecond, ticksPerSecondSizeBytes);								// Ticks per second
			file.read((char*)&numChannels, numChannelsSizeBytes);									// Number of channels

			// Setting each variable
			animation->SetName(std::string(name));													// Set name
			animation->SetDuration(duration);														// Set duration
			animation->SetTicksPerSecond(ticksPerSecond);											// Set ticks per second
			animation->SetStartFrame(0);															// Set start frame
			animation->SetEndFrame(duration);														// Set end frame

			for (int i = 0; i < numChannels; ++i)													// Channels
			{
				// Reading each channel variable size bytes.
				uint channelNameSizeBytes = 0;
				uint positionKeyframesSizeBytes = 0;
				uint rotationKeyframesSizeBytes = 0;
				uint scaleKeyframesSizeBytes = 0;
				file.read((char*)&channelNameSizeBytes, sizeof(unsigned));							// Channel name size bytes
				file.read((char*)&positionKeyframesSizeBytes, sizeof(unsigned));					// Position keyframes size bytes
				file.read((char*)&rotationKeyframesSizeBytes, sizeof(unsigned));					// Rotation keyframes size bytes
				file.read((char*)&scaleKeyframesSizeBytes, sizeof(unsigned));						// Scale keyframes size bytes

				// Reading each channel variable from the file.
				std::string channelName;
				channelName.resize(channelNameSizeBytes);
				file.read((char*)(channelName.data()), channelNameSizeBytes);						// Channel name

				Channel channel(channelName.c_str());

				uint vecKeySize = sizeof(double) + (sizeof(float) * 3);								// float3 is composed by 3 floats.
				uint quatKeySize = sizeof(double) + (sizeof(float) * 4);							// Quat is composed by 4 floats.
				uint numPositionKeyframes = positionKeyframesSizeBytes / vecKeySize;
				uint numRotationKeyframes = rotationKeyframesSizeBytes / quatKeySize;
				uint numScaleKeyframes = scaleKeyframesSizeBytes / vecKeySize;
				for (int j = 0; j < numPositionKeyframes; ++j)										// Position keyframes
				{
					double time = 0;
					float3 value;
					file.read((char*)&time, sizeof(double));
					file.read((char*)&value.x, sizeof(float));
					file.read((char*)&value.y, sizeof(float));
					file.read((char*)&value.z, sizeof(float));

					channel.positionKeyframes.push_back(PositionKeyframe(time, value));
				}
				for (int k = 0; k < numRotationKeyframes; ++k)										// Rotation keyframes
				{
					double time = 0;
					Quat value;
					file.read((char*)&time, sizeof(double));
					file.read((char*)&value.x, sizeof(float));
					file.read((char*)&value.y, sizeof(float));
					file.read((char*)&value.z, sizeof(float));
					file.read((char*)&value.w, sizeof(float));

					channel.rotationKeyframes.push_back(RotationKeyframe(time, value));
				}
				for (int o = 0; o < numScaleKeyframes; ++o)											// Scale keyframes
				{
					double time = 0;
					float3 value;
					file.read((char*)&time, sizeof(double));
					file.read((char*)&value.x, sizeof(float));
					file.read((char*)&value.y, sizeof(float));
					file.read((char*)&value.z, sizeof(float));

					channel.scaleKeyframes.push_back(ScaleKeyframe(time, value));
				}
				animation->channels.emplace(std::make_pair(channel.name, channel));

			}

			file.close();

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Animation Load: directory %s couldn't be accessed.", ANIMATIONS_DIR);

	return false;
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

uint I_Animations::GetChannelsDataSize(const R_Animation* rAnimation)
{
	uint channelsSize = 0;																									// Will contain the total size of all the animation's channels.

	if (rAnimation == nullptr)
	{
		KOFI_ERROR("Animations Importer: Could not get Channels Size Of Data! Error: Argument R_Animation* was nullptr");
		return 0;
	}

	uint vecKeySize = sizeof(double) + (sizeof(float) * 3);																	// float3 is composed by 3 floats.
	uint quatKeySize = sizeof(double) + (sizeof(float) * 4);																// Quat is composed by 4 floats.
	for (auto c = rAnimation->channels.begin(); c != rAnimation->channels.end(); ++c)
	{
		const Channel& rChannel = c->second;
		uint channelSize = 0;

		/*channelSize += sizeof(uint);*/ // Channel Type
		channelSize += sizeof(uint) * 4;																					// Length of the name and sizes of the keys maps.
		channelSize += (strlen(rChannel.name.c_str()) * sizeof(char));
		channelSize += rChannel.positionKeyframes.size() * vecKeySize;
		channelSize += rChannel.rotationKeyframes.size() * quatKeySize;
		channelSize += rChannel.scaleKeyframes.size() * vecKeySize;

		channelsSize += channelSize;
	}

	return channelsSize;
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