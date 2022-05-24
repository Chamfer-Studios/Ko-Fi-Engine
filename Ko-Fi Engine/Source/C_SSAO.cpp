#include "C_SSAO.h"

#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

C_SSAO::C_SSAO(GameObject* parent) : Component(parent)
{
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1920, 1080, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
}

bool C_SSAO::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("SSAO", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		if (DrawDeleteButton(owner, this))
			return true;
	}
	else
		DrawDeleteButton(owner, this);

	return true;
}
