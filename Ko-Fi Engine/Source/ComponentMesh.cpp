#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"
#include "Engine.h"
#include "Camera3D.h"
#include "PanelChooser.h"
#include "glew.h"
#include <gl/GL.h>
//#include "Primitive.h"
#include "par_shapes.h"
#include "Defs.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
	this->time = 0.0f;
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
	//GenerateLocalBoundingBox();
	GenerateBounds();
	return true;
}

bool ComponentMesh::Update()
{

	return true;
}

bool ComponentMesh::PostUpdate(float dt)
{
	bool ret = true;
	if (renderMesh) {
		

		uint shader = 
			owner->GetComponent<ComponentMaterial>()->GetShader();
		glUseProgram(shader);
		//Matrices
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
		
		Material mat = owner->GetComponent<ComponentMaterial>()->GetMaterial();
		for (Uniform* uniform : mat.uniforms) {
			switch (uniform->type) {
			case GL_FLOAT:
			{
				if(uniform->name != "time")
				glUniform1f(glGetUniformLocation(shader, uniform->name.c_str()), ((UniformT<float>*)uniform)->value);

			}
				break;
			case GL_FLOAT_VEC2:
			{
				if (uniform->name != "resolution")
				{
					UniformT<float2>* uf2 = (UniformT<float2>*)uniform;
					glUniform2fv(glGetUniformLocation(shader, uniform->name.c_str()), 1, uf2->value.ptr());
				}
				
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

			break;
			}
		}

		mesh->Draw(owner);
		//GenerateGlobalBoundingBox();
		//DrawBoundingBox(aabb, float3(1.0f, 0.0f, 0.0f));
		
	if (drawAABB)
		DrawAABB();

	//if (owner->active || owner->isParentSelected())
	//	DrawOutline();

		glUseProgram(0);
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
	if (this->mesh != nullptr) {
		RELEASE(this->mesh);
	}
	this->mesh = mesh;
	GenerateBounds();
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
		ImGui::Checkbox("Draw bounding box", &drawAABB);
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
	//obb = GetLocalAABB();

	//// Generate global AABB
	//aabb.SetNegativeInfinity();
	//aabb.Enclose(obb);
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

bool ComponentMesh::GetRenderMesh()
{
	return renderMesh;
}

void ComponentMesh::SetRenderMesh(bool renderMesh)
{
	this->renderMesh = renderMesh;
}

void ComponentMesh::GenerateBounds()
{
	mesh->localAABB.SetNegativeInfinity();
	//mesh->localAABB->Enclose(&mesh->vertices[0], vertices.size());
	mesh->localAABB.Enclose((float3*)mesh->vertices, mesh->verticesSizeBytes / (sizeof(float) * 3));


	Sphere sphere;
	sphere.r = 0.f;
	sphere.pos = mesh->localAABB.CenterPoint();
	sphere.Enclose(mesh->localAABB);

	radius = sphere.r;
	centerPoint = sphere.pos;
}

AABB ComponentMesh::GetGlobalAABB() const
{
	AABB global = AABB(mesh->localAABB);
	global.Translate(owner->GetComponent<ComponentTransform>()->GetPosition());
	return global;
}

void ComponentMesh::DrawAABB() const
{
	AABB aabb = GetGlobalAABB();
	float3 corners[8];
	aabb.GetCornerPoints(corners);

	for (int i = 0; i < 8; i++)
	{
		glColor3f(1, 0.8, 0);
		GLUquadric* quadric = gluNewQuadric();
		glTranslatef(corners[i].x, corners[i].y, corners[i].z);
		gluSphere(quadric, 0.05f, 8, 8);
		glTranslatef(-corners[i].x, -corners[i].y, -corners[i].z);
		gluDeleteQuadric(quadric);

		for (int j = 0; j < 8; j++)
		{
			if (j == i) continue;
			glBegin(GL_LINES);
			glDisable(GL_LIGHTING);
			glLineWidth(3.f);
			glVertex3f(corners[i].x, corners[i].y, corners[i].z);
			glVertex3f(corners[j].x, corners[j].y, corners[j].z);
			glEnd();
			glEnable(GL_LIGHTING);
		}
	}
}

void ComponentMesh::DrawOutline() const
{
	glEnable(GL_STENCIL);
	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glDisable(GL_LIGHTING);

	if (owner->active)
	{
		glColor3f(0.8f, 0.8f, 0.0f);
		glLineWidth(1.5f);
	}
	else if (owner->isParentSelected())
	{
		glColor3f(0.8f, 0.2f, 0.0f);
		glLineWidth(1.5f);
	}

	glStencilFunc(GL_NOTEQUAL, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glPolygonMode(GL_FRONT, GL_LINE);

	glPushMatrix();
	ComponentTransform* transform = owner->GetComponent<ComponentTransform>();
	glMultMatrixf(transform->transformMatrix.Transposed().ptr());

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_index);
	glVertexPointer(3, GL_FLOAT, 0, 0);

	glDrawElements(GL_TRIANGLES, *mesh->indices * 3, GL_UNSIGNED_INT, 0);

	glDisable(GL_STENCIL);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_LIGHTING);
	glDisableClientState(GL_VERTEX_ARRAY);
	glLineWidth(1);

	glPopMatrix();
}


