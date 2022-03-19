#include "Mesh.h"
#include "Texture.h"

#include "GameObject.h"
#include "ComponentMaterial.h"

// OpenGL / GLEW
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <iostream>

#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "Globals.h"

#include "MathGeoLib/Math/float4x4.h"

Mesh::Mesh(Shape shape)
{
	verticesSizeBytes = 0;
	normalsSizeBytes = 0;
	texCoordSizeBytes = 0;
	indicesSizeBytes = 0;
	VAO = 0;

	meshType = shape;

	switch (shape)
	{
	case Shape::CUBE:
		PrimitiveMesh(par_shapes_create_cube());
		break;
	case Shape::CYLINDER:
		PrimitiveMesh(par_shapes_create_cylinder(20, 20));
		break;
	case Shape::SPHERE:
		PrimitiveMesh(par_shapes_create_parametric_sphere(20, 20));
		break;
	case Shape::TORUS:
		PrimitiveMesh(par_shapes_create_torus(20, 20, 0.2));
		break;
	case Shape::PLANE:
		PrimitiveMesh(par_shapes_create_plane(20, 20));
		break;
	case Shape::CONE:
		PrimitiveMesh(par_shapes_create_cone(20, 20));
		break;
	default:
		break;
	}
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);

	// Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glDeleteBuffers(1, &idIndex);
	RELEASE_MALLOC(indices);

	// Vertices
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glDeleteBuffers(1, &idVertex);
	RELEASE_MALLOC(vertices);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, idNormal);
	glDeleteBuffers(1, &idNormal);
	RELEASE_MALLOC(normals);

	// Texture coords
	glBindBuffer(GL_ARRAY_BUFFER, idTexCoord);
	glDeleteBuffers(1, &idTexCoord);
	RELEASE_MALLOC(texCoords);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetUpMeshBuffers()
{
	// Vertex Array Object (VAO)
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Indices
	glGenBuffers(1, &idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSizeBytes, indices, GL_STATIC_DRAW);

	// Vertices
	glGenBuffers(1, &idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, idVertex);
	glBufferData(GL_ARRAY_BUFFER, verticesSizeBytes, vertices, GL_STATIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normals
	glGenBuffers(1, &idNormal);
	glBindBuffer(GL_ARRAY_BUFFER, idNormal);
	glBufferData(GL_ARRAY_BUFFER, normalsSizeBytes, normals, GL_STATIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	// Texture coords
	if (texCoords)
	{
		glGenBuffers(1, &idTexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, idTexCoord);
		glBufferData(GL_ARRAY_BUFFER, texCoordSizeBytes, texCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
	}

	// Unbind any vertex array we have binded before.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::Draw(GameObject* owner)
{
	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indicesSizeBytes / sizeof(uint), GL_UNSIGNED_INT, 0);

	DebugDraw();

	glBindVertexArray(0);

	// Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Mesh::DebugDraw()
{
	// Debug draw
	if (drawVertexNormals)
		DrawVertexNormals();

	if (drawFaceNormals)
		DrawFaceNormals();
}

float* Mesh::GetTransformedVertices(float4x4 transform)
{
	float* ret = (float*)malloc(verticesSizeBytes);

	int nvf = verticesSizeBytes / sizeof(float);
	for (int i = 0; i < nvf; i += 3) {
		float3 v = { vertices[i], vertices[i + 1], vertices[i + 2] };
		float4 e = { v.x, v.y, v.z, 1 };
		float4 t = transform * e;
		float3 homogenized = { t.x / t.w, t.y / t.w, t.z / t.w };
		ret[i] = homogenized.x;
		ret[i + 1] = homogenized.y;
		ret[i + 2] = homogenized.z;
	}

	return ret;
}

Mesh* Mesh::MeshUnion(std::vector<Mesh*> meshes, std::vector<float4x4> transformations)
{
	Mesh* ret = new Mesh();

	int verticesSizeBytes = 0;
	int indicesSizeBytes = 0;
	for (auto m : meshes) {
		verticesSizeBytes += m->verticesSizeBytes;
		indicesSizeBytes += m->indicesSizeBytes;
	}

	ret->vertices = (float*)malloc(verticesSizeBytes);
	ret->indices = (unsigned int*)malloc(indicesSizeBytes);
	ret->verticesSizeBytes = verticesSizeBytes;
	ret->indicesSizeBytes = indicesSizeBytes;

	int indicesOffset = 0;
	std::vector<float> outputVertices;
	std::vector<unsigned int> outputIndices;
	for (int i = 0; i < meshes.size(); i++) {
		Mesh* m = meshes[i];
		float4x4 t = transformations[i];

		float* vertices = m->GetTransformedVertices(t);

		for (int i = 0; i < m->verticesSizeBytes / sizeof(float); i++) {
			outputVertices.push_back(vertices[i]);
		}

		//memcpy(ret->vertices + verticesBytesOffset, vertices, m->verticesSizeBytes);
		//verticesBytesOffset += m->verticesSizeBytes;

		for (int i = 0; i < m->indicesSizeBytes / sizeof(unsigned int); i++) {
			outputIndices.push_back(m->indices[i] + indicesOffset);
		}

		indicesOffset += m->verticesSizeBytes / sizeof(float3);

		free(vertices);
	}

	for (int i = 0; i < outputVertices.size(); i++) {
		ret->vertices[i] = outputVertices[i];
	}

	for (int i = 0; i < outputIndices.size(); i++) {
		ret->indices[i] = outputIndices[i];
	}

	return ret;
}

void Mesh::DrawVertexNormals() const
{
	if (idNormal == -1 || normals == nullptr)
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
	if (idNormal == -1 || normals == nullptr)
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

void Mesh::PrimitiveMesh(par_shapes_mesh* primitiveMesh)
{
	//vertexNum = primitiveMesh->npoints;
	//indexNum = primitiveMesh->ntriangles * 3;
	//normalNum = primitiveMesh->ntriangles;

	vertices = (float*)malloc(primitiveMesh->npoints);
	normals = (float*)malloc(primitiveMesh->ntriangles);
	indices = (uint*)malloc(primitiveMesh->ntriangles * 3);
	texCoords = (float*)malloc(primitiveMesh->npoints);

	verticesSizeBytes = sizeof(float) * primitiveMesh->npoints;
	normalsSizeBytes = sizeof(float) * primitiveMesh->ntriangles;
	texCoordSizeBytes = sizeof(uint) * primitiveMesh->ntriangles * 3;
	indicesSizeBytes = sizeof(float) * primitiveMesh->npoints;

	par_shapes_compute_normals(primitiveMesh);
	for (size_t i = 0; i < primitiveMesh->npoints; ++i)
	{
		memcpy(&vertices[i], &primitiveMesh->points[i * 3], sizeof(float) * 3);
		memcpy(&normals[i], &primitiveMesh->normals[i * 3], sizeof(float) * 3);
		if (primitiveMesh->tcoords != nullptr)
		{
			memcpy(&texCoords[i], &primitiveMesh->tcoords[i * 2], sizeof(float) * 2);
		}
		else if (meshType == Shape::CUBE)
		{
			switch (i % 4)
			{
			case 0:
				texCoords[i] = 0.0f;
				texCoords[i + 1] = 0.0f;
				break;
			case 1:
				texCoords[i] = 1.0f;
				texCoords[i + 1] = 0.0f;
				break;
			case 2:
				texCoords[i] = 1.0f;
				texCoords[i + 1] = 1.0f;
				break;
			case 3:
				texCoords[i] = 0.0f;
				texCoords[i + 1] = 1.0f;
				break;
			}
		}
		else
		{
			texCoords[i] = 0.0f;
			texCoords[i + 1] = 0.0f;
		}
	}

	for (size_t i = 0; i < (primitiveMesh->ntriangles * 3); ++i)
	{
		indices[i] = primitiveMesh->triangles[i];
	}

	memcpy(&normals[0], primitiveMesh->normals, primitiveMesh->npoints);

	par_shapes_free_mesh(primitiveMesh);
}