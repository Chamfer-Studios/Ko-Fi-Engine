#include "R_Animation.h"
#include "Channel.h"

R_Animation::R_Animation() : Resource(ResourceType::ANIMATION)
{
	name = "";
	ticksPerSecond = 0;
	duration = 0;
	startFrame = 0;
	endFrame = 0;
}

R_Animation::~R_Animation()
{
	for (auto channel : channels)
	{
		channel.second.positionKeyframes.clear();
		channel.second.positionKeyframes.shrink_to_fit();
		channel.second.rotationKeyframes.clear();
		channel.second.rotationKeyframes.shrink_to_fit();
		channel.second.scaleKeyframes.clear();
		channel.second.scaleKeyframes.shrink_to_fit();
		channel.second.name.clear();
		channel.second.name.shrink_to_fit();
	}
	channels.clear();
	name.clear();
	name.shrink_to_fit();
}
