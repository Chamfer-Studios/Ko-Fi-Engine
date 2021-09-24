#ifndef __TIMER_H__
#define __TIMER_H__

#include "Defs.h"
#include "SDL\include\SDL_timer.h"

class Timer
{
public:

	Timer()
	{
		Start();
	}

	void Start()
	{
		startTime = SDL_GetTicks();
	}

	uint32 Read() const
	{
		return SDL_GetTicks() - startTime;
	}

	float ReadSec() const
	{
		return float(SDL_GetTicks() - startTime) / 1000.0f;
	}

private:

	uint32 startTime;
};

#endif //__TIMER_H__