#include "GameObject.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GameObject::GameObject(const char* path, uint id)
{
    LoadModel(path);
    name = "GameObject" + std::to_string(id);
    this->id = id;
}

void GameObject::Draw()
{
    for(Mesh var : meshes)
    {
        var.Draw();
    }
}

void GameObject::LoadModel(std::string path)
{
    LoadMesh(path);
}

void GameObject::LoadMesh(std::string path)
{
    const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene != nullptr && scene->HasMeshes())
    {
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            Mesh ourMesh;
            aiMesh* aiMesh = scene->mMeshes[i];

            //positions
            ourMesh.num_vertices = aiMesh->mNumVertices;
            ourMesh.vertices = new float[ourMesh.num_vertices * 3];
            memcpy(ourMesh.vertices, aiMesh->mVertices, sizeof(float) * ourMesh.num_vertices * 3);
           
            // copy faces
            if (aiMesh->HasFaces())
            {
                ourMesh.num_indices = aiMesh->mNumFaces * 3;
                ourMesh.indices = new uint[ourMesh.num_indices]; // assume each face is a triangle
                for (uint i = 0; i < aiMesh->mNumFaces; ++i)
                {
                    if (aiMesh->mFaces[i].mNumIndices != 3)
                    {
                        /*                       LOG("WARNING, geometry face with != 3 indices!");
                                               appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
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

            // Texture coordinates
            if (aiMesh->HasTextureCoords(0))
            {
                ourMesh.num_tex_coords = aiMesh->mNumVertices;
                ourMesh.tex_coords = new float[ourMesh.num_tex_coords * 2];
                /*memcpy(ourMesh.tex_coords_3D, aiMesh->mTextureCoords, sizeof(float) * ourMesh.num_tex_coords * 2);*/
                for (uint j = 0; j < ourMesh.num_tex_coords; ++j)
                {
                    ourMesh.tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
                    ourMesh.tex_coords[j * 2 + 1] = aiMesh->mTextureCoords[0][j].y;
                }
            }
            else
                ourMesh.tex_coords = 0;

            ourMesh.SetUpMesh();
            meshes.push_back(ourMesh);

        }
    }

    aiReleaseImport(scene);
}

void GameObject::SetName(std::string name)
{
    this->name = name;
}

std::string GameObject::GetName()
{
    return name;
}

std::vector<Mesh>* GameObject::GetMeshes()
{
    return &meshes;
}

void GameObject::SetNewTexture(const char* path)
{
    for(Mesh mesh : meshes)
    {
        mesh.SetUpTexture(path);
    }
}
