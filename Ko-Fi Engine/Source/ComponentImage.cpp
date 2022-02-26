#include "ComponentImage.h"

#include "ComponentMesh.h"
#include "GameObject.h"
#include "ComponentTransform2D.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "Camera3D.h"
#include "Engine.h"
#include "Editor.h"
#include "Window.h"

#include "PanelChooser.h"
#include "SceneIntro.h"

#include "par_shapes.h"
#include "Log.h"
#include "UI.h"

#include "MathGeoLib/Math/Quat.h"
#include "ImGuiAppLog.h"
#include "SDL_image.h"

#include "SDL.h"

#include "glew.h"
#include <vector>

ComponentImage::ComponentImage(GameObject* parent) : Component(parent)
{
	type = ComponentType::IMAGE;
	plane = new TMPPlane(0);
	plane->GenerateBuffers();
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

	//ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	/*float3 pos = { cTransform->position, App->camera->nearPlaneDistance + 0.1f };*/
	/*float3 size = {  (float)viewport[2], (float)viewport[3], 1.0f };*/
	/*float2 realPosition;
	cTransform->GetRealPosition(realPosition);
	float3 pos = { realPosition.x, realPosition.y, this->owner->GetEngine()->GetCamera3D()->nearPlaneDistance + 0.1f };
	float2 realSize;
	cTransform->GetRealSize(realSize);
	float3 size = { realSize.x, realSize.y, 1.0f };
	float3 rotation = { cTransform->rotation.x,cTransform->rotation.y,cTransform->rotation.z };
	Quat rotationQuat = Quat::FromEulerXYZ(DEGTORAD * rotation.x, DEGTORAD * rotation.y, DEGTORAD * rotation.z);

	transform3D = transform3D.FromTRS(pos, Quat::identity, size);

	transform3D = transform3D * rotationQuat;

	uint shader =
		owner->GetComponent<ComponentMaterial>()->GetShader();
	glUseProgram(shader);
	//Matrices
	GLint model_matrix = glGetUniformLocation(shader, "model_matrix");
	glUniformMatrix4fv(model_matrix, 1, GL_FALSE, transform3D.Transposed().ptr());

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
			if (uniform->name != "time")
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

	//Texture
	if (ComponentMaterial* material = owner->GetComponent<ComponentMaterial>()) {
		for (Texture& tex : material->GetMaterial().textures) {
			glBindTexture(GL_TEXTURE_2D, tex.GetTextureId());

		}
	}

	glBindVertexArray(plane->VAO);
	glDrawElements(GL_TRIANGLES, plane->indexNum, GL_UNSIGNED_INT, 0);

	//Unbind Texture
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
	*/

	ComponentTransform2D* cTransform = this->owner->GetComponent<ComponentTransform2D>();

	SDL_Rect rect;

	rect.x = 30;
	rect.y = 30;

	if (SDLTexture != nullptr) {
		SDL_QueryTexture(SDLTexture, NULL, NULL, &rect.w, &rect.h);
	}
	else {
		rect.w = 0;
		rect.h = 0;
	}

	GLuint fboId = 0;
	glGenFramebuffers(1, &fboId);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLTexture.GetTextureId(), 0);
	float2 normalizedPosition = cTransform->GetNormalizedPosition();
	float2 pivot = cTransform->pivot;

	float2 normalizedSize = cTransform->GetNormalizedSize();

	float2 lowerLeft = { normalizedPosition.x - pivot.x * normalizedSize.x, normalizedPosition.y - pivot.y * normalizedSize.y };
	float canvasRatio = (owner->GetEngine()->GetEditor()->lastViewportSize.x) / (owner->GetEngine()->GetEditor()->lastViewportSize.y);
	float2 upperRight = { lowerLeft.x + normalizedSize.x, lowerLeft.y + normalizedSize.y };
	glBlitFramebuffer(0, 0, rect.w, rect.h, lowerLeft.x, lowerLeft.y, upperRight.x, upperRight.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

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
		if (openGLTexture.GetTextureId() == 0) // Supposedly there is no textureId = 0 in textures array
		{
			ImGui::Text("None");
		}
		else
		{
			ImGui::Text(openGLTexture.GetTexturePath());
			ImGui::Image((ImTextureID)openGLTexture.GetTextureId(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
		}

		if (panelChooser->IsReadyToClose("AddTextureImage")) {
			if (panelChooser->OnChooserClosed() != nullptr) {
				std::string path = panelChooser->OnChooserClosed();
				//LoadTextureFromId(texture.textureID, path.c_str());
				openGLTexture.SetUpTexture(path);
				SDLTexture = LoadTexture(path.c_str());
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

SDL_Texture* ComponentImage::LoadTexture(const char* path)
{
	SDL_Texture* texture = NULL;
	SDL_Surface* surface = IMG_Load(path);

	if (surface == NULL)
	{
		appLog->AddLog("Could not load surface with path: %s. IMG_Load: %s", path, IMG_GetError());
	}
	else
	{
		texture = LoadSurface(surface);
		SDL_FreeSurface(surface);
	}

	return texture;
}

SDL_Texture* const ComponentImage::LoadSurface(SDL_Surface* surface)
{
	SDL_Texture* texture = SDL_CreateTextureFromSurface(owner->GetEngine()->GetUI()->renderer, surface);

	if (texture == NULL)
	{
		appLog->AddLog("Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
	}
	else
	{
		owner->GetEngine()->GetUI()->loadedTextures.push_back(texture);
	}

	return texture;
}

/*float4x4 ComponentImage::GetTransform()
{
	float4x4 ret = transform.ScaleAlongAxis(float3::unitX, 1);
	ret = ret.ScaleAlongAxis(float3::unitY, 1);
	return transform;
}*/

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
