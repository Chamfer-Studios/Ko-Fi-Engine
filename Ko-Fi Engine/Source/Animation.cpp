#include "Animation.h"

Animation::Animation() 
{
	name = "";
	ticksPerSecond = 0;
	duration = 0;
	startPoint = 0;
	endPoint = 0;
}

Animation::~Animation()
{
	CleanUp();
}

void Animation::CleanUp()
{
	name.clear();
	name.shrink_to_fit();
}
