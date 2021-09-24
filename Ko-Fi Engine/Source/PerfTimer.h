#ifndef __PERFTIMER_H__
#define __PERFTIMER_H__

#include "Defs.h"
#include "SDL\include\SDL_timer.h"

class PerfTimer
{
public:

	PerfTimer()
	{
		if (frequency == 0) frequency = SDL_GetPerformanceFrequency();

		Start();
	}

	void Start()
	{
		startTime = SDL_GetPerformanceCounter();
	}

	double ReadMs() const
	{
		return 1000.0 * (double(SDL_GetPerformanceCounter() - startTime) / double(frequency));
	}

	uint64 ReadTicks() const
	{
		return SDL_GetPerformanceCounter() - startTime;
	}

private:

	uint64 startTime;
	uint64 frequency;
};

#endif //__PERFTIMER_H__