#include "Mesh.h"

// OpenGL / GLEW
#include "glew.h"
//#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>

#include "Defs.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glDeleteBuffers(1, &id_vertex);
	RELEASE_ARRAY(vertices);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glDeleteBuffers(1, &id_normal);
	RELEASE_ARRAY(normals);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glDeleteBuffers(1, &id_index);
	RELEASE_ARRAY(indices);

	// Texture coords
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	glDeleteBuffers(1, &id_tex_coord);
	RELEASE_ARRAY(tex_coords);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetUpMeshBuffers()
{
	SetUpDefaultTexture();

	// Vertices
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);

	// Normals
	glGenBuffers(1, &id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);

	// Indices
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);

	// Texture coords
	glGenBuffers(1, &id_tex_coord);
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords * 2, tex_coords, GL_STATIC_DRAW);
}

void Mesh::SetUpDefaultTexture()
{
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture.textureID);
	glBindTexture(GL_TEXTURE_2D, texture.textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(texture.textureID, 0);
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	//vertex
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	//normals
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glNormalPointer(GL_FLOAT, 0, NULL);
	//coord
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	//texture
	glBindTexture(GL_TEXTURE_2D, texture.textureID);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	DebugDraw();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Mesh::DebugDraw()
{
	// Debug draw
	if (drawVertexNormals) DrawVertexNormals();
	if (drawFaceNormals) DrawFaceNormals();
}

void Mesh::DrawVertexNormals() const
{
	if (id_normal == -1 || normals == nullptr)
		return;

	float normal_lenght = 0.5f;

	// Vertices normals
	glBegin(GL_LINES);
	for (size_t i = 0, c = 0; i < num_vertices * 3; i += 3, c += 4)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		//glColor4f(colors[c], colors[c + 1], colors[c + 2], colors[c + 3]);
		glVertex3f(vertices[i], vertices[i + 1], vertices[i + 2]);

		glVertex3f(vertices[i] + (normals[i] * normal_lenght),
			vertices[i + 1] + (normals[i + 1] * normal_lenght),
			vertices[i + 2] + (normals[i + 2]) * normal_lenght);
	}

	glColor3f(1.0f, 1.0f, 1.0f);
	glEnd();
}

void Mesh::DrawFaceNormals() const
{
	if (id_normal == -1 || normals == nullptr)
		return;

	// Face normals
	glBegin(GL_LINES);
	for (size_t i = 0; i < num_vertices * 3; i += 3)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		float vx = (vertices[i] + vertices[i + 3] + vertices[i + 6]) / 3;
		float vy = (vertices[i + 1] + vertices[i + 4] + vertices[i + 7]) / 3;
		float vz = (vertices[i + 2] + vertices[i + 5] + vertices[i + 8]) / 3;

		float nx = (normals[i] + normals[i + 3] + normals[i + 6]) / 3;
		float ny = (normals[i + 1] + normals[i + 4] + normals[i + 7]) / 3;
		float nz = (normals[i + 2] + normals[i + 5] + normals[i + 8]) / 3;

		glVertex3f(vx, vy, vz);

		glVertex3f(vx + (normals[i] * 0.5f),
			vy + (normals[i + 1] * 0.5f),
			vz + (normals[i + 2]) * 0.5f);
	}

	glColor3f(1.0f, 1.0f, 1.0f);

	glEnd();
}

void Mesh::ToggleVertexNormals()
{
	drawVertexNormals = !drawVertexNormals;
}

void Mesh::ToggleFacesNormals()
{
	drawFaceNormals = !drawFaceNormals;
}
