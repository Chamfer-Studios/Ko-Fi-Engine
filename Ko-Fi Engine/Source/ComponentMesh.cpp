#include "ComponentMesh.h"
#include "Globals.h"
#include "Engine.h"
#include "FSDefs.h"
//#include "Color.h"
#include "Camera3D.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "SceneManager.h"
#include "Engine.h"
#include "Renderer3D.h"
#include "Camera3D.h"
#include "PanelChooser.h"
#include "PanelViewport.h"
#include "Editor.h"
#include "EmitterInstance.h"
#include "Log.h"

#include "Importer.h"

#include "I_Mesh.h"
#include "Material.h"

#include "glew.h"
#include <gl/GL.h>

#include <MathGeoLib/Math/float2.h>
#include <MathGeoLib/Math/float3.h>
#include <MathGeoLib/Math/float4.h>


ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
	centerPoint = float3::zero;
	radius = 0.0f;
	mesh = nullptr;
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

bool ComponentMesh::Start()
{
	GenerateLocalBoundingBox();
	//GenerateBounds();
	return true;
}

bool ComponentMesh::Update(float dt)
{
	return true;
}

bool ComponentMesh::PostUpdate(float dt) //AKA the real render
{
	bool ret = true;

	ComponentMaterial* cMat = owner->GetComponent<ComponentMaterial>();

	if (cMat != nullptr && mesh != nullptr)
	{
		if (!cMat->active)
		{
			glDisable(GL_TEXTURE_2D);
		}
		else
		{
			//for (Texture& tex : cMaterial->textures)
			//{
			//	glBindTexture(GL_TEXTURE_2D, tex.textureID);
			//}
			glBindTexture(GL_TEXTURE_2D, cMat->texture.GetTextureId());
		}

		if (renderMesh)
		{
			uint shader = cMat->GetMaterial()->shaderProgramID;

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

			//if (cMat->texture.GetTextureId() == -1 && cMat->GetMaterial()->FindUniform("albedoTint"))
			//{
			//	Color color = cMat->GetMaterial()->diffuseColor;

			//	Uniform* colorUf = cMat->GetMaterial()->FindUniform("albedoTint");
			//	UniformT<float4>* uf = (UniformT<float4>*)colorUf;
			//	uf->value = { color.r, color.g, color.b, color.a };
			//}

			for (Uniform* uniform : cMat->GetMaterial()->uniforms)
			{
				switch (uniform->type)
				{
				case GL_INT:
				{
					glUniform1d(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<int>*)uniform)->value);
				}
				break;
				case GL_FLOAT:
				{
					glUniform1f(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<float>*)uniform)->value);
				}
				break;
				case GL_BOOL:
				{
					glUniform1d(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<bool>*)uniform)->value);
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
				default:
					break;
				}
			}		

			mesh->Draw(owner);

			//draw bounding boxes
			int GOID = 0;

			GOID = owner->GetEngine()->GetEditor()->panelGameObjectInfo.selectedGameObjectID;
			GenerateGlobalBoundingBox();
		
			DrawMouseSelection(); // Draw AABB if Selected with Mosue

			glUseProgram(0);

			owner->GetEngine()->GetRenderer()->RenderAllParticles();
		}
	}
	return ret;
}

bool ComponentMesh::CleanUp()
{
	RELEASE(mesh);

	return true;
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
	if (mesh == nullptr)
	{
		int type = json.at("shape_type");
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

		mesh->meshType = meshType;
	}
	std::string path = json.at("path");
	Importer::GetInstance()->meshImporter->Load(path.c_str(), mesh); // TODO: CHECK IF MESH DATA IS USED
	mesh->path = path;

	SetVertexNormals(json.at("draw_vertex_normals"));
	SetFaceNormals(json.at("draw_face_normals"));
}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	if (this->mesh != nullptr)
		RELEASE(this->mesh);

	this->mesh = mesh;
	GenerateLocalBoundingBox();
}

uint ComponentMesh::GetVertices()
{
	uint numVertices = 0;
	numVertices += mesh->verticesSizeBytes / (sizeof(float) * 3);
	return numVertices;
}

float3 ComponentMesh::GetCenterPointInWorldCoords() const
{
	return owner->GetTransform()->transformMatrix.TransformPos(centerPoint);
}

void ComponentMesh::SetVertexNormals(bool vertexNormals)
{
	this->mesh->SetVertexNormals(vertexNormals);
}

bool ComponentMesh::GetVertexNormals() const
{
	return mesh->GetVertexNormals();
}

void ComponentMesh::SetFaceNormals(bool facesNormals)
{
	this->mesh->SetFaceNormals(facesNormals);
}

bool ComponentMesh::GetFaceNormals() const
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

const AABB ComponentMesh::GetLocalAABB()
{
	GenerateLocalBoundingBox();

	return mesh->localAABB;
}

void ComponentMesh::GenerateGlobalBoundingBox()
{
	// Generate global OBB
	obb.SetFrom(GetLocalAABB());
	obb.Transform(owner->GetTransform()->GetGlobalTransform());

	// Generate global AABB
	aabb.SetNegativeInfinity();
	aabb.Enclose(obb);
}

const AABB ComponentMesh::GetGlobalAABB() const
{
	return aabb;
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
bool ComponentMesh::InspectorDraw(PanelChooser* chooser)
{

	bool ret = true;
	if (mesh != nullptr && ImGui::CollapsingHeader("Mesh"))
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
	return ret;
}
//bool ComponentMesh::GetRenderAABB()
//{
//	return renderAABB;
//}
//
//void ComponentMesh::SetRenderAABB(bool newRenderAABB)
//{
//	this->renderAABB = newRenderAABB;
//}

//AABB ComponentMesh::GetGlobalAABB() const
//{
//	//AABB global = AABB(mesh->localAABB);
//	//global.Translate(owner->GetComponent<ComponentTransform>()->GetPosition());
//	return mesh->localAABB;
//}

void ComponentMesh::DrawMouseSelection()
{
	int selectedId = owner->GetEngine()->GetEditor()->panelGameObjectInfo.selectedGameObjectID;

	if (selectedId == -1) return;


	if ( selectedId == owner->GetUID()) // Draw Selected Object
		DrawBoundingBox(GetLocalAABB(), float3(0.0f, 1.0f, 1.0f));

	//else if(owner->HasParentWithUID(selectedId) && selectedId != owner->GetUID()) // Check if has Parent
	//	DrawBoundingBox(GetLocalAABB(), float3(0.0f, 1.0f, 1.0f));
}

