#include "AnimatorClip.h"
#include <string>
#include "Globals.h"

class Animation;

typedef unsigned int uint;

AnimatorClip::AnimatorClip() :
	clipAnim(nullptr),
	name("[NONE]"),
	start(0),
	end(0),
	speed(1.0f),
	duration(0.0f),
	durationInSeconds(0.0f),
	loop(false),
	time(0.0f),
	frame(0.0f),
	tick(0)
	//inNewTick(false),
	//playing(false)
{

}

AnimatorClip::AnimatorClip(const Animation* animation, const std::string& name, uint start, uint end, float speed, bool loop) :
	clipAnim(animation),
	name(name),
	start(start),
	end(end),
	speed(speed),
	loop(loop),
	time(0.0f),
	frame(0.0f),
	tick(0)
	///inNewTick(false),
	//playing(false)
{
	duration = ((float)(end - start)) / speed;
	//durationInSeconds = (animation != nullptr) ? (duration / animation->GetTicksPerSecond()) : 0.0f;
}

void AnimatorClip::CleanUp()
{
	name.clear();
	name.shrink_to_fit();

	if (clipAnim)
		RELEASE(clipAnim);
}
