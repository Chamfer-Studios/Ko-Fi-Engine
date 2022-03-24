#ifndef __CHANNEL__
#define __CHANNEL__

#include <string>
#include <map>

#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/Quat.h"

enum class KeyframeType
{
	POSITION,
	ROTATION,
	SCALE
};

struct Channel
{
	Channel();
	Channel(const char* name);

	bool				HasKeyframes(KeyframeType type) const;
	bool				HasPositionKeyframes() const;	// --- There will be no keyframes when:
	bool				HasRotationKeyframes() const;	// Size == 1	(Initial Position. Always needed regardless)
	bool				HasScaleKeyframes() const;		// Time == -1	(Time cannot be negative, -1 used as "non-valid" ID)

	std::map<double, float3>	positionKeyframes;	// Position-related keyframes.
	std::map<double, Quat>		rotationKeyframes;	// Rotation-related keyframes.
	std::map<double, float3>	scaleKeyframes;		// Scale-related keyframes.

	std::string name;
};

#endif __CHANNEL__
