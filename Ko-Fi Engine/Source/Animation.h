#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <string>
#include "MathGeoLib/Math/float2.h"
#include "Globals.h"
//#include "Resource.h"

class Animation
{
public:
	Animation();
	~Animation();

	inline void SetName(std::string name) { this->name = name; }
	inline void SetTicksPerSecond(float ticksPerSecond) { this->ticksPerSecond = ticksPerSecond; }
	inline void SetDuration(float duration) { this->duration = duration; }
	inline void SetStartFrame(float startPoint) { this->startPoint = startPoint; }
	inline void SetEndFrame(float endPoint) { this->endPoint = endPoint; }

	inline std::string GetName() const { return name; }
	inline float GetTicksPerSecond() const { return ticksPerSecond; }
	inline float GetDuration() const { return duration; }
	inline float GetStartFrame() const { return startPoint; }
	inline float GetEndFrame() const { return endPoint; }

public:
	std::string name;
	float ticksPerSecond;
	float duration;
	int startPoint;
	int endPoint;
};

#endif // !__TEXTURE_H__
