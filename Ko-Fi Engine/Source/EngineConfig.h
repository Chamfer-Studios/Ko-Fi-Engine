#pragma once
#include "PerfTimer.h"
#include "Timer.h"
#include "SString.h"
#include "SDL.h"
class EngineConfig {
public:
	EngineConfig() {
		title = "";
		organization = "";
		frameCount = 0;
		lastSecFrameCount = 0;
		prevLastSecFrameCount = 0;
		dt = 0.0f;
		cappedMs = -1;
	}
	SString title;
	SString organization;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	float lastSecFrameCount = 0;
	float prevLastSecFrameCount = 0;
	float dt = 0.0f;
	int	cappedMs = -1;
	int maxFps = 60;
	std::vector<float> msLog;
	std::vector<float> fpsLog;

	//Hardware
	SDL_version sdlVersion;
	Uint8 cpuCores = 0;
	float RAM;
	float vramBudget, vramUsage, vramAvailable,vramReserved = 0;
	unsigned char* gpuVendor = nullptr;
	unsigned char* gpuRenderer = nullptr;
	unsigned char* gpuVersion = nullptr;
};