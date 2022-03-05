#include "RNG.h"

// --- LCG ---
uint32 RNG::GetRandomUint()
{
	return (uint32)lcg_rand.Int();
}

uint32 RNG::GetBoundedRandomUint(uint32 min, uint32 max)
{
	return (uint32)lcg_rand.Int(min, max);
}

float RNG::GetBoundedRandomFloat(float min, float max)
{
	return lcg_rand.Float(min, max);
}