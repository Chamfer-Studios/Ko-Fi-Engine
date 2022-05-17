#include "C_FOW.h"
#include "M_UI.h"
#include "GameObject.h"
#include "C_Material.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "R_Texture.h"

C_FOW::C_FOW(GameObject* parent) : Component(parent)
{
	drawablePlane = new MyPlane(owner);

	size = float2(128, 128);

	GenerateTexture();
}

C_FOW::~C_FOW()
{
}

void C_FOW::Save(Json& json) const
{
}

void C_FOW::Load(Json& json)
{
}

bool C_FOW::Update(float dt)
{
	if (isDirty) {
		UpdateTexture();
		isDirty = false;
	}

	return true;
}

bool C_FOW::CleanUp()
{
	CleanTexture();

	return true;
}

void C_FOW::CleanTexture()
{
	if (fowMap != nullptr)
	{
		for (int i = 0; i < size.y; i++) {
			for (int j = 0; j < size.x; j++) {
				free(fowMap[i][j]);
			}
			
			free(fowMap[i]);
		}

		free(fowMap);
	}
	
	if (textureID != 0) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
}

void C_FOW::GenerateTexture()
{
	CleanTexture();

	fowMap = (unsigned char***)malloc(size.y * sizeof(unsigned char) * 4 * size.x);

	for (int i = 0; i < size.y; i++) {
		fowMap[i] = (unsigned char**)malloc(sizeof(unsigned char) * 4 * size.x);

		for (int j = 0; j < size.x; j++) {
			fowMap[i][j] = (unsigned char*)malloc(sizeof(unsigned char) * 4);
			memset(fowMap[i][j], 0, 3);
			fowMap[i][j][3] = 255;
		}
	}

	UpdateTexture();
}

void C_FOW::UpdateTexture()
{
	unsigned int textureSizeBytes = size.x * size.y * 4 * sizeof(unsigned char);

	unsigned char* data = (unsigned char*)malloc(textureSizeBytes);

	int a = 0;
	for (int i = 0; i < size.y; i++) {
		for (int j = 0; j < size.x; j++) {
			data[a] = fowMap[i][j][0];
			data[a + 1] = fowMap[i][j][1];
			data[a + 2] = fowMap[i][j][2];
			data[a + 3] = fowMap[i][j][3];
			a += 4;
		}
	}
	
	R_Texture* tex = new R_Texture();

	tex->SetTextureWidth(size.x);
	tex->SetTextureHeight(size.y);
	tex->SetNrChannels(4);
	tex->data = data;
	
	tex->SetUpTexture();
	
	C_Material* mat = owner->GetComponent<C_Material>();
	if (mat != nullptr) {
		mat->texture = tex;
	}

	free(data);
}

float2 C_FOW::GlobalToPixel(float3 global)
{
	C_Mesh* mesh = owner->GetComponent<C_Mesh>();

	if (mesh != nullptr) {
		AABB aabb = owner->GetComponent<C_Mesh>()->GetGlobalAABB();
		
		float xProp = 1 - (aabb.MaxX() - global.x) / (aabb.MaxX() - aabb.MinX());
		float zProp = 1 - (aabb.MaxZ() - global.z) / (aabb.MaxZ() - aabb.MinZ());

		float2 pixel = float2((int)((xProp * size.x)), (int)((zProp * size.y)));

		return pixel;
	}

	return float2(0, 0);
}

float C_FOW::GlobalRadiusToPixel(float radius)
{
	C_Mesh* mesh = owner->GetComponent<C_Mesh>();

	if (mesh != nullptr) {
		AABB aabb = owner->GetComponent<C_Mesh>()->GetGlobalAABB();

		float prop = std::max(size.x, size.y) / std::max(aabb.MaxX() - aabb.MinX(), aabb.MaxZ() - aabb.MinZ());
		return radius * prop;
	}

	return 0.0f;
}



void C_FOW::WriteCircle(float3 position, float radius, int value)
{
	float2 localPos = GlobalToPixel(position);
	float pixelRadius = GlobalRadiusToPixel(radius);

	float radiusIntervals = std::max(size.x, size.y) / (radius * 2);
	float angleIntervals = M_PI * 2 / std::max(size.x, size.y);

	for (int i = 0; i < size.y; i++) {
		for (int j = 0; j < size.x; j++) {
			double dx = j - localPos.x;
			double dy = i - localPos.y;

			double distanceSquared = dx * dx + dy * dy;
			if (distanceSquared < pixelRadius * pixelRadius) {
				fowMap[i][j][3] = value;
			}
		}
	}

	isDirty = true;
}

bool C_FOW::InspectorDraw(PanelChooser* chooser)
{
	if (ImGui::CollapsingHeader("FOW##", ImGuiTreeNodeFlags_AllowItemOverlap)) {
		ImGui::DragFloat2("size", size.ptr());
		if (ImGui::Button("Generate Texture", ImVec2(100, 40))) {
			GenerateTexture();
		}
	}

	return true;
}
