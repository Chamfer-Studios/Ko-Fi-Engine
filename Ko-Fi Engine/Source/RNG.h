#pragma once
#include "MathGeoLib/Algorithm/Random/LCG.h"
class RNG {
public:
	RNG(){
	}
	~RNG() {

	}

	int GetRandomInt() {
		LCG randomGenerator;

		return randomGenerator.Int();
	}
	int GetRandomInt(int min, int max) {
		LCG randomGenerator;

		return randomGenerator.Int(min, max);
	}
};