#include "I_Mesh.h"
#include "Globals.h"
#include "Log.h"
#include "R_Mesh.h"
#include "Engine.h"
#include "M_FileSystem.h"
#include "FSDefs.h"

#include <fstream>

I_Mesh::I_Mesh(KoFiEngine* engine) : engine(engine)
{
}

I_Mesh::~I_Mesh()
{
}

bool I_Mesh::Import(const aiMesh* aiMesh, R_Mesh* mesh, const aiScene* assimpScene)
{
	if (mesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Mesh! Error: R_Mesh* was nullptr.");
		return false;
	}
	if (aiMesh == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Mesh! Error: aiMesh* was nullptr.");
		return false;
	}

	// Loading the data from the mesh into the corresponding vectors
	if (aiMesh->HasPositions())
	{
		// Positions
		mesh->verticesSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		mesh->vertices = (float*)malloc(mesh->verticesSizeBytes);
		memcpy(mesh->vertices, aiMesh->mVertices, mesh->verticesSizeBytes); // &vertices[0]

		CONSOLE_LOG("[STATUS] Imported %u position vertices!", aiMesh->mNumVertices * 3);
	}

	if (aiMesh->HasFaces())
	{
		mesh->indicesSizeBytes = aiMesh->mNumFaces * sizeof(uint) * 3;
		mesh->indices = (uint*)malloc(mesh->indicesSizeBytes); // Assume each face is a triangle

		for (uint i = 0; i < aiMesh->mNumFaces; ++i)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
				CONSOLE_LOG("[WARNING] Geometry face %u with != 3 indices!", i);
			else
				memcpy(&mesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	}

	if (aiMesh->HasNormals())
	{
		mesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;
		mesh->normals = (float*)malloc(mesh->normalsSizeBytes);
		memcpy(mesh->normals, aiMesh->mNormals, mesh->normalsSizeBytes);

		CONSOLE_LOG("[STATUS] Imported %u normals!", aiMesh->mNumVertices * 3);
	}

	if (aiMesh->HasTextureCoords(0))
	{
		mesh->texCoordSizeBytes = aiMesh->mNumVertices * sizeof(float) * 2;
		mesh->texCoords = (float*)malloc(mesh->texCoordSizeBytes);
		for (uint j = 0; j < aiMesh->mNumVertices; ++j)
		{
			mesh->texCoords[j * 2] = aiMesh->mTextureCoords[0][j].x;
			mesh->texCoords[j * 2 + 1] = 1.0f - aiMesh->mTextureCoords[0][j].y;
		}

		CONSOLE_LOG("[STATUS] Imported %u texture coordinates!", aiMesh->mNumVertices * 2);
	}
	else
		mesh->texCoords = 0;

	if (aiMesh->HasBones())
	{
		uint numVertices = mesh->verticesSizeBytes / (sizeof(float) * 3);
		mesh->bones.resize(numVertices);
		// Load mesh bones
		for (int i = 0; i < aiMesh->mNumBones; i++)
		{
			// Load single bone
			aiBone* bone = aiMesh->mBones[i];
			int boneId = GetBoneId(bone, mesh->boneNameToIndexMap);
			if (boneId == mesh->boneInfo.size())
			{
				float4x4 offsetmatrix = aiMatrix2Float4x4(bone->mOffsetMatrix);
				BoneInfo bi(offsetmatrix);
				mesh->boneInfo.push_back(bi);
			}
			for (int j = 0; j < bone->mNumWeights; j++)
			{
				const aiVertexWeight& vw = bone->mWeights[j];
				uint globalVertexID = bone->mWeights[j].mVertexId;
				mesh->bones[globalVertexID].AddBoneData(boneId, vw.mWeight);
				CONSOLE_LOG("%d: vertex id %d weight %.2f\n", j, vw.mVertexId, vw.mWeight);
			}
		}
	}

	mesh->SetUpMeshBuffers();

	return true;
}

bool I_Mesh::Save(const R_Mesh* mesh, const char* path)
{
	if (engine->GetFileSystem()->CheckDirectory(MESHES_DIR))
	{
		std::ofstream file;
		file.open(path, std::ios::in | std::ios::trunc | std::ios::binary);
		if (file.is_open())
		{
			// HEADER
			file.write((char*)&mesh->indicesSizeBytes, sizeof(unsigned));
			file.write((char*)&mesh->verticesSizeBytes, sizeof(unsigned));
			file.write((char*)&mesh->normalsSizeBytes, sizeof(unsigned));
			file.write((char*)&mesh->texCoordSizeBytes, sizeof(unsigned));

			file.write((char*)mesh->indices, mesh->indicesSizeBytes);			// Indices

			file.write((char*)mesh->vertices, mesh->verticesSizeBytes);			// Vertices

			file.write((char*)mesh->normals, mesh->normalsSizeBytes);			// Normals

			if (mesh->texCoordSizeBytes != 0)
				file.write((char*)mesh->texCoords, mesh->texCoordSizeBytes);	// Texture coordinates

			/*bool isAnimated = false;*/
			if (mesh->IsAnimated())
			{
				/*isAnimated = true;
				file.write((char*)&isAnimated, sizeof(bool));*/

				uint boneInfoSize = mesh->boneInfo.size();
				uint bonesSize = mesh->bones.size();
				uint boneNameToIndexMapSize = mesh->boneNameToIndexMap.size();
				uint boneInfoSizeBytes = mesh->boneInfo.size() * sizeof(BoneInfo);
				uint bonesSizeBytes = mesh->bones.size() * sizeof(VertexBoneData);
				uint boneNameToIndexMapSizeBytes = sizeof(mesh->boneNameToIndexMap);
				file.write((char*)&boneInfoSize, sizeof(uint));					// Bone info size
				file.write((char*)&bonesSize, sizeof(uint));					// Bones size
				file.write((char*)&boneNameToIndexMapSize, sizeof(uint));		// Bone name to index map size
				file.write((char*)&boneInfoSizeBytes, sizeof(uint));			// Bone info size bytes
				file.write((char*)&bonesSizeBytes, sizeof(uint));				// Bones size bytes
				file.write((char*)&boneNameToIndexMapSizeBytes, sizeof(uint));	// Bone name to index map size bytes
				for (BoneInfo boneInfo : mesh->boneInfo)						// Bone info
				{
					float4x4 offsetMatrix = boneInfo.offsetMatrix;
					file.write((char*)&offsetMatrix[0][0], sizeof(float));
					file.write((char*)&offsetMatrix[0][1], sizeof(float));
					file.write((char*)&offsetMatrix[0][2], sizeof(float));
					file.write((char*)&offsetMatrix[0][3], sizeof(float));
					file.write((char*)&offsetMatrix[1][0], sizeof(float));
					file.write((char*)&offsetMatrix[1][1], sizeof(float));
					file.write((char*)&offsetMatrix[1][2], sizeof(float));
					file.write((char*)&offsetMatrix[1][3], sizeof(float));
					file.write((char*)&offsetMatrix[2][0], sizeof(float));
					file.write((char*)&offsetMatrix[2][1], sizeof(float));
					file.write((char*)&offsetMatrix[2][2], sizeof(float));
					file.write((char*)&offsetMatrix[2][3], sizeof(float));
					file.write((char*)&offsetMatrix[3][0], sizeof(float));
					file.write((char*)&offsetMatrix[3][1], sizeof(float));
					file.write((char*)&offsetMatrix[3][2], sizeof(float));
					file.write((char*)&offsetMatrix[3][3], sizeof(float));

					float4x4 finalTransformation = boneInfo.finalTransformation;
					file.write((char*)&finalTransformation[0][0], sizeof(float));
					file.write((char*)&finalTransformation[0][1], sizeof(float));
					file.write((char*)&finalTransformation[0][2], sizeof(float));
					file.write((char*)&finalTransformation[0][3], sizeof(float));
					file.write((char*)&finalTransformation[1][0], sizeof(float));
					file.write((char*)&finalTransformation[1][1], sizeof(float));
					file.write((char*)&finalTransformation[1][2], sizeof(float));
					file.write((char*)&finalTransformation[1][3], sizeof(float));
					file.write((char*)&finalTransformation[2][0], sizeof(float));
					file.write((char*)&finalTransformation[2][1], sizeof(float));
					file.write((char*)&finalTransformation[2][2], sizeof(float));
					file.write((char*)&finalTransformation[2][3], sizeof(float));
					file.write((char*)&finalTransformation[3][0], sizeof(float));
					file.write((char*)&finalTransformation[3][1], sizeof(float));
					file.write((char*)&finalTransformation[3][2], sizeof(float));
					file.write((char*)&finalTransformation[3][3], sizeof(float));
				}
				for (VertexBoneData vertexBoneData : mesh->bones)
				{
					file.write((char*)&vertexBoneData.boneIDs[0], MAX_NUM_BONES_PER_VERTEX * sizeof(uint));
					file.write((char*)&vertexBoneData.weights[0], MAX_NUM_BONES_PER_VERTEX * sizeof(float));
				}
				for (const auto& it : mesh->boneNameToIndexMap)
				{
					uint nameSizeBytes = it.first.length() * 4;
					file.write((char*)&nameSizeBytes, sizeof(uint));
					file.write((char*)it.first.data(), nameSizeBytes);
					file.write((char*)&it.second, sizeof(uint));
				}
			}
			/*else
				file.write((char*)&isAnimated, sizeof(bool));*/

			file.close();

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Mesh Save: directory %s couldn't be accessed.", MESHES_DIR);

	return false;
}

bool I_Mesh::Load(const char* path, R_Mesh* mesh)
{
	if (engine->GetFileSystem()->CheckDirectory(MESHES_DIR))
	{
		std::ifstream file;
		file.open(path, std::ios::binary);
		if (file.is_open())
		{
			file.read((char*)&mesh->indicesSizeBytes, sizeof(unsigned));
			file.read((char*)&mesh->verticesSizeBytes, sizeof(unsigned));
			file.read((char*)&mesh->normalsSizeBytes, sizeof(unsigned));
			file.read((char*)&mesh->texCoordSizeBytes, sizeof(unsigned));

			mesh->indices = (uint*)malloc(mesh->indicesSizeBytes);
			file.read((char*)mesh->indices, mesh->indicesSizeBytes);

			mesh->vertices = (float*)malloc(mesh->verticesSizeBytes);
			file.read((char*)mesh->vertices, mesh->verticesSizeBytes);

			mesh->normals = (float*)malloc(mesh->normalsSizeBytes);
			file.read((char*)mesh->normals, mesh->normalsSizeBytes);

			if (mesh->texCoordSizeBytes != 0)
			{
				mesh->texCoords = (float*)malloc(mesh->texCoordSizeBytes);
				file.read((char*)mesh->texCoords, mesh->texCoordSizeBytes);
			}

			/*bool isAnimated = false;
			file.read((char*)&isAnimated, sizeof(bool));
			mesh->SetIsAnimated(isAnimated);*/

			if (mesh->IsAnimated())
			{
				uint boneInfoSize;
				uint bonesSize;
				uint boneNameToIndexMapSize;
				uint boneInfoSizeBytes;
				uint bonesSizeBytes;
				uint boneNameToIndexMapSizeBytes;
				file.read((char*)&boneInfoSize, sizeof(uint));
				file.read((char*)&bonesSize, sizeof(uint));
				file.read((char*)&boneNameToIndexMapSize, sizeof(uint));
				file.read((char*)&boneInfoSizeBytes, sizeof(uint));
				file.read((char*)&bonesSizeBytes, sizeof(uint));
				file.read((char*)&boneNameToIndexMapSizeBytes, sizeof(uint));
				mesh->boneInfo.clear();
				mesh->boneInfo.resize(boneInfoSize);
				mesh->bones.clear();
				mesh->bones.resize(bonesSize);
				mesh->boneNameToIndexMap.clear();
				for (int i = 0; i < boneInfoSize; ++i)
				{
					float4x4 offsetMatrix;
					file.read((char*)&offsetMatrix[0][0], sizeof(float));
					file.read((char*)&offsetMatrix[0][1], sizeof(float));
					file.read((char*)&offsetMatrix[0][2], sizeof(float));
					file.read((char*)&offsetMatrix[0][3], sizeof(float));
					file.read((char*)&offsetMatrix[1][0], sizeof(float));
					file.read((char*)&offsetMatrix[1][1], sizeof(float));
					file.read((char*)&offsetMatrix[1][2], sizeof(float));
					file.read((char*)&offsetMatrix[1][3], sizeof(float));
					file.read((char*)&offsetMatrix[2][0], sizeof(float));
					file.read((char*)&offsetMatrix[2][1], sizeof(float));
					file.read((char*)&offsetMatrix[2][2], sizeof(float));
					file.read((char*)&offsetMatrix[2][3], sizeof(float));
					file.read((char*)&offsetMatrix[3][0], sizeof(float));
					file.read((char*)&offsetMatrix[3][1], sizeof(float));
					file.read((char*)&offsetMatrix[3][2], sizeof(float));
					file.read((char*)&offsetMatrix[3][3], sizeof(float));
					mesh->boneInfo.at(i).offsetMatrix = offsetMatrix;

					float4x4 finalTransformation;
					file.read((char*)&finalTransformation[0][0], sizeof(float));
					file.read((char*)&finalTransformation[0][1], sizeof(float));
					file.read((char*)&finalTransformation[0][2], sizeof(float));
					file.read((char*)&finalTransformation[0][3], sizeof(float));
					file.read((char*)&finalTransformation[1][0], sizeof(float));
					file.read((char*)&finalTransformation[1][1], sizeof(float));
					file.read((char*)&finalTransformation[1][2], sizeof(float));
					file.read((char*)&finalTransformation[1][3], sizeof(float));
					file.read((char*)&finalTransformation[2][0], sizeof(float));
					file.read((char*)&finalTransformation[2][1], sizeof(float));
					file.read((char*)&finalTransformation[2][2], sizeof(float));
					file.read((char*)&finalTransformation[2][3], sizeof(float));
					file.read((char*)&finalTransformation[3][0], sizeof(float));
					file.read((char*)&finalTransformation[3][1], sizeof(float));
					file.read((char*)&finalTransformation[3][2], sizeof(float));
					file.read((char*)&finalTransformation[3][3], sizeof(float));
					mesh->boneInfo.at(i).finalTransformation = finalTransformation;
				}
				for (int i = 0; i < bonesSize; ++i)
				{
					file.read((char*)&mesh->bones.at(i).boneIDs[0], MAX_NUM_BONES_PER_VERTEX * sizeof(uint));
					file.read((char*)&mesh->bones.at(i).weights[0], MAX_NUM_BONES_PER_VERTEX * sizeof(float));
				}
				for (int z = 0; z < boneNameToIndexMapSize; ++z)
				{
					uint nameSizeBytes;
					std::string name;
					uint index;
					file.read((char*)&nameSizeBytes, sizeof(uint));
					name.resize(nameSizeBytes);
					file.read((char*)(name.data()), nameSizeBytes);
					file.read((char*)&index, sizeof(uint));
					mesh->boneNameToIndexMap.emplace(name.c_str(), index + 1);
				}
			}

			file.close();

			mesh->SetUpMeshBuffers();

			return true;
		}
	}
	else
		CONSOLE_LOG("[ERROR] Mesh Load: directory %s couldn't be accessed.", MESHES_DIR);

	return false;
}

int I_Mesh::GetBoneId(const aiBone* pBone, std::map<std::string, uint>& boneNameToIndexMap)
{
	int boneIndex = 0;
	std::string boneName(pBone->mName.C_Str());
	if (boneNameToIndexMap.find(boneName) == boneNameToIndexMap.end())
	{
		boneIndex = (int)boneNameToIndexMap.size();
		boneNameToIndexMap[boneName] = boneIndex;
	}
	else
	{
		boneIndex = boneNameToIndexMap[boneName];
	}
	return boneIndex;
}
