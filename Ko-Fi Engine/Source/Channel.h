#ifndef __CHANNEL__
#define __CHANNEL__

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

#include <string>
#include <vector>

enum class KeyframeType
{
	POSITION,
	ROTATION,
	SCALE
};

struct PositionKeyframe
{
	PositionKeyframe(double time, float3 value) : time(time), value(value) {}

	double time;
	float3 value;
};

struct RotationKeyframe
{
	RotationKeyframe(double time, Quat value) : time(time), value(value) {}

	double time;
	Quat value;
};

struct ScaleKeyframe
{
	ScaleKeyframe(double time, float3 value) : time(time), value(value) {}

	double time;
	float3 value;
};

struct Channel
{
	Channel();
	Channel(const char* name);

	bool				HasKeyframes(KeyframeType type) const;
	bool				HasPositionKeyframes() const;	// --- There will be no keyframes when:
	bool				HasRotationKeyframes() const;	// Size == 1	(Initial Position. Always needed regardless)
	bool				HasScaleKeyframes() const;		// Time == -1	(Time cannot be negative, -1 used as "non-valid" ID)

	std::vector<PositionKeyframe>	positionKeyframes;	// Position-related keyframes.
	std::vector<RotationKeyframe>	rotationKeyframes;	// Rotation-related keyframes.
	std::vector<ScaleKeyframe>		scaleKeyframes;		// Scale-related keyframes.

	std::string name;
};

#endif __CHANNEL__