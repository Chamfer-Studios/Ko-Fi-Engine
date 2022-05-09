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
#include "M_ResourceManager.h"

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
	time = 0.0f;
}

C_Mesh::~C_Mesh()
{
	CleanUp(); // Already called
}

bool C_Mesh::Start()
{
	if (mesh != nullptr)
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

	if (mesh != nullptr)
		owner->GetEngine()->GetResourceManager()->FreeResource(mesh->GetUID());
	mesh = nullptr;

	return true;
}

void C_Mesh::Save(Json& json) const
{
	json["type"] = (int)type;

	if (mesh != nullptr)
	{
		json["mesh"]["uid"] = mesh->GetUID();
		json["mesh"]["asset_path"] = mesh->GetAssetPath();
		json["mesh"]["shape_type"] = (int)mesh->meshType;
		json["mesh"]["draw_vertex_normals"] = mesh->GetVertexNormals();
		json["mesh"]["draw_face_normals"] = mesh->GetFaceNormals();
		json["mesh"]["is_animated"] = mesh->IsAnimated();

		if (mesh->IsAnimated())
			json["mesh"]["root_node_UID"] = mesh->GetRootNode()->GetUID();
	}
}

void C_Mesh::Load(Json& json)
{
	if (!json.empty())
	{
		CleanUp();

		Json jsonMesh = json.at("mesh");
		UID uid = jsonMesh.at("uid");
		owner->GetEngine()->GetResourceManager()->LoadResource(uid, jsonMesh.at("asset_path").get<std::string>().c_str());
		mesh = (R_Mesh*)owner->GetEngine()->GetResourceManager()->RequestResource(uid);

		if (mesh == nullptr)
			CONSOLE_LOG("[ERROR] Component Mesh: could not load resource from library.");
		else
		{
			GenerateLocalBoundingBox();
			GenerateGlobalBoundingBox();
			mesh->meshType = (Shape)jsonMesh.at("shape_type").get<int>();
			mesh->SetVertexNormals(jsonMesh.at("draw_vertex_normals"));
			mesh->SetFaceNormals(jsonMesh.at("draw_face_normals"));
			mesh->SetIsAnimated(jsonMesh.at("is_animated"));

			if (mesh->IsAnimated())
				mesh->SetRootNode(owner->GetEngine()->GetSceneManager()->GetCurrentScene()->GetGameObject(jsonMesh.at("root_node_UID")));
		}
	}
}

void C_Mesh::SetMesh(R_Mesh* mesh)
{
	if (this->mesh != nullptr)
		owner->GetEngine()->GetResourceManager()->FreeResource(this->mesh->GetUID());

	this->mesh = nullptr;

	this->mesh = mesh;
	GenerateLocalBoundingBox();
}

void C_Mesh::SetPath(const char* path)
{
	if (mesh != nullptr)
		mesh->SetAssetPath(path);
	else
		CONSOLE_LOG("[ERROR] Mesh: Could not set path, mesh was nullptr.");
}

void C_Mesh::SetVertexNormals(bool vertexNormals)
{
	if (this->mesh != nullptr)
		this->mesh->SetVertexNormals(vertexNormals);
}

void C_Mesh::SetFaceNormals(bool facesNormals)
{
	if (this->mesh != nullptr)
		this->mesh->SetFaceNormals(facesNormals);
}

const char* C_Mesh::GetMeshPath() const
{
	if (mesh != nullptr)
		return mesh->GetAssetPath();
	else
		CONSOLE_LOG("[ERROR] Mesh: Could not get assets path, mesh was nullptr.");
	return nullptr;
}

float3 C_Mesh::GetCenterPoint() const
{
	if (mesh != nullptr)
		return mesh->localAABB.CenterPoint();
	else
		CONSOLE_LOG("[ERROR] Mesh: Could not get center point, mesh was nullptr.");
	return float3::zero;
}

float3 C_Mesh::GetCenterPointInWorldCoords() const
{
	if (this->mesh != nullptr)
		return owner->GetTransform()->GetGlobalTransform().TransformPos(GetCenterPoint());
}

uint C_Mesh::GetVertices()
{
	uint numVertices = 0;
	if (this->mesh != nullptr)
		numVertices += mesh->verticesSizeBytes / (sizeof(float) * 3);
	return numVertices;
}

bool C_Mesh::GetVertexNormals() const
{
	if (this->mesh != nullptr)
		return mesh->GetVertexNormals();
}

bool C_Mesh::GetFaceNormals() const
{
	if (this->mesh != nullptr)
		return mesh->GetFaceNormals();
}

const AABB C_Mesh::GetLocalAABB()
{
	if (this->mesh != nullptr)
	{
		GenerateLocalBoundingBox();
		return mesh->localAABB;
	}
	else
		CONSOLE_LOG("[ERROR] Mesh: Could not get local AABB, mesh was nullptr.");

	return AABB();
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
	if (drawAABB)
	{
		glPushMatrix();
		glMultMatrixf(this->owner->GetTransform()->GetGlobalTransform().Transposed().ptr());

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
		glPopMatrix();

	}
}

bool C_Mesh::InspectorDraw(PanelChooser* chooser)
{
	bool ret = true;
	if (mesh != nullptr && ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_DefaultOpen))
	{
		/*if (DrawDeleteButton(owner, this))
			return true;*/

		if (mesh->GetLibraryPath() != nullptr)
		{
			ImGui::Text("Mesh Path: ");
			ImGui::SameLine();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			if (ImGui::Selectable(mesh->GetLibraryPath())) {}
			ImGui::PopStyleColor();
		}

		ImGui::Text("Num. vertices: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", GetVertices());
		if (owner->GetRenderGameObject())
		{
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Visible");
		}
		else
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not Visible");

		}
		bool vertex = GetVertexNormals();
		if (ImGui::Checkbox("Vertex Normals", &vertex))
			mesh->SetVertexNormals(vertex);
		bool faces = GetFaceNormals();
		if (ImGui::Checkbox("Faces Normals", &faces))
			mesh->SetFaceNormals(faces);
		bool drawAABB = SetDrawAABB();
		if (ImGui::Checkbox("Draw AABB##", &drawAABB))
			GetDrawAABB(drawAABB);
	}
	//else
	//	DrawDeleteButton(owner, this);

	return ret;
}