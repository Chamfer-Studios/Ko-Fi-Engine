#include "ComponentMesh.h"
#include "Globals.h"
#include "Engine.h"
#include "FSDefs.h"
//#include "Color.h"
#include "Camera3D.h"

#include "GameObject.h"
#include "ComponentTransform.h"
#include "C_Material.h"
#include "C_Camera.h"
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

#include "Material.h"

#include "glew.h"
#include <gl/GL.h>

#include <MathGeoLib/Math/float2.h>
#include <MathGeoLib/Math/float3.h>
#include <MathGeoLib/Math/float4.h>


ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = ComponentType::MESH;
	radius = 0.0f;
	mesh = nullptr;
	renderMesh = true;
	time = 0.0f;

}

ComponentMesh::~ComponentMesh()
{
	RELEASE(mesh);
}

bool ComponentMesh::Start()
{
	GenerateLocalBoundingBox();
	return true;
}

bool ComponentMesh::Update(float dt)
{
	return true;
}

bool ComponentMesh::PostUpdate(float dt) //AKA the real render
{
	bool ret = true;


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
		SetMesh(new Mesh(meshType));

		mesh->meshType = meshType;
	}

	std::string path = json.at("path");
	if (json.contains("isAnimated"))
	{
		mesh->SetIsAnimated(json.at("isAnimated"));

	}
	else
	{
		mesh->SetIsAnimated(false);
	}
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

}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	if (this->mesh != nullptr)
		RELEASE(this->mesh);

	this->mesh = mesh;
	GenerateLocalBoundingBox();
}

void ComponentMesh::SetVertexNormals(bool vertexNormals)
{
	this->mesh->SetVertexNormals(vertexNormals);
}

void ComponentMesh::SetFaceNormals(bool facesNormals)
{
	this->mesh->SetFaceNormals(facesNormals);
}

float3 ComponentMesh::GetCenterPointInWorldCoords() const
{
	return owner->GetTransform()->GetGlobalTransform().TransformPos(GetCenterPoint());
}

uint ComponentMesh::GetVertices()
{
	uint numVertices = 0;
	numVertices += mesh->verticesSizeBytes / (sizeof(float) * 3);
	return numVertices;
}

bool ComponentMesh::GetVertexNormals() const
{
	return mesh->GetVertexNormals();
}

bool ComponentMesh::GetFaceNormals() const
{
	return mesh->GetFaceNormals();
}

const AABB ComponentMesh::GetLocalAABB()
{
	GenerateLocalBoundingBox();
	return mesh->localAABB;
}

const AABB ComponentMesh::GetGlobalAABB() const
{
	return aabb;
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

void ComponentMesh::GenerateGlobalBoundingBox()
{
	// Generate global OBB

	obb.SetFrom(GetLocalAABB());
	obb.Transform(owner->GetTransform()->GetGlobalTransform());

	// Generate global AABB
	aabb.SetNegativeInfinity();
	aabb.Enclose(obb);
}

void ComponentMesh::DrawBoundingBox(const AABB& aabb, const float3& rgb)
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

bool ComponentMesh::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (mesh != nullptr && ImGui::CollapsingHeader("Mesh"))
	{
		DrawDeleteButton(owner, this);

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
	else
		DrawDeleteButton(owner, this);

	return ret;
}
