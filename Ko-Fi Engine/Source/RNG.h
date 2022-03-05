#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "Globals.h"

namespace RNG // RNG library included with MathGeoLib. Average Performance (Real random factor...)
{
	static math::LCG lcg_rand;

	uint32	GetRandomUint();
	uint32	GetBoundedRandomUint(uint32 min = 0, uint32 max = 4294967295);
	float	GetBoundedRandomFloat(float min, float max);
}

#endif // !__RANDOM_H__