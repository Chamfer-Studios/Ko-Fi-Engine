#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glew.h"
#include "Primitive.h"
#include <gl/GL.h>
ComponentMesh::ComponentMesh(COMPONENT_SUBTYPE subtype) : Component(COMPONENT_TYPE::COMPONENT_MESH)
{
	LoadPrimitive(subtype);
	this->subtype = subtype;
}

ComponentMesh::ComponentMesh(const char* path) : Component(COMPONENT_TYPE::COMPONENT_MESH)
{
	this->path = path;
	LoadMesh(this->path);
	this->subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
}

ComponentMesh::~ComponentMesh()
{
}

bool ComponentMesh::Start(const char* path)
{
	bool ret = true;

	return ret;
}

bool ComponentMesh::PostUpdate()
{
	bool ret = true;

	if (subtype == COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE)
	{
		Cube cube(6, 6, 6);
		cube.DrawInterleavedMode();
	}
	else {
		for (Mesh* mesh : meshes) {
			mesh->Draw();
		}
	}

	return ret;
}

void ComponentMesh::LoadMesh(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		materialComponent = new ComponentMaterial();
		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
		{
			Mesh* ourMesh = new Mesh();
			aiMesh* aiMesh = scene->mMeshes[i];

			//positions
			ourMesh->num_vertices = aiMesh->mNumVertices;
			ourMesh->vertices = new float[ourMesh->num_vertices * 3];
			memcpy(ourMesh->vertices, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertices * 3);

			// copy faces
			if (aiMesh->HasFaces())
			{
				ourMesh->num_indices = aiMesh->mNumFaces * 3;
				ourMesh->indices = new uint[ourMesh->num_indices]; // assume each face is a triangle
				for (uint i = 0; i < aiMesh->mNumFaces; ++i)
				{
					if (aiMesh->mFaces[i].mNumIndices != 3)
					{
						/*                       LOG("WARNING, geometry face with != 3 indices!");
											   appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
					}
					else
						memcpy(&ourMesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}

			// Loading mesh normals data
			if (aiMesh->HasNormals())
			{
				ourMesh->num_normals = aiMesh->mNumVertices;
				ourMesh->normals = new float[ourMesh->num_normals * 3];
				memcpy(ourMesh->normals, aiMesh->mNormals, sizeof(float) * ourMesh->num_normals * 3);
			}

			// Texture coordinates
			if (aiMesh->HasTextureCoords(0))
			{
				ourMesh->num_tex_coords = aiMesh->mNumVertices;
				ourMesh->tex_coords = new float[ourMesh->num_tex_coords * 2];
				for (uint j = 0; j < ourMesh->num_tex_coords; ++j)
				{
					ourMesh->tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
					ourMesh->tex_coords[j * 2 + 1] = /*1.0f - */aiMesh->mTextureCoords[0][j].y;
				}
			}
			else
				ourMesh->tex_coords = 0;

			ourMesh->SetUpMeshBuffers();
			materialComponent->AddTextureId(ourMesh->textureID);
			meshes.push_back(ourMesh);

		}
	}

	aiReleaseImport(scene);
}

bool ComponentMesh::InspectorDraw()
{
	bool ret = true;
	if (ImGui::CollapsingHeader("Mesh")) {
		ImGui::Text("Mesh Path: ");
		ImGui::SameLine();
		if (ImGui::Selectable(path))
		{
		}
	}

	materialComponent->InspectorDraw();
	return ret;
}

void ComponentMesh::LoadPrimitive(COMPONENT_SUBTYPE subtype)
{
	switch (subtype)
	{
	case COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE:
		materialComponent = new ComponentMaterial();
		break;
	}
}

void ComponentMesh::DrawCube() {
	float sx = 1.0f;
	float sy = 1.0f;
	float sz = 1.0f;

	// interleaved vertex array for glDrawElements() & glDrawRangeElements() ======
	// All vertex attributes (position, normal, color) are packed together as a
	// struct or set, for example, ((V,N,C), (V,N,C), (V,N,C),...).
	// It is called an array of struct, and provides better memory locality.
	GLfloat vertices[] = { sx, sy, sz,   0, 0, 1,   1, 1, 1,              // v0 (front)
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
	GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
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