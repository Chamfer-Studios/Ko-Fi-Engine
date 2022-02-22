#include "ComponentImage.h"

#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "Camera3D.h"
#include "Engine.h"

#include "PanelChooser.h"

#include "par_shapes.h"

#include "MathGeoLib/Math/Quat.h"

#include "glew.h"
#include <vector>

ComponentImage::ComponentImage(GameObject* parent) : Component(parent)
{
	type = ComponentType::IMAGE;
	plane = new ComponentMesh(nullptr);
	AddPlaneToMesh(plane);
	plane->GenerateLocalBoundingBox();
	plane->GenerateGlobalBoundingBox();

	if (plane->GetMesh()->texCoordSizeBytes != 0)
	{
		textureBufferId = 0;
		glGenBuffers(1, &textureBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, textureBufferId);
		glBufferData(GL_ARRAY_BUFFER, plane->GetMesh()->texCoordSizeBytes, &plane->GetMesh()->tex_coords[0], GL_STATIC_DRAW);
	}
}

ComponentImage::~ComponentImage()
{

}

bool ComponentImage::Update()
{
	return true;
}

bool ComponentImage::PostUpdate(float dt)
{
	/*glBindTexture(GL_TEXTURE_2D, 0); // Bindear Textura a Default
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, plane->GetMesh()->id_vertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->GetMesh()->id_index);

	ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);*/

	ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	/*float3 pos = { cTransform->position, App->camera->nearPlaneDistance + 0.1f };*/
	/*float3 size = {  (float)viewport[2], (float)viewport[3], 1.0f };*/
	float2 realPosition;
	cTransform->GetRealPosition(realPosition);
	float3 pos = { realPosition.x, realPosition.y, this->owner->GetEngine()->GetCamera3D()->nearPlaneDistance + 0.1f };
	float2 realSize;
	cTransform->GetRealSize(realSize);
	float3 size = { realSize.x, realSize.y, 1.0f };
	float3 rotation = { cTransform->rotation.x,cTransform->rotation.y,cTransform->rotation.z };
	Quat rotationQuat = Quat::FromEulerXYZ(DEGTORAD * rotation.x, DEGTORAD * rotation.y, DEGTORAD * rotation.z);

	transform3D = transform3D.FromTRS(pos, Quat::identity, size);

	transform3D = transform3D * rotationQuat;

	if (this->textureBufferId)
	{
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, this->textureBufferId);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glColor4f(imageColor.x, imageColor.y, imageColor.z, imageColor.w);
	}

	if (texture.GetTextureId() != 0)
	{
		glBindTexture(GL_TEXTURE_2D, texture.GetTextureId());
	}
	else // Called once when created an object. Set white fallback as a default texture
	{
		//glBindTexture(GL_TEXTURE_2D, owner->GetEngine()->);
	}

	glPushMatrix();
	glMultMatrixf(transform3D.Transposed().ptr());
	plane->GetMesh()->Draw(owner);
	glPopMatrix();

	//-- Buffers--//

	//-- Textures --//
	if (this->textureBufferId)
	{
		glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}
	//App->textures->Load(texture.name);
	glBindTexture(GL_TEXTURE_2D, 0);
	//--Disables States--//
	glDisableClientState(GL_VERTEX_ARRAY);
	return true;
}


/*void ComponentImage::SetTextureById(const int id)
{
	for (auto i = App->textures->textures.begin(); i != App->textures->textures.end(); ++i)
	{
		if (i->second.id == id)
		{
			texture = App->textures->Get(i->second.name);
		}
	}
}

void ComponentImage::SetOpacity(float alpha)
{
	if (imageColor.w == 0.f)
	{
		App->ui->fadeOut = false;
		return;
	}
	imageColor.w -= alpha;
}*/

bool ComponentImage::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Image")) {
		// Texture display
		ImGui::Text("Texture: ");
		ImGui::SameLine();
		if (texture.GetTextureId() == 0) // Supposedly there is no textureId = 0 in textures array
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(texture.GetTexturePath());
			ImGui::Image((ImTextureID)texture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("AddTextureImage")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				texture.SetUpTexture(path);
			}
		}

		if (ImGui::Button("Set Texture")) {
			panelChooser->OpenPanel("AddTextureImage", "png");
		}

		// Image color button
		ImGui::Separator();
		ImGui::ColorEdit4("##Image color", (float*)&imageColor, ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoBorder | ImGuiColorEditFlags_AlphaPreviewHalf);
		ImGui::SameLine();
		ImGui::Text("Image color");
	}

	return true;
}

/*float4x4 ComponentImage::GetTransform()
{
	float4x4 ret = transform.ScaleAlongAxis(float3::unitX, 1);
	ret = ret.ScaleAlongAxis(float3::unitY, 1);
	return transform;
}*/

void ComponentImage::AddPlaneToMesh(ComponentMesh* mesh)
{
	par_shapes_mesh* plane = par_shapes_create_plane(1, 1);
	par_shapes_translate(plane, -0.5f, -0.5f, 0);
	mesh->CopyParMesh(plane);
}

/*void ComponentImage::OnLoad(const JSONReader& reader)
{
	// Loading texture name
	if (reader.HasMember("Texture name"))
	{
		const rapidjson::Value& itemTextureName = reader["Texture name"];
		texture.name = itemTextureName.GetString();
		texture = App->textures->Load(texture.name);
	}

	// Loading texture size
	if (reader.HasMember("Size"))
	{
		const rapidjson::Value& itemSize = reader["Size"];
		int width_ = 0;
		int height_ = 0;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemSize.Begin(); it != itemSize.End(); ++it)
		{
			if (i == 0) width_ = it->GetInt();
			else if (i == 1) height_ = it->GetInt();
			i++;
		}
		texture.width = width_;
		texture.height = height_;
	}

	// Loading texture Id
	if (reader.HasMember("Texture ID"))
	{
		const rapidjson::Value& itemTextureId = reader["Texture ID"];
		//texture.id = itemTextureId.GetInt();
	}

	// Loading image color
	if (reader.HasMember("Image color"))
	{
		const rapidjson::Value& itemSize = reader["Image color"];
		float x_ = 0.000f, y_ = 0.000f, z_ = 0.000f, w_ = 0.000f;
		int i = 0;
		for (rapidjson::Value::ConstValueIterator it = itemSize.Begin(); it != itemSize.End(); ++it)
		{
			if (i == 0) x_ = it->GetDouble();
			else if (i == 1) y_ = it->GetDouble();
			else if (i == 2) z_ = it->GetDouble();
			else if (i == 3) w_ = it->GetDouble();
			i++;
		}
		imageColor = { x_, y_, z_, w_ };
	}
}

void ComponentImage::OnSave(JSONWriter& writer) const
{
	writer.String("Image");
	writer.StartObject();

	// Saving texture name
	writer.String("Texture name");
	writer.String(texture.name.c_str());

	// Saving texture size
	writer.String("Size");
	writer.StartArray();
	writer.Int(texture.width);
	writer.Int(texture.height);
	writer.EndArray();

	// Saving texture id
	writer.String("Texture ID");
	writer.Int(texture.id);

	// Saving image color
	writer.String("Image color");
	writer.StartArray();
	writer.Double(imageColor.x);
	writer.Double(imageColor.y);
	writer.Double(imageColor.z);
	writer.Double(imageColor.w);
	writer.EndArray();

	writer.EndObject();
}*/
