#include "Engine.h"
#include "ViewportFrameBuffer.h"
#include "Globals.h"
#include "Window.h"
#include <string>
#include "imgui.h"
#include "imgui_internal.h"
#include "glew.h"
#include <gl/GL.h>

ViewportFrameBuffer::ViewportFrameBuffer(KoFiEngine* engine/*, bool start_enabled*/) : Module(/*engine, start_enabled*/)
{
	show_viewport_window = true;

	name = "ViewportFrameBuffer";
	this->engine = engine;
}

ViewportFrameBuffer::~ViewportFrameBuffer()
{}

bool ViewportFrameBuffer::Start()
{
	bool ret = false;
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, engine->GetWindow()->GetWidth(), engine->GetWindow()->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0); //Unbind texture

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//Render Buffers
	glGenRenderbuffers(1, &renderBufferoutput);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBufferoutput);

	//Bind tex data with render buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, engine->GetWindow()->GetWidth(), engine->GetWindow()->GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferoutput);

	//After binding tex data, we must unbind renderbuffer and framebuffer not usefull anymore
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool ViewportFrameBuffer::PreUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	return true;
}

bool ViewportFrameBuffer::PostUpdate(float dt)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return true;
}

bool ViewportFrameBuffer::CleanUp()
{
	/*texture ? glDeleteTextures(1, &texture) : 0;
	frameBuffer ? glDeleteFramebuffers(1, &frameBuffer) : 0;
	renderBufferoutput ? glDeleteRenderbuffers(1, &renderBufferoutput): 0;*/
	if (texture != 0) glDeleteTextures(1, &texture);
	if (frameBuffer != 0) glDeleteFramebuffers(1, &frameBuffer);
	if (renderBufferoutput != 0) glDeleteRenderbuffers(1, &renderBufferoutput);

	return true;
}

// Method to receive and manage events
void ViewportFrameBuffer::OnNotify(const Event& event)
{
	// Manage events
}