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
	bool CleanUp();
	// Method to receive and manage events
	void OnNotify(const Event& event);

public:
	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool show_viewport_window = true;

	KoFiEngine* engine = nullptr;
};