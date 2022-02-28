#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Material.h"
#include "Engine.h"
#include "Camera3D.h"
#include "PanelChooser.h"
#include "glew.h"
#include <gl/GL.h>
#include "Primitive.h"
#include "par_shapes.h"
#include "Globals.h"
#include "I_Mesh.h"
#include "I_Shader.h"
#include "Importer.h"
#include "FSDefs.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
	centerPoint = float3::zero;
	radius = 0.0f;
	mesh = nullptr;
	SetVertexNormals(false);
	SetFaceNormals(false);
	renderMesh = true;
	time = 0.0f;
}

//ComponentMesh::ComponentMesh(GameObject* parent, Shape shape) : Component(parent)
//{
//	switch (shape)
//	{
//	case Shape::CUBE:
//		CopyParMesh(par_shapes_create_cube());
//		break;
//	case Shape::CYLINDER:
//		CopyParMesh(par_shapes_create_cylinder(20, 20));
//		break;
//	case Shape::SPHERE:
//		CopyParMesh(par_shapes_create_parametric_sphere(20, 20));
//		break;
//	}
//}

////ComponentMesh::ComponentMesh(GameObject* owner,COMPONENT_SUBTYPE subtype) : Component(COMPONENT_TYPE::COMPONENT_MESH)
////{
////	this->subtype = subtype;
////	this->owner = owner;
////}
////
////ComponentMesh::ComponentMesh(GameObject* owner,std::string path) : Component(COMPONENT_TYPE::COMPONENT_MESH)
////{
////	this->path = path;
////	LoadMesh(this->path.c_str());
////	this->subtype = COMPONENT_SUBTYPE::COMPONENT_MESH_MESH;
////	this->owner = owner;
////}

ComponentMesh::~ComponentMesh()
{
	RELEASE(mesh);
	//RELEASE(materialComponent);
}

//void ComponentMesh::CopyParMesh(par_shapes_mesh* parMesh)
//{
//		this->mesh->num_vertices = parMesh->npoints;
//		this->mesh->num_indices = parMesh->ntriangles * 3;
//		this->mesh->num_normals = parMesh->ntriangles;
//		this->mesh->vertices = new float[this->mesh->num_vertices * 3];
//		this->mesh->normals = new float[this->mesh->num_normals * 3];
//		this->mesh->indices = new uint[this->mesh->num_indices];
//		par_shapes_compute_normals(parMesh);
//		for (size_t i = 0; i < mesh->num_vertices; ++i)
//		{
//			memcpy(&mesh->vertices[i], &parMesh->points[i * 3], sizeof(float) * 3);
//			memcpy(&mesh->normals[i], &parMesh->normals[i * 3], sizeof(float) * 3);
//		}
//		for (size_t i = 0; i < mesh->num_indices; ++i)
//		{
//			mesh->indices[i] = parMesh->triangles[i];
//		}
//		memcpy(&mesh->normals[0], parMesh->normals, mesh->num_vertices);
//
//		par_shapes_free_mesh(parMesh);
//
//		mesh->SetUpMeshBuffers();
//		//GenerateBuffers();
//		//ComputeNormals();
//		//GenerateBounds();
//}

float3 ComponentMesh::GetCenterPointInWorldCoords() const
{
	return owner->GetTransform()->transformMatrix.TransformPos(centerPoint);
}

bool ComponentMesh::Start()
{
	GenerateLocalBoundingBox();
	return true;
}

bool ComponentMesh::Update()
{

	return true;
}

bool ComponentMesh::PostUpdate(float dt)
{
	bool ret = true;
	ComponentMaterial* cm = owner->GetComponent<ComponentMaterial>();
	if (cm != nullptr)
	{
		if (!cm->active)
		{
			glDisable(GL_TEXTURE_2D);
		}

		if (renderMesh || owner->GetComponent<ComponentMaterial>()->GetShader() == 0)
		{
			uint shader = owner->GetComponent<ComponentMaterial>()->GetShader()->shaderProgramID;
			
			glUseProgram(shader);
			
			// Matrices
			GLint model_matrix = glGetUniformLocation(shader, "model_matrix");
			glUniformMatrix4fv(model_matrix, 1, GL_FALSE, owner->GetTransform()->GetGlobalTransform().Transposed().ptr());

			GLint projection_location = glGetUniformLocation(shader, "projection");
			glUniformMatrix4fv(projection_location, 1, GL_FALSE, owner->GetEngine()->GetCamera3D()->cameraFrustum.ProjectionMatrix().Transposed().ptr());

			GLint view_location = glGetUniformLocation(shader, "view");
			glUniformMatrix4fv(view_location, 1, GL_FALSE, owner->GetEngine()->GetCamera3D()->viewMatrix.Transposed().ptr());

			GLint refractTexCoord = glGetUniformLocation(shader, "refractTexCoord");
			glUniformMatrix4fv(refractTexCoord, 1, GL_FALSE, owner->GetEngine()->GetCamera3D()->viewMatrix.Transposed().ptr());
			
			float2 resolution = float2(1080.0f, 720.0f);
			glUniform2fv(glGetUniformLocation(shader, "resolution"), 1, resolution.ptr());
			
			this->time += 0.02f;
			glUniform1f(glGetUniformLocation(shader, "time"), this->time);

			ComponentMaterial* cMat = owner->GetComponent<ComponentMaterial>();
			for (Uniform* uniform : cMat->GetShader()->uniforms)
			{
				switch (uniform->type)
				{
				case GL_FLOAT:
				{
					glUniform1f(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<float>*)uniform)->value);
				}
				break;
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
					glUniform2fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf2->value.ptr());
				}
				break;
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uf3 = (UniformT<float3>*)uniform;
					glUniform3fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf3->value.ptr());
				}
				break;
				case GL_FLOAT_VEC4:
				{
					UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
					glUniform4fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf4->value.ptr());
				}
				break;
				case GL_INT:
				{
					glUniform1d(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<int>*)uniform)->value);
				}
				break;
				default:
					break;
				}
			}

			mesh->Draw(owner);
			GenerateGlobalBoundingBox();
			DrawBoundingBox(mesh->localAABB, float3(1.0f, 0.0f, 0.0f));

			glUseProgram(0);
		}
	}
	return ret;
}

bool ComponentMesh::CleanUp()
{
	RELEASE(mesh);

	return true;
}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	if (this->mesh != nullptr)
		RELEASE(this->mesh);

	this->mesh = mesh;
}

Mesh* ComponentMesh::GetMesh()
{
	return mesh;
}

void ComponentMesh::SetPath(std::string path)
{
	this->mesh->path = path;
}

void ComponentMesh::SetVertexNormals(bool vertexNormals)
{
	this->mesh->SetVertexNormals(vertexNormals);
}

void ComponentMesh::SetFaceNormals(bool facesNormals)
{
	this->mesh->SetFaceNormals(facesNormals);
}

const char* ComponentMesh::GetMeshPath()
{
	return mesh->path.c_str();
}

bool ComponentMesh::GetVertexNormals()
{
	return mesh->GetVertexNormals();
}

bool ComponentMesh::GetFaceNormals()
{
	return mesh->GetFaceNormals();
}

void ComponentMesh::GenerateLocalBoundingBox()
{
	// Generate AABB
	if (mesh != nullptr)
	{
		mesh->localAABB.SetNegativeInfinity();
		mesh->localAABB.Enclose((float3*)mesh->vertices, mesh->verticesSizeBytes / (sizeof(float) * 3));

		/*Sphere sphere;
		sphere.r = 0.f;
		sphere.pos = mesh->localAABB.CenterPoint();
		sphere.Enclose(localAABB);

		radius = sphere.r;
		centerPoint = sphere.pos;*/
	}
}

AABB ComponentMesh::GetLocalAABB()
{
	GenerateLocalBoundingBox();
	return mesh->localAABB;
}

void ComponentMesh::Save(Json& json) const
{
	json["type"] = "mesh";

	std::string name = owner->name;
	mesh->path = MESHES_DIR + name + MESH_EXTENSION;

	Importer::GetInstance()->meshImporter->Save(mesh, mesh->path.c_str());

	json["path"] = mesh->path;
	json["shape_type"] = (int)mesh->meshType;
	json["draw_vertex_normals"] = mesh->GetVertexNormals();
	json["draw_face_normals"] = mesh->GetFaceNormals();
}

void ComponentMesh::Load(Json& json)
{
	//Json jsonComponentMesh;
	//Json jsonComponentMaterial;
	//for (const auto& cmp : json.items())
	//{
	//	if (cmp.value().at("component_type") == "mesh")
	//		jsonComponentMesh = cmp.value();
	//	//else if (cmp.value().at("component_type") == "material")
	//	//	jsonComponentMaterial = cmp.value();
	//}
	int type = json["shape_type"];
	Shape meshType = Shape::NONE;
	switch (type)
	{
	case 0:
		meshType = Shape::NONE;
		break;
	case 1:
		meshType = Shape::CUBE;
		break;
	case 2:
		meshType = Shape::SPHERE;
		break;
	case 3:
		meshType = Shape::CYLINDER;
		break;
	case 4:
		meshType = Shape::TORUS;
		break;
	case 5:
		meshType = Shape::PLANE;
		break;
	case 6:
		meshType = Shape::CONE;
		break;
	}
	mesh = new Mesh(meshType);
	std::string path = json["path"];
	Importer::GetInstance()->meshImporter->Load(path.c_str(), mesh);
	mesh->meshType = meshType;
	mesh->path = path;
	SetVertexNormals(json["draw_vertex_normals"]);
	SetFaceNormals(json["draw_face_normals"]);

	//Loading Material
	//if (mesh->texCoords && !jsonComponentMaterial.empty())
	//{
	//	ComponentMaterial* cMat = owner->CreateComponent<ComponentMaterial>();
	//	cMat->GetMaterial()->materialName = jsonComponentMaterial.at("materialName").get<std::string>();
	//	cMat->GetMaterial()->materialPath = jsonComponentMaterial.at("materialPath").get<std::string>();
	//	Shader* shader = cMat->GetShader();
	//	Importer::GetInstance()->shaderImporter->Load(jsonComponentMaterial.at("shaderPath").get<std::string>().c_str(), shader);
	//	for (const auto& tex : jsonComponentMaterial.at("textures").items()) {
	//		cMat->LoadTexture(tex.value().at("path").get<std::string>());
	//	}
	//	for (const auto& uni : jsonComponentMaterial.at("uniforms").items()) {
	//		std::string uniformName = uni.value().at("name").get<std::string>();
	//		uint uniformType = uni.value().at("type").get<uint>();
	//		switch (uniformType) {
	//		case GL_FLOAT:
	//		{
	//			UniformT<float>* uniform = (UniformT<float>*)shader->FindUniform(uniformName);
	//			uniform->value = uni.value().at("value");

	//		}
	//		break;
	//		case GL_FLOAT_VEC2:
	//		{
	//			UniformT<float2>* uniform = (UniformT<float2>*)shader->FindUniform(uniformName);
	//			uniform->value.x = uni.value().at("value").at("x");
	//			uniform->value.y = uni.value().at("value").at("y");

	//		}
	//		break;
	//		case GL_FLOAT_VEC3:
	//		{
	//			UniformT<float3>* uniform = (UniformT<float3>*)shader->FindUniform(uniformName);
	//			uniform->value.x = uni.value().at("value").at("x");
	//			uniform->value.y = uni.value().at("value").at("y");
	//			uniform->value.z = uni.value().at("value").at("z");
	//		}
	//		break;
	//		case GL_FLOAT_VEC4:
	//		{
	//			UniformT<float4>* uniform = (UniformT<float4>*)shader->FindUniform(uniformName);
	//			uniform->value.x = uni.value().at("value").at("x");
	//			uniform->value.y = uni.value().at("value").at("y");
	//			uniform->value.z = uni.value().at("value").at("z");
	//			uniform->value.w = uni.value().at("value").at("w");
	//		}
	//		break;
	//		case GL_INT:
	//		{
	//			UniformT<int>* uniform = (UniformT<int>*)shader->FindUniform(uniformName);
	//			uniform->value = uni.value().at("value");
	//		}
	//		break;
	//		}
	//	}
	//}
	//else if (mesh->texCoords && jsonComponentMaterial.empty())
	//{
	//	ComponentMaterial* cMat = owner->CreateComponent<ComponentMaterial>();
	//}
}


////void ComponentMesh::LoadMesh(const char* path)
////{
////	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
////	if (scene != nullptr && scene->HasMeshes())
////	{
////		materialComponent = new ComponentMaterial(this->owner);
////		for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
////		{
////			Mesh* ourMesh = new Mesh();
////			aiMesh* aiMesh = scene->mMeshes[i];
////
////			// Positions
////			ourMesh->num_vertices = aiMesh->mNumVertices;
////			ourMesh->vertices = new float[ourMesh->num_vertices * 3];
////			memcpy(ourMesh->vertices, aiMesh->mVertices, sizeof(float) * ourMesh->num_vertices * 3); // &vertices[0]
////
////			// Faces
////			if (aiMesh->HasFaces())
////			{
////				ourMesh->num_indices = aiMesh->mNumFaces * 3;
////				ourMesh->indices = new uint[ourMesh->num_indices]; // assume each face is a triangle
////				for (uint i = 0; i < aiMesh->mNumFaces; ++i)
////				{
////					if (aiMesh->mFaces[i].mNumIndices != 3)
////					{
////						/*                       CONSOLE_LOG("WARNING, geometry face with != 3 indices!");
////											   appLog->AddLog("WARNING, geometry face with != 3 indices!\n");*/
////					}
////					else
////						memcpy(&ourMesh->indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
////				}
////			}
////
////			// Loading mesh normals data
////			if (aiMesh->HasNormals())
////			{
////				ourMesh->num_normals = aiMesh->mNumVertices;
////				ourMesh->normals = new float[ourMesh->num_normals * 3];
////				memcpy(ourMesh->normals, aiMesh->mNormals, sizeof(float) * ourMesh->num_normals * 3);
////			}
////
////			// Texture coordinates
////			if (aiMesh->HasTextureCoords(0))
////			{
////				ourMesh->num_tex_coords = aiMesh->mNumVertices;
////				ourMesh->tex_coords = new float[ourMesh->num_tex_coords * 2];
////				for (uint j = 0; j < ourMesh->num_tex_coords; ++j)
////				{
////					ourMesh->tex_coords[j * 2] = aiMesh->mTextureCoords[0][j].x;
////					ourMesh->tex_coords[j * 2 + 1] = /*1.0f - */aiMesh->mTextureCoords[0][j].y;
////				}
////			}
////			else
////				ourMesh->tex_coords = 0;
////
////			ourMesh->SetUpMeshBuffers();
////			materialComponent->AddTextures(ourMesh->texture);
////			meshes.push_back(ourMesh);
////		}
////	}
////
////	aiReleaseImport(scene);
////}

bool ComponentMesh::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Mesh"))
	{
		ImGui::Text("Mesh Path: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		if (ImGui::Selectable(mesh->path.c_str())) {}
		ImGui::PopStyleColor();
		ImGui::Text("Num. vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", GetVertices());
		bool vertex = GetVertexNormals();
		if (ImGui::Checkbox("Vertex Normals", &vertex))
			mesh->SetVertexNormals(vertex);
		bool faces = GetFaceNormals();
		if (ImGui::Checkbox("Faces Normals", &faces))
			mesh->SetFaceNormals(faces);
	}

	//if(materialComponent != nullptr)
	//	materialComponent->InspectorDraw(chooser);

	return ret;
}

uint ComponentMesh::GetVertices()
{
	uint numVertices = 0;
	numVertices += mesh->verticesSizeBytes / (sizeof(float) * 3);
	return numVertices;
}

void ComponentMesh::GenerateGlobalBoundingBox()
{
	// Generate global OBB
	obb = GetLocalAABB();

	// Generate global AABB
	mesh->localAABB.SetNegativeInfinity();
	mesh->localAABB.Enclose(obb);
}

void ComponentMesh::DrawBoundingBox(const AABB& aabb, const float3& rgb)
{
	//float vertices[] = {
	//aabb.MinX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MinX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MinZ(),
	//aabb.MinX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MinX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MinZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MinY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MinZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MaxX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MinY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MaxZ(),
	//aabb.MinX(), aabb.MaxY(), aabb.MinZ(),

	//};
	glLineWidth(2.0f);
	glColor3f(rgb.x, rgb.y, rgb.z);
	glBegin(GL_LINES);

	// Bottom 1
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	// Bottom 2
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	// Top 1
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MinY(), aabb.MinZ());

	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MinZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	// Top 2
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MaxX(), aabb.MaxY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MinY(), aabb.MaxZ());

	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MaxZ());
	glVertex3f(aabb.MinX(), aabb.MaxY(), aabb.MinZ());

	glEnd();
	glColor3f(1.f, 1.f, 1.f);
	glLineWidth(1.0f);
}

AABB ComponentMesh::GetGlobalAABB()
{
	return mesh->localAABB;
}

bool ComponentMesh::GetRenderMesh()
{
	return renderMesh;
}

void ComponentMesh::SetRenderMesh(bool renderMesh)
{
	this->renderMesh = renderMesh;
}