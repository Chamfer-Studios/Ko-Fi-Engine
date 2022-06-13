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
	channels.clear();
}
