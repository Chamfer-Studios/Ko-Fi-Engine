#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "glew.h"
#include <gl/GL.h>
#include "Primitive.h"

ComponentMesh::ComponentMesh(COMPONENT_SUBTYPE subtype) : Component(COMPONENT_TYPE::COMPONENT_MESH)
{
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

	switch (subtype)
	{
	case COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE:
	{
		Cube cube(1, 1, 1);
		cube.DrawInterleavedMode();
		break;
	}
	case COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE:
	{
		Sphere sphere(1, 25, 25);
		sphere.InnerRender();
		break;
	}
	case COMPONENT_SUBTYPE::COMPONENT_MESH_CYLINDER:
	{
		break;
	}
	case COMPONENT_SUBTYPE::COMPONENT_MESH_LINE:
	{
		break;
	}
	case COMPONENT_SUBTYPE::COMPONENT_MESH_PLANE:
	{
		Plane p(0, 1, 0, 0);
		p.Render();
		break;
	}
	case COMPONENT_SUBTYPE::COMPONENT_MESH_PYRAMID:
	{
		Pyramid pyramid(1,1,1);
		pyramid.InnerRender();
		break;
	}
	default:
		break;
	}

	if (subtype == COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE)
	{
		Cube cube(1, 1, 1);
		cube.DrawInterleavedMode();
	}
	else
	{
		for (Mesh* mesh : meshes)
		{
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