#include "Primitive.h"
#include "Globals.h"
//#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>

// ------------------------------------------------------------
Primitive::Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(PrimitiveTypes::Primitive_Point)
{}

// ------------------------------------------------------------
PrimitiveTypes Primitive::GetType() const
{
	return type;
}

// ------------------------------------------------------------
void Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

// ------------------------------------------------------------
void Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

// ------------------------------------------------------------
void Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// CUBE ============================================
Cube::Cube() : Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(float sizeX, float sizeY, float sizeZ) : Primitive(), size(sizeX, sizeY, sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void Cube::InnerRender() const
{
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(-sx, sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, sy, -sz);
	glVertex3f(sx, sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(sx, sy, sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy, sz);
	glVertex3f(-sx, sy, sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glVertex3f(sx, sy, sz);
	glVertex3f(sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx, -sy, sz);
	glVertex3f(-sx, -sy, sz);

	glEnd();
}

void Cube::DrawInterleavedMode()
{
	// cube ///////////////////////////////////////////////////////////////////////
	//    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v7---|-|v4
	//  |/      |/
	//  v2------v3

	float sx = size.x;
	float sy = size.y;
	float sz = size.z;

	// interleaved vertex array for glDrawElements() & glDrawRangeElements() ======
	// All vertex attributes (position, normal, color) are packed together as a
	// struct or set, for example, ((V,N,C), (V,N,C), (V,N,C),...).
	// It is called an array of struct, and provides better memory locality.
	GLfloat vertices[]  = { sx, sy, sz,   0, 0, 1,   1, 1, 1,              // v0 (front)
						   -sx, sy, sz,   0, 0, 1,   1, 1, 0,              // v1
						   -sx,-sy, sz,   0, 0, 1,   1, 0, 0,              // v2
							sx,-sy, sz,   0, 0, 1,   1, 0, 1,              // v3

							sx, sy, sz,   1, 0, 0,   1, 1, 1,              // v0 (right)
							sx,-sy, sz,   1, 0, 0,   1, 0, 1,              // v3
							sx,-sy,-sz,   1, 0, 0,   0, 0, 1,              // v4
							sx, sy,-sz,   1, 0, 0,   0, 1, 1,              // v5

							sx, sy, sz,   0, 1, 0,   1, 1, 1,              // v0 (top)
							sx, sy,-sz,   0, 1, 0,   0, 1, 1,              // v5
						   -sx, sy,-sz,   0, 1, 0,   0, 1, 0,              // v6
						   -sx, sy, sz,   0, 1, 0,   1, 1, 0,              // v1

						   -sx, sy, sz,  -1, 0, 0,   1, 1, 0,              // v1 (left)
						   -sx, sy,-sz,  -1, 0, 0,   0, 1, 0,              // v6
						   -sx,-sy,-sz,  -1, 0, 0,   0, 0, 0,              // v7
						   -sx,-sy, sz,  -1, 0, 0,   1, 0, 0,              // v2

						   -sx,-sy,-sz,   0,-1, 0,   0, 0, 0,              // v7 (bottom)
							sx,-sy,-sz,   0,-1, 0,   0, 0, 1,              // v4
							sx,-sy, sz,   0,-1, 0,   1, 0, 1,              // v3
						   -sx,-sy, sz,   0,-1, 0,   1, 0, 0,              // v2

							sx,-sy,-sz,   0, 0,-1,   0, 0, 1,              // v4 (back)
						   -sx,-sy,-sz,   0, 0,-1,   0, 0, 0,              // v7
						   -sx, sy,-sz,   0, 0,-1,   0, 1, 0,              // v6
							sx, sy,-sz,   0, 0,-1,   0, 1, 1 };            // v5

	// index array of vertex array for glDrawElements() & glDrawRangeElement()
	GLubyte indices[]  = { 0, 1, 2,   2, 3, 0,      // front
						   4, 5, 6,   6, 7, 4,      // right
						   8, 9,10,  10,11, 8,      // top
						  12,13,14,  14,15,12,      // left
						  16,17,18,  18,19,16,      // bottom
						  20,21,22,  22,23,20 };    // back

	// enable and specify pointers to vertex arrays
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glNormalPointer(GL_FLOAT, 9 * sizeof(GLfloat), vertices + 3);
	glColorPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), vertices + 6);
	glVertexPointer(3, GL_FLOAT, 9 * sizeof(GLfloat), vertices);

	glPushMatrix();
	/*glTranslatef(-2, -2, 0);*/                // move to bottom-left

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	glPopMatrix();

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

// SPHERE ============================================
Sphere::Sphere() : Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius) : Primitive(), radius(radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

Sphere::Sphere(float radius, unsigned int stacks, unsigned int sectors) : Primitive(), radius(radius), stacks(stacks), sectors(sectors)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void Sphere::InnerRender() const
{
	for (int j = 0; j < stacks; j++)
	{
		double latitude1 = (M_PI / stacks) * j - M_PI_2;
		double latitude2 = (M_PI / stacks) * (j + 1) - M_PI_2;
		double sinLat1 = sin(latitude1);
		double cosLat1 = cos(latitude1);
		double sinLat2 = sin(latitude2);
		double cosLat2 = cos(latitude2);
		glBegin(GL_TRIANGLE_STRIP);
		for (int i = 0; i <= sectors; i++)
		{
			double longitude = (2 * M_PI / sectors) * i;
			double sinLong = sin(longitude);
			double cosLong = cos(longitude);
			double x1 = cosLong * cosLat1;
			double y1 = sinLong * cosLat1;
			double z1 = sinLat1;
			double x2 = cosLong * cosLat2;
			double y2 = sinLong * cosLat2;
			double z2 = sinLat2;
			glNormal3d(x2, y2, z2);
			glVertex3d(radius * x2, radius * y2, radius * z2);
			glNormal3d(x1, y1, z1);
			glVertex3d(radius * x1, radius * y1, radius * z1);
		}
		glEnd();
	}
}

// CYLINDER ============================================
Cylinder::Cylinder() : Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

Cylinder::Cylinder(float radius, float height) : Primitive(), radius(radius), height(height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void Cylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
Line::Line() : Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

Line::Line(float x, float y, float z) : Primitive(), origin(0, 0, 0), destination(x, y, z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
Plane::Plane() : Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

Plane::Plane(float x, float y, float z, float d) : Primitive(), normal(x, y, z), constant(d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void Plane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}