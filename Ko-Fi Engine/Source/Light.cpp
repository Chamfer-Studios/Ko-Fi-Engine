#include "Globals.h"
#include "Light.h"
#include <gl/GL.h>
// Memory Leak Detection
//#include "MemLeaks.h"

Light::Light() : ref(-1), on(false)
{
	pos_x = pos_y = pos_z = 0.f;
}

void Light::Init()
{
	glLightfv(ref, GL_AMBIENT, &ambient);
	glLightfv(ref, GL_DIFFUSE, &diffuse);
}

void Light::SetPos(float x, float y, float z)
{
	pos_x = x;
	pos_y = y;
	pos_z = z;
}

void Light::Render()
{
	if (on)
	{
		float pos[] = { pos_x, pos_y, pos_z, 1.0f };
		glLightfv(ref, GL_POSITION, pos);
	}
}

void Light::Active(bool active)
{
	if (on != active)
	{
		on = !on;

		if (on)
			glEnable(ref);
		else
			glDisable(ref);
	}
}