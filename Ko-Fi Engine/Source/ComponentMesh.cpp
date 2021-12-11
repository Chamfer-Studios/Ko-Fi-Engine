#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "PanelChooser.h"
#include "glew.h"
#include <gl/GL.h>
#include "Primitive.h"
#include "par_shapes.h"
#include "Defs.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
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

//
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
////
ComponentMesh::~ComponentMesh()
{

		RELEASE(mesh);

	//RELEASE(materialComponent);
}

//void ComponentMesh::CopyParMesh(par_shapes_mesh* parMesh)
//{
//	
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
//	
//}

bool ComponentMesh::Start(const char* path)
{
	bool ret = true;

	return ret;
}

bool ComponentMesh::PreUpdate()
{
	return true;
}

bool ComponentMesh::Update()
{
	return true;
}

bool ComponentMesh::PostUpdate()
{
	bool ret = true;

	glPushMatrix();
	glMultMatrixf(this->owner->GetTransform()->transformMatrix.Transposed().ptr());
////
////	switch (subtype)
////	{
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_MESH:
////	{
////		
			mesh->Draw(owner);
			/*DrawBoundingBox();*/
			//texture
		
////		
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_CUBE:
////	{
////		Cube cube(1, 1, 1);
////		cube.DrawInterleavedMode();
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_SPHERE:
////	{
////		Sphere sphere(1, 25, 25);
////		sphere.InnerRender();
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_CYLINDER:
////	{
////		Cylinder cylinder(1, 1);
////		cylinder.InnerRender();
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_LINE:
////	{
////		Line line(1, 1, 1);
////		line.InnerRender();
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_PLANE:
////	{
////		Plane plane(0, 1, 0, 0);
////		plane.axis = true;
////		plane.Render();
////		break;
////	}
////	case COMPONENT_SUBTYPE::COMPONENT_MESH_PYRAMID:
////	{
////		Pyramid pyramid(1,1,1);
////		pyramid.InnerRender();
////		break;
////	}
////	default:
////		break;
////	}
////
	glPopMatrix();
////

	GetGlobalBoundingBox();
	DrawBoundingBox(aabb, float3(0.0, 1.0, 0.0));

	return ret;
}

bool ComponentMesh::CleanUp()
{
	
	RELEASE(mesh);

	return true;
}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	if (this->mesh != nullptr) {
		RELEASE(this->mesh);
	}
	this->mesh = mesh;
}

Mesh* ComponentMesh::GetMesh()
{
	return mesh;
}

void ComponentMesh::SetPath(std::string path)
{
	this->path = path;
}

void ComponentMesh::SetVertexNormals(bool vertexNormals)
{
	this->vertexNormals = vertexNormals;
}

void ComponentMesh::SetFacesNormals(bool facesNormals)
{
	this->facesNormals = facesNormals;
}

GameObject* ComponentMesh::GetParent()
{
	return owner;
}

std::string ComponentMesh::GetPath()
{
	return path;
}

bool ComponentMesh::GetVertexNormals()
{
	return vertexNormals;
}

bool ComponentMesh::GetFacesNormals()
{
	return facesNormals;
}

void ComponentMesh::GenerateBounds()
{
	// Generate AABB
	mesh->localAABB.SetNegativeInfinity();
	mesh->localAABB.Enclose((float3*)mesh->vertices, mesh->verticesSizeBytes/(sizeof(float)*3));

	/*Sphere sphere;
	sphere.r = 0.f;
	sphere.pos = mesh->localAABB.CenterPoint();
	sphere.Enclose(mesh->localAABB);

	radius = sphere.r;
	centerPoint = sphere.pos;*/
}

AABB ComponentMesh::GetAABB()
{
	GenerateBounds();
	return mesh->localAABB;
}

////
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
////
bool ComponentMesh::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;

	if (ImGui::CollapsingHeader("Mesh")) {
		ImGui::Text("Mesh Path: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		if (ImGui::Selectable(path.c_str())) {}
		ImGui::PopStyleColor();
		ImGui::Text("Num. vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", GetVertices());
		if (ImGui::Checkbox("Vertex Normals", &vertexNormals))
				mesh->ToggleVertexNormals();
		if (ImGui::Checkbox("Faces Normals", &facesNormals))
				mesh->ToggleFacesNormals();
	}

	//if(materialComponent != nullptr)
	//	materialComponent->InspectorDraw(chooser);

	return ret;
}

uint ComponentMesh::GetVertices()
{
	uint numVertices = 0;
	
		numVertices += mesh->verticesSizeBytes/(sizeof(float)*3);
	
	return numVertices;
}

void ComponentMesh::GetGlobalBoundingBox()
{
	// Generate global OBB
	obb = GetAABB();
	obb.Transform(owner->GetTransform()->GetGlobalTransform());

	// Generate global AABB
	aabb.SetNegativeInfinity();
	aabb.Enclose(obb);
}

void ComponentMesh::DrawBoundingBox(const AABB& aabb, const float3& rgb)
{
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