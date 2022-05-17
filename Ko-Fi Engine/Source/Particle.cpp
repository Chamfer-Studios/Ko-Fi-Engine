#include "Particle.h"

#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

Particle::Particle()
{
	active = false;
	position = float3::zero;
	rotation = Quat(0.0f, 0.0f, 0.0f, 0.0f);
	degrees = 0;
	scale = float3(1.0f, 1.0f, 1.0f);
	velocity = float3::zero;
	acceleration = float3::zero;
	direction = float3::zero;
	//intensity = 1.0f;
	lifeTime = 0.0f;
	maxLifetime = 0.0f;
	distanceToCamera = 0.0f;
	CurrentColor = Color(1.0f,1.0f,1.0f,1.0f);
}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	if (!active)
	{
		return true;
	}

	return true;
}

void Particle::Draw(uint id, uint indexNum)
{
	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	if (id != 0)
		glBindTexture(GL_TEXTURE_2D,id);
	
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	float4x4 t = float4x4::FromTRS(position, rotation, scale);
	glPushMatrix();
	glMultMatrixf((float*)t.Transposed().ptr());

	/*glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDrawElements(GL_TRIANGLES, indexNum, GL_UNSIGNED_INT, NULL);

	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);*/


	//Drawing to tris in direct mode
	glBegin(GL_TRIANGLES);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-.5f, -.5f, .0f);

	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(.5f, -.5f, .0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(.5f, .5f, .0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-.5f, .5f, .0f);

	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

	//glBindTexture(GL_TEXTURE_2D, 0);

	//glPopMatrix();

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_NORMAL_ARRAY, 0);
	//glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);

}