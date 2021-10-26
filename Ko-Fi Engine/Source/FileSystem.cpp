#include "FileSystem.h"
#include "Log.h"
#include "ImGuiAppLog.h"
#include "SDL_assert.h"
#include "SDL.h"
#include <fstream>
#include <filesystem>
#include <iomanip>

// Assimp (Open Asset Import Library)
//#include "assimp/Importer.hpp"
#include "assimp/cimport.h"     //#include "Assimp/include/cimport.h"
#include "assimp/scene.h"       //#include "Assimp/include/scene.h"
#include "assimp/postprocess.h" //#include "Assimp/include/postprocess.h"

// OpenGL / GLEW
#include "glew.h"
//#include "SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

FileSystem::FileSystem()
{
	name = "ModelLoader";
	std::string rootPathString = SDL_GetBasePath();
	std::replace(rootPathString.begin(), rootPathString.end(), '\\', '/');
	rootPath = rootPathString;
	rootPath = rootPath.parent_path().parent_path();
	AddPath("/Ko-Fi Engine/Ko-Fi");
}

FileSystem::~FileSystem()
{

}

bool FileSystem::Awake()
{
	LOG("Turning on FileSystem debugger...");
	appLog->AddLog("Turning on FileSystem debugger...\n");
	//Prepare filesystem
	//std::filesystem::directory_entry().assign(addPath);
	// Stream log messages to Debug window

	return true;
}

bool FileSystem::Start()
{
	LOG("Starting FileSystem...");
	appLog->AddLog("Starting FileSystem...\n");

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
	LOG("Cleaning FileSystem up...");
	appLog->AddLog("Cleaning FileSystem up...\n");

	// detach log stream
	
	return true;
}

//bool FileSystem::OpenFile(const char* path) const
//{
//	bool ret = true;
//	SDL_assert(path != nullptr);
//	std::ifstream stream(path);
//	stream.is_open() ? ret = true : ret = false;
//	stream.close();
//	return ret;
//}
//
//bool FileSystem::SaveFile(const char* path) const
//{
//	bool ret = true;
//	SDL_assert(path != nullptr);
//	std::ofstream stream(path)
//	stream.is_open() ? ret = true : ret = false;
//	return ret;
//}



void FileSystem::EnumerateFiles(const char* path,std::vector<std::string>&files,std::vector<std::string>&dirs)
{
	std::string p = rootPath.string()+path;
	for (const auto& file : std::filesystem::directory_iterator(p))
	{
		if (std::filesystem::is_directory(file.path())) {
			dirs.push_back(file.path().filename().string());
		}
		else {
			files.push_back(file.path().filename().string());
		}
	}
}

void FileSystem::AddPath(const char* path)
{
	rootPath += path;
}

void FileSystem::LoadMesh(const char* file_path, std::vector<Mesh>& meshes)
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

			// Loading mesh normals data
			if (aiMesh->HasNormals())
			{
				ourMesh.num_normals = aiMesh->mNumVertices;
				ourMesh.normals = new float[ourMesh.num_normals * 3];
				memcpy(ourMesh.normals, aiMesh->mNormals, sizeof(float) * ourMesh.num_normals * 3);
			}

			if (aiMesh->HasTextureCoords(0))
			{
				ourMesh.num_tex_coords = aiMesh->mNumVertices;
				ourMesh.tex_coords = new float[ourMesh.num_tex_coords * 2];
				memcpy(ourMesh.tex_coords, aiMesh->mTextureCoords, sizeof(float) * ourMesh.num_tex_coords * 2);
				for (uint i = 0; i < ourMesh.num_tex_coords; ++i)
				{
					ourMesh.tex_coords[i * 2] = aiMesh->mTextureCoords[0][i].x;
					ourMesh.tex_coords[i * 2 + 1] = aiMesh->mTextureCoords[0][i].y;
				}
			}
			// Add the new mesh to our array of meshes to draw
			meshes.push_back(ourMesh);

			//  Generate new buffer for each mesh send the data to the VRAM
			GenerateMeshesBuffers(meshes);
		}
		aiReleaseImport(scene);
	}
	else
	{
		LOG("Error loading scene % s", file_path);
		appLog->AddLog("Error loading scene % s\n", file_path);
	}
}

void FileSystem::GenerateTextureBuffers(Mesh& mesh) {
	
	
	
}
void FileSystem::GenerateMeshesBuffers(std::vector<Mesh>& meshes)
{
	std::vector<Mesh>::iterator item = meshes.begin();
	while (item != meshes.end())
	{
		GenerateTextureBuffers((Mesh&)*item);
		GenerateMeshBuffer((Mesh&)*item);
		++item;
	}
}

void FileSystem::GenerateMeshBuffer(Mesh& mesh)
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

	glGenBuffers(1, &mesh.id_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.id_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.num_indices, mesh.indices, GL_STATIC_DRAW);

	// Vertex Array Object (VAO) --------------------------------------------------
	glGenVertexArrays(1, &mesh.VAO);
	glBindVertexArray(mesh.VAO);

	// Vertices
	glGenBuffers(1, &mesh.id_vertex);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_vertices * 3, mesh.vertices, GL_STATIC_DRAW);
	// Add vertex position attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	// Normals
	glGenBuffers(1, &mesh.id_normal);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.id_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.num_normals * 3, mesh.normals, GL_STATIC_DRAW);
	// Add normals attribute to the vertex array object (VAO)
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glEnableVertexAttribArray(2);

	//Textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &mesh.id_tex_coord);
	glBindTexture(GL_TEXTURE_2D, mesh.id_tex_coord);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);
	//glGenerateMipmap(GL_TEXTURE_2D);
	
	glGenBuffers(1, &mesh.id_tex_coord);
	glBindTexture(GL_ELEMENT_ARRAY_BUFFER, mesh.id_tex_coord);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(float) * mesh.num_vertices * 2, mesh.tex_coords, GL_STATIC_DRAW);

	// Add texture position attribute to the vertex array object (VAO)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	// Unbind any vertex array we have binded before.
	glBindVertexArray(0);
}
