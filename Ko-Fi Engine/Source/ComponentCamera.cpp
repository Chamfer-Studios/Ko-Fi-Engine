#include "ComponentCamera.h"
#include "Globals.h"

#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	type = ComponentType::CAMERA;

	/*frustum.pos = float3(0.0f, 0.0f, -5.0f);
	frustum.horizontalFov = horizontalFOV * DEGTORAD;
	frustum.verticalFov = 30 * DEGTORAD;
	frustum.AspectRatio();
	frustum.nearPlaneDistance = 5.0f;
	frustum.farPlaneDistance = 100.0f;
	frustum.CenterPoint()*/
}

ComponentCamera::~ComponentCamera()
{
}

bool ComponentCamera::Update()
{
	bool ret = true;

	return ret;
}

bool ComponentCamera::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	
	if (ImGui::CollapsingHeader("Camera")) {

	}

	return ret;
}

void ComponentCamera::DrawFrustum() const
{
	float3 cornerPoints[8];
	frustum.GetCornerPoints(cornerPoints);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(3.5f);
	glBegin(GL_LINES);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glVertex3f(cornerPoints[0].x, cornerPoints[0].y, cornerPoints[0].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[1].x, cornerPoints[1].y, cornerPoints[1].z);

	glVertex3f(cornerPoints[5].x, cornerPoints[5].y, cornerPoints[5].z);
	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[2].x, cornerPoints[2].y, cornerPoints[2].z);

	glVertex3f(cornerPoints[6].x, cornerPoints[6].y, cornerPoints[6].z);
	glVertex3f(cornerPoints[4].x, cornerPoints[4].y, cornerPoints[4].z);

	glVertex3f(cornerPoints[7].x, cornerPoints[7].y, cornerPoints[7].z);
	glVertex3f(cornerPoints[3].x, cornerPoints[3].y, cornerPoints[3].z);

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}