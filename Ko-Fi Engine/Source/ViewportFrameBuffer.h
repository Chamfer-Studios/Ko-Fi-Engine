#pragma once
#include "Module.h"
#include "Globals.h"

#include <string>

class KoFiEngine;

class ViewportFrameBuffer : public Module
{
public:
	ViewportFrameBuffer(KoFiEngine* engine/*, bool start_enabled = true*/);
	~ViewportFrameBuffer();

	bool Start();
	bool PreUpdate(float dt);
	bool PostUpdate(float dt);
	void OnResize(int width, int height);
	bool CleanUp();

public:
	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint textureBuffer = 0;
	bool show_viewport_window = true;

	KoFiEngine* engine = nullptr;
};