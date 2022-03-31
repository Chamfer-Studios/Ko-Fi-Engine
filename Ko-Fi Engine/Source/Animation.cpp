#include "Animation.h"

Animation::Animation() : Resource(ResourceType::ANIMATION)
{
	name = "";
	ticksPerSecond = 0;
	duration = 0;
	startPoint = 0;
	endPoint = 0;
}

Animation::~Animation()
{

}