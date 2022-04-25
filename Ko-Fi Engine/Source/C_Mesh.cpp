#include "C_Mesh.h"
#include "Globals.h"
#include "Engine.h"
#include "FSDefs.h"
//#include "Color.h"
#include "M_Camera3D.h"

#include "EmitterInstance.h"
#include "Log.h"
// Modules
#include "Engine.h"
#include "M_SceneManager.h"
#include "M_Renderer3D.h"
#include "M_Camera3D.h"
#include "M_Editor.h"

// GameObject
#include "GameObject.h"
#include "C_Transform.h"
#include "C_Material.h"
#include "C_Camera.h"

// Panels
#include "PanelChooser.h"
#include "PanelViewport.h"

// Resources
#include "Importer.h"
#include "R_Material.h"

#include "glew.h"
#include <gl/GL.h>

#include <MathGeoLib/Math/float2.h>
#include <MathGeoLib/Math/float3.h>
#include <MathGeoLib/Math/float4.h>


C_Mesh::C_Mesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
	radius = 0.0f;
	mesh = nullptr;
	renderMesh = true;
	time = 0.0f;

}

C_Mesh::~C_Mesh()
{
	CleanUp(); // Already called
}

bool C_Mesh::Start()
{
	if (mesh)
		GenerateGlobalBoundingBox();
	return true;
}

bool C_Mesh::Update(float dt)
{
	return true;
}

bool C_Mesh::PostUpdate(float dt) //AKA the real render
{
	bool ret = true;

	return ret;
}

bool C_Mesh::CleanUp()
{
	std::string temp(owner->GetName());
	if (temp.find("Knife") != std::string::npos || temp.find("Decoy") != std::string::npos || temp.find("Mosquito") != std::string::npos)  // Dirty Fix before resource manager works
		return true;
	RELEASE(mesh);

	return true;
}

void C_Mesh::Save(Json& json) const
{
	json["type"] = "mesh";

	std::string name = owner->GetName();
	mesh->path = MESHES_DIR + name + MESH_EXTENSION;
	Importer::GetInstance()->meshImporter->Save(mesh, mesh->path.c_str());

	json["path"] = mesh->path;
	json["shape_type"] = (int)mesh->meshType;
	json["draw_vertex_normals"] = mesh->GetVertexNormals();
	json["draw_face_normals"] = mesh->GetFaceNormals();
	json["isAnimated"] = mesh->IsAnimated();
	if (mesh->IsAnimated())
	{
		json["rootNodeUID"] = mesh->GetRootNode()->GetUID();
	}
}

void C_Mesh::Load(Json& json)
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
		SetMesh(new R_Mesh(meshType));

		mesh->meshType = meshType;
	}

	std::string path = json.at("path");
	if (json.contains("isAnimated"))
		mesh->SetIsAnimated(json.at("isAnimated"));
	else
		mesh->SetIsAnimated(false);
	Importer::GetInstance()->meshImporter->Load(path.c_str(), mesh); // TODO: CHECK IF MESH DATA IS USED
	mesh->path = path;

	SetVertexNormals(json.at("draw_vertex_normals"));
	SetFaceNormals(json.at("draw_face_normals"));
	if (mesh->IsAnimated())
	{
		uint uid = (uint)json.at("rootNodeUID");

		GameObject* object = owner->GetEngine()->GetSceneManager()->GetCurrentScene()->GetGameObject(uid);
		this->GetMesh()->SetRootNode(object);
	}
	GenerateGlobalBoundingBox();
}

void C_Mesh::SetMesh(R_Mesh* mesh)
{
	if (this->mesh != nullptr)
		RELEASE(this->mesh);

	this->mesh = mesh;
	GenerateLocalBoundingBox();
}

void C_Mesh::SetVertexNormals(bool vertexNormals)
{
	this->mesh->SetVertexNormals(vertexNormals);
}

void C_Mesh::SetFaceNormals(bool facesNormals)
{
	this->mesh->SetFaceNormals(facesNormals);
}

float3 C_Mesh::GetCenterPointInWorldCoords() const
{
	return owner->GetTransform()->GetGlobalTransform().TransformPos(GetCenterPoint());
}

uint C_Mesh::GetVertices()
{
	uint numVertices = 0;
	numVertices += mesh->verticesSizeBytes / (sizeof(float) * 3);
	return numVertices;
}

bool C_Mesh::GetVertexNormals() const
{
	return mesh->GetVertexNormals();
}

bool C_Mesh::GetFaceNormals() const
{
	return mesh->GetFaceNormals();
}

const AABB C_Mesh::GetLocalAABB()
{
	GenerateLocalBoundingBox();
	return mesh->localAABB;
}

const AABB C_Mesh::GetGlobalAABB() const
{
	return aabb;
}

void C_Mesh::GenerateLocalBoundingBox()
{
	// Generate AABB
	if (mesh != nullptr && mesh->vertices != 0) // to avoid float3 isfinite warning.
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

void C_Mesh::GenerateGlobalBoundingBox()
{
	// Generate global OBB
	obb.SetFrom(GetLocalAABB());
	obb.Transform(owner->GetTransform()->GetGlobalTransform());

	// Generate global AABB
	aabb.SetNegativeInfinity();
	aabb.Enclose(obb);
}

void C_Mesh::DrawBoundingBox(const AABB& aabb, const float3& rgb)
{
	glLineWidth(2.0f);
	glColor3f(rgb.x, rgb.y, rgb.z);
	glPushMatrix();
	glMultMatrixf(this->owner->GetTransform()->GetGlobalTransform().Transposed().ptr());
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
	glPopMatrix();

	glPopMatrix();

}

bool C_Mesh::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (mesh != nullptr && ImGui::CollapsingHeader("R_Mesh", ImGuiTreeNodeFlags_AllowItemOverlap))
	{
		DrawDeleteButton(owner, this);

		if (mesh->path.empty())
			ImGui::BeginDisabled();
		ImGui::Text("Mesh Path: ");
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
		if (ImGui::Selectable(mesh->path.c_str())) {}
		ImGui::PopStyleColor();
		if (mesh->path.empty())
			ImGui::EndDisabled();

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
	else
		DrawDeleteButton(owner, this);

	return ret;
}
