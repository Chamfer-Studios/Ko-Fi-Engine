#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "Globals.h"
#include "Resource.h"

#include <map>
#include <string>

struct Channel;

class R_Animation : public Resource
{
public:
	R_Animation();
	~R_Animation() override;

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
	std::map<std::string, Channel> channels;

public:
	std::string name;
	float duration;
	float ticksPerSecond;
	int startFrame; // Not necessary, remove when possible.
	int endFrame; // Not necessary, remove when possible.
};

#endif // !__ANIMATION_H__