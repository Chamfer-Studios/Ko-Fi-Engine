#include "ComponentMaterial.h"
//#include "Mesh.h"
#include "stb_image.h"
#include "imgui.h"
#include "ImGuiAppLog.h"
#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "PanelChooser.h"
#include "Shader.h"
#include "JsonHandler.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent)
{
	type = ComponentType::MATERIAL;
	LoadDefaultMaterial();
	LoadShader();
}

ComponentMaterial::~ComponentMaterial()
{
	glDeleteProgram(this->materialShader);

}

//void ComponentMaterial::LoadTextureFromId(uint& textureID, const char* path)
//{
//	this->path = path;
//	int width, height, nrChannels;
//	unsigned char* pixels = stbi_load(path, &width, &height, &nrChannels, STBI_rgb);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//	glBindTexture(GL_TEXTURE_2D, textureID);
//	// set the texture wrapping/filtering options (on the currently bound texture object)
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	if (pixels)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//		glGenerateMipmap(GL_TEXTURE_2D);
//	}
//	else
//	{
//		appLog->AddLog("%s", "Texture Image not loaded correctly");
//	}
//
//	stbi_image_free(pixels);
//}

void ComponentMaterial::LoadDefaultMaterial()
{

}



void ComponentMaterial::LoadMaterial(const char* path)
{

}

void ComponentMaterial::Compile()
{
	LoadShader(this->shaderPath.c_str());
}

void ComponentMaterial::LoadTexture(std::string path)
{
	this->material.texture.SetUpTexture(path);
}



bool ComponentMaterial::InspectorDraw(PanelChooser* panelChooser)
{
	if (ImGui::CollapsingHeader("Material")) {

		for (Uniform* uniform : material.uniforms) {
			switch (uniform->type) {
			case GL_FLOAT:
			{
				UniformT<float>* uf = (UniformT<float>*)uniform;
				ImGui::SliderFloat(uniform->name.c_str(), &uf->value, 0, 1);
			}
			break;
			case GL_FLOAT_VEC4:
			{

				UniformT<float4>* uf4 = (UniformT<float4>*)uniform;
				ImGui::SliderFloat4(uniform->name.c_str(), uf4->value.ptr(), 0, 1);
			}
			break;
			case GL_INT:

			{
				UniformT<int>* ui = (UniformT<int>*)uniform;
				ImGui::SliderInt(uniform->name.c_str(), &ui->value, 0, 1);
			}
			break;
			}
		}

	}
	return true;
}

Texture ComponentMaterial::GetTexture()
{
	return material.texture;
}

Material ComponentMaterial::GetMaterial()
{
	return this->material;
}

uint ComponentMaterial::GetShader()
{
	return materialShader;
}

void ComponentMaterial::LoadUniforms()
{
	this->material.uniforms.clear();
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length
	glGetProgramiv(this->materialShader, GL_ACTIVE_UNIFORMS, &count);

	for (i = 0; i < count; i++) {

		glGetActiveUniform(this->materialShader, (GLuint)i, bufSize, &length, &size, &type, name);
		switch (type) {
		case GL_FLOAT:
		{
			UniformT<float>* uf = new UniformT<float>(name,type,0.0f);
			this->material.AddUniform(uf);
		}
		break;
		case GL_FLOAT_VEC4:
		{

			UniformT<float4>* uf4 = new UniformT<float4>(name,type, float4(1.0f, 1.0f, 1.0f, 1.0f));
			this->material.AddUniform(uf4);
		}
		break;
		case GL_INT:

		{
			UniformT<int>* ui = new UniformT<int>(name, type, 0);
			this->material.AddUniform(ui);
		}
		break;
		}
	}
}
void ComponentMaterial::LoadShader(const char* shaderPath)
{
	if (this->materialShader != 0) {
		glDeleteProgram(this->materialShader);
	}
	shaderPath ? this->shaderPath = shaderPath : this->shaderPath = "Library/Shaders/default_shader.glsl";
	shader::ShaderProgramSource shaderSource = shader::ParseShader(this->shaderPath);
	materialShader = shader::CreateShader(shaderSource.VertexSource, shaderSource.FragmentSource);

	LoadUniforms();
}



