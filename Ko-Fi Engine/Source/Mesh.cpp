#include "Mesh.h"

// OpenGL / GLEW
#include "glew.h"
//#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>
#include "ComponentMaterial.h"
#include "GameObject.h"
#include "Defs.h"

Mesh::Mesh()
{
	verticesSizeBytes = 0;
	normalsSizeBytes = 0;
	texCoordSizeBytes = 0;
	indicesSizeBytes = 0;
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glDeleteBuffers(1, &id_vertex);
	RELEASE_MALLOC(vertices);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glDeleteBuffers(1, &id_normal);
	RELEASE_MALLOC(normals);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glDeleteBuffers(1, &id_index);
	RELEASE_MALLOC(indices);

	// Texture coords
	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	glDeleteBuffers(1, &id_tex_coord);
	RELEASE_MALLOC(tex_coords);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

void Mesh::SetUpMeshBuffers()
{
	//SetUpDefaultTexture();
	//// Vertices
	//glGenBuffers(1, &id_vertex);
	//glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	//glBufferData(GL_ARRAY_BUFFER, verticesSizeBytes, vertices, GL_STATIC_DRAW);

	//// Normals
	//glGenBuffers(1, &id_normal);
	//glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	//glBufferData(GL_ARRAY_BUFFER, normalsSizeBytes, normals, GL_STATIC_DRAW);

	//// Indices
	//glGenBuffers(1, &id_index);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSizeBytes, indices, GL_STATIC_DRAW);

	//// Texture coords
	//if (tex_coords) {
	//	glGenBuffers(1, &id_tex_coord);
	//	glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
	//	glBufferData(GL_ARRAY_BUFFER, texCoordSizeBytes, tex_coords, GL_STATIC_DRAW);
	//}

	// Vertex Array Object (VAO) --------------------------------------------------
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Indices
	glGenBuffers(1, &id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSizeBytes, indices, GL_STATIC_DRAW);

	// Vertices
	glGenBuffers(1, &id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertex);
	glBufferData(GL_ARRAY_BUFFER, verticesSizeBytes, vertices, GL_STATIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normals
	glGenBuffers(1, &id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, id_normal);
	glBufferData(GL_ARRAY_BUFFER, normalsSizeBytes, normals, GL_STATIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Texture coords
	if (tex_coords)
	{
		glGenBuffers(1, &id_tex_coord);
		glBindBuffer(GL_ARRAY_BUFFER, id_tex_coord);
		glBufferData(GL_ARRAY_BUFFER, texCoordSizeBytes, tex_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Unbind any vertex array we have binded before.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetUpDefaultTexture()
{
	
}

void Mesh::Draw(GameObject* owner)
{
	//Texture
	if (ComponentMaterial* material = owner->GetComponent<ComponentMaterial>()) {
		glBindTexture(GL_TEXTURE_2D, material->GetTexture().GetTextureId());
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicesSizeBytes / sizeof(uint), GL_UNSIGNED_INT, 0);
	DebugDraw();

	//Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);

	
	

	
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
	for (size_t i = 0, c = 0; i < verticesSizeBytes / sizeof(float); i += 3, c += 4)
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
	for (size_t i = 0; i < verticesSizeBytes / sizeof(float); i += 3)
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

