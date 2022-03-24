#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Globals.h"
//#include "Resource.h"

#include <string>
#include <vector>

struct Channel;

class R_Animation
{
public:
	R_Animation();
	~R_Animation();

	inline void SetName(std::string name) { this->name = name; }
	inline void SetDuration(float duration) { this->duration = duration; }
	inline void SetTicksPerSecond(float ticksPerSecond) { this->ticksPerSecond = ticksPerSecond; }
	inline void SetStartFrame(float startFrame) { this->startFrame = startFrame; }
	inline void SetEndFrame(float endFrame) { this->endFrame = endFrame; }

	inline std::string GetName() const { return name; }
	inline float GetDuration() const { return duration; }
	inline float GetTicksPerSecond() const { return ticksPerSecond; }
	inline float GetStartFrame() const { return startFrame; }
	inline float GetEndFrame() const { return endFrame; }

public:
	std::vector<Channel> channels;

public:
	std::string name;
	float duration;
	float ticksPerSecond;
	int startFrame;
	int endFrame;
};

#endif // !__ANIMATION_H__
