#include "Mesh.h"

// OpenGL / GLEW
#include "glew.h"
//#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::SetUpMesh()
{
	/*SetUpTexture();*/

	// Vertex Array Object (VAO) --------------------------------------------------
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Vertices
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, vertices, GL_STATIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	// Texture coords
	glGenBuffers(1, &id_tex_coord);
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_tex_coords * 2, tex_coords, GL_STATIC_DRAW);
	// Add texture coords attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

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
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(textureID, 0);

	// Normals
	glGenBuffers(1, &id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(2);

	// Indices
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);

	// Unbind any vertex array we have binded before.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetUpTexture()
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
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
}

void Mesh::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	// Draw mesh
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glActiveTexture(textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);

	// Unbind buffers
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_VERTEX_ARRAY, 0);
	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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
		glColor3f(0.85f, 0.0f, 0.85f);
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
		glColor3f(0.0f, 0.85f, 1.0f);
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
