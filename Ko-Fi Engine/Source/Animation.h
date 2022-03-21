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

	std::string GetName();


public:

	std::string name;
	float ticksPerSecond;
	float duration;

	int startPoint;
	int endPoint;
};

#endif // !__TEXTURE_H__
