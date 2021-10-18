#include "FileSystem.h"
#include "Log.h"
#include "ImGuiAppLog.h"

// Assimp (Open Asset Import Library)
//#include "assimp/Importer.hpp"
#include "assimp/cimport.h"     //#include "Assimp/include/cimport.h"
#include "assimp/scene.h"       //#include "Assimp/include/scene.h"
#include "assimp/postprocess.h" //#include "Assimp/include/postprocess.h"

//#pragma comment (lib, "Assimp/libx86/assimp.lib")
//Already included in the project configuration...

// OpenGL / GLEW
#include "glew.h"
#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

FileSystem::FileSystem()
{
	name = "ModelLoader";
}

FileSystem::~FileSystem()
{

}

bool FileSystem::Awake()
{
	LOG("Turning on Assimp debugger...");
	appLog->AddLog("Turning on Assimp debugger...\n");

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return true;
}

bool FileSystem::Start()
{
	LOG("Starting Assimp...");
	appLog->AddLog("Starting Assimp...\n");

	return true;
}

bool FileSystem::PreUpdate(float dt)
{
	return true;
}

bool FileSystem::Update(float dt)
{
	return true;
}

bool FileSystem::PostUpdate(float dt)
{
	return true;
}

bool FileSystem::CleanUp()
{
	LOG("Cleaning Assimp up...");
	appLog->AddLog("Cleaning Assimp up...\n");

	// detach log stream
	aiDetachAllLogStreams();

	return true;
}

void FileSystem::LoadMesh(const char* file_path)
{
	const aiScene* scene = aiImportFile(file_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint j = 0; j < scene->mNumMeshes; ++j)
		{
			Mesh ourMesh;
			aiMesh* aiMesh = scene->mMeshes[j];

			// copy vertices
			ourMesh.num_vertices = aiMesh->mNumVertices;
			ourMesh.vertices = new float[ourMesh.num_vertices * 3];
			memcpy(ourMesh.vertices, aiMesh->mVertices, sizeof(float) * ourMesh.num_vertices * 3);
			LOG("New mesh with %d vertices", ourMesh.num_vertices);
			appLog->AddLog("New mesh with %d vertices\n", ourMesh.num_vertices);

			// copy faces
			if (aiMesh->HasFaces())
			{
				ourMesh.num_indices = aiMesh->mNumFaces * 3;
				ourMesh.indices = new uint[ourMesh.num_indices]; // assume each face is a triangle
				for (uint i = 0; i < aiMesh->mNumFaces; ++i)
				{
					if (aiMesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
						appLog->AddLog("WARNING, geometry face with != 3 indices!\n");
					}
					else
						memcpy(&ourMesh.indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
				}
			}

			// Add the new mesh to our array of meshes to draw
			meshes.push_back(ourMesh);

			//  Generate new buffer for each mesh send the data to the VRAM
			GenerateMeshesBuffers();
		}
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
		appLog->AddLog("Error loading scene % s\n", file_path);
	}
}

void FileSystem::GenerateMeshesBuffers()
{
	std::vector<Mesh>::iterator item = meshes.begin();
	while (item != meshes.end())
	{
		GenerateMeshBuffer((Mesh)*item);
		++item;
	}
}

void FileSystem::GenerateMeshBuffer(Mesh mesh)
{
	// Vertices
	glGenBuffers(1, &mesh.id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertices * 3, mesh.vertices, GL_STATIC_DRAW);

	// Indices
	glGenBuffers(1, &mesh.id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_indices, mesh.indices, GL_STATIC_DRAW);

	// Later: Normals, Colors, etc.
}
