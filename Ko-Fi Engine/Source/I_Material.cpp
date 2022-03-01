#include "I_Material.h"
#include "Material.h"
#include "Log.h"
#include "Assimp.h"
#include "FSDefs.h"

#include <glew.h>
#include <fstream>
#include <sstream>
#include <MathGeoLib/Math/float2.h>
#include <MathGeoLib/Math/float3.h>
#include <MathGeoLib/Math/float4.h>

I_Material::I_Material()
{
}

I_Material::~I_Material()
{
}

bool I_Material::Import(const aiMaterial* aiMaterial, Material* material)
{
	bool ret = false;

	if (material == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Material! Error: R_Material* was nullptr.");
		return false;
	}
	if (aiMaterial == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Material! Error: aiMaterial* was nullptr.");
		return false;
	}

	// Dirty setting of the assets path.
	//std::string dirPath = ASSETS_TEXTURES_PATH + App->fileSystem->GetLastDirectory(material->GetAssetsPath());

	//std::string file = "";
	//std::string fullPath = "";

	//aiColor4D color;
	//if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)															// Could also get specular and ambient occlusion colours.
	//{
	//	material->diffuseColor = Color(color.r, color.g, color.b, color.a);
	//}

	//aiString texPath;
	//if (aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)												// At the moment only DIFFUSE textures will be imported.
	//{
		//file = App->fileSystem->GetFileAndExtension(texPath.C_Str());
		//fullPath = dirPath + file;
		//material->materials.push_back(MaterialData(TextureType::DIFFUSE, 0, fullPath));
	//}

	std::ifstream stream(SHADERS_DIR + std::string("default_shader") + SHADER_EXTENSION);
	std::string line;
	std::stringstream ss[2];

	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	std::string vertexSource = ss[0].str();
	std::string fragmentSource = ss[1].str();

	if (vertexSource != "\n" && fragmentSource != "\n")
	{
		// First we create the shader program
		material->shaderProgramID = glCreateProgram();

		// We create the vertex shader and the fragment shader
		unsigned int vShader = ImportShader(GL_VERTEX_SHADER, vertexSource);
		unsigned int fShader = ImportShader(GL_FRAGMENT_SHADER, fragmentSource);

		glAttachShader(material->shaderProgramID, vShader);
		glAttachShader(material->shaderProgramID, fShader);
		glLinkProgram(material->shaderProgramID);

		// Check linking errors
		GLint success;
		glGetProgramiv(material->shaderProgramID, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLchar info[512];
			glGetProgramInfoLog(material->shaderProgramID, 512, NULL, info);
			glDeleteProgram(material->shaderProgramID);
			glDeleteShader(vShader);
			glDeleteShader(fShader);
			CONSOLE_LOG("Shader compiling error: %s", info);
		}
		glValidateProgram(material->shaderProgramID);

		ret = LoadUniforms(material);

		glDetachShader(material->shaderProgramID, vShader);
		glDetachShader(material->shaderProgramID, fShader);

		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
	else
		CONSOLE_LOG("[ERROR] Vertex shader: %d or Fragment shader: %d are not correctly compiled.", vertexSource, fragmentSource);


	return true;
}

unsigned int I_Material::ImportShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);

	const GLchar* src = (const GLchar*)source.c_str();

	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	// Check shader compilation errors
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		GLint maxLength;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);

		// maxLength includes /0 character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);

		CONSOLE_LOG("Failed to compile shader! The type was: %d\n", type);
		std::string str(errorLog.begin(), errorLog.end());
		CONSOLE_LOG("error: %s\n", str.c_str());

		glDeleteShader(id);

		return 0;
	}
	return id;
}

bool I_Material::LoadUniforms(Material* material)
{
	material->uniforms.clear();

	GLint uniformsCount;

	glGetProgramiv(material->shaderProgramID, GL_ACTIVE_UNIFORMS, &uniformsCount);

	if (uniformsCount == 0)
	{
		GLchar info[512];
		glGetProgramInfoLog(material->shaderProgramID, 512, NULL, info);
		CONSOLE_LOG("Shader compiling error: %s", info);
		return false;
	}
	else
	{
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		const GLsizei bufSize = 32; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length

		for (GLuint i = 0; i < uniformsCount; i++)
		{
			glGetActiveUniform(material->shaderProgramID, (GLuint)i, bufSize, &length, &size, &type, name);

			if (CheckUniformName(name))
			{
				switch (type)
				{
				case GL_FLOAT:
				{
					UniformT<float>* uf = new UniformT<float>(name, type, 0.0f);
					material->AddUniform(uf);
				}
				break;
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uf2 = new UniformT<float2>(name, type, float2(1.0f, 1.0f));
					material->AddUniform(uf2);
				}
				break;
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uf3 = new UniformT<float3>(name, type, float3(1.0f, 1.0f, 1.0f));
					material->AddUniform(uf3);
				}
				break;
				case GL_FLOAT_VEC4:
				{
					UniformT<float4>* uf4 = new UniformT<float4>(name, type, float4(1.0f, 1.0f, 1.0f, 1.0f));
					material->AddUniform(uf4);
				}
				break;
				case GL_INT:
				{
					UniformT<int>* ui = new UniformT<int>(name, type, 0);
					material->AddUniform(ui);
				}
				break;
				case GL_BOOL:
				{
					UniformT<bool>* ub = new UniformT<bool>(name, type, false);
					material->AddUniform(ub);
				}
				break;
				default:
					break;
				}
			}
		}
	}
}

bool I_Material::CheckUniformName(std::string name)
{
	if (name != "time" && name != "model_matrix" &&
		name != "view" && name != "projection" && name != "resolution")
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool I_Material::Save(const Material* material)
{
	bool ret = true;

	//uint written = 0;

	//if (material == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Save Material to Library! Error: R_Material* was nullptr.");
	//	return 0;
	//}

	//float color[4] = {
	//	material->diffuseColor.r,
	//	material->diffuseColor.g,
	//	material->diffuseColor.b,
	//	material->diffuseColor.a,
	//};

	//uint size = sizeof(color);

	//if (size == 0)
	//{
	//	return 0;
	//}

	//*buffer = new char[size];
	//char* cursor = *buffer;
	//uint bytes = 0;

	//// --- COLOR DATA ---
	//bytes = sizeof(color);
	//memcpy_s(cursor, size, color, bytes);
	//cursor += bytes;

	//// --- SAVING THE BUFFER ---
	//std::string path = std::string(MATERIALS_PATH) + std::to_string(material->GetUID()) + std::string(MATERIALS_EXTENSION);

	//written = App->fileSystem->Save(path.c_str(), *buffer, size);
	//if (written > 0)
	//{
	//	CONSOLE_LOG("[STATUS] Importer Materials: Successfully Saved Material { %s } to Library! Path: { %s }", material->GetAssetsFile(), path.c_str());
	//}
	//else
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Save Material { %s } to Library! Error: File System could not Write File.", material->GetAssetsFile());
	//}

	//return written;

		//uint written = 0;

	//if (shader == nullptr)
	//{
	//	LOG("[ERROR] Importer: Could not Save R_Shader* in Library! Error: Given R_Shader* was nullptr.");
	//	return 0;
	//}

	//ParsonNode parsonFile = ParsonNode();
	//ParsonArray parsonArrray = parsonFile.SetArray("Uniforms");

	//for (uint i = 0; i < shader->uniforms.size(); i++)
	//{
	//	ParsonNode& node = parsonArrray.SetNode(shader->uniforms[i].name.c_str());
	//	node.SetString("Name", shader->uniforms[i].name.c_str());
	//	node.SetInteger("Type", (int)shader->uniforms[i].uniformType);
	//	switch (shader->uniforms[i].uniformType)
	//	{
	//	case  UniformType::INT:			node.SetNumber("Value", shader->uniforms[i].integer);		break;
	//	case  UniformType::FLOAT:		node.SetNumber("Value", shader->uniforms[i].floatNumber);	break;
	//	case  UniformType::INT_VEC2:	node.SetFloat2("Value", shader->uniforms[i].vec2);			break;
	//	case  UniformType::INT_VEC3:	node.SetFloat3("Value", shader->uniforms[i].vec3);			break;
	//	case  UniformType::INT_VEC4:	node.SetFloat4("Value", shader->uniforms[i].vec4);			break;
	//	case  UniformType::FLOAT_VEC2:	node.SetFloat2("Value", shader->uniforms[i].vec2);			break;
	//	case  UniformType::FLOAT_VEC3:	node.SetFloat3("Value", shader->uniforms[i].vec3);			break;
	//	case  UniformType::FLOAT_VEC4:	node.SetFloat4("Value", shader->uniforms[i].vec4);			break;
	//	}
	//}

	//std::string path = SHADERS_PATH + std::to_string(shader->GetUID()) + SHADERS_EXTENSION;
	//written = parsonFile.SerializeToFile(path.c_str(), buffer);

	//return written;

	return ret;
}

bool I_Material::Load(Material* material)
{
	bool ret = true;

	//if (material == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Load Material from Library! Error: R_Material* was nullptr.");
	//	return false;
	//}
	//if (buffer == nullptr)
	//{
	//	CONSOLE_LOG("[ERROR] Importer: Could not Load Material { %s } from Library! Error: Buffer was nullptr.", material->GetAssetsFile());
	//	return false;
	//}

	//char* cursor = (char*)buffer;
	//uint bytes = 0;

	//float color[4];

	//bytes = sizeof(color);
	//memcpy_s(color, bytes, cursor, bytes);
	//cursor += bytes;

	//material->diffuseColor.Set(color[0], color[1], color[2], color[3]);

	//CONSOLE_LOG("[STATUS] Importer: Successfully Loaded Material { %s } from Library!", material->GetAssetsFile());

		//ParsonNode parsonFile(buffer);
	//ParsonArray parsonArrray = parsonFile.GetArray("Uniforms");
	//Uniform uniform;
	//for (uint i = 0; i < parsonArrray.GetSize(); i++)
	//{
	//	ParsonNode node = parsonArrray.GetNode(i);
	//	uniform.name = node.GetString("Name");
	//	uniform.uniformType = (UniformType)node.GetInteger("Type");
	//	switch (uniform.uniformType)
	//	{
	//	case  UniformType::INT:			uniform.integer = node.GetNumber("Value");		break;
	//	case  UniformType::FLOAT:		uniform.floatNumber = node.GetNumber("Value");	break;
	//	case  UniformType::INT_VEC2:	uniform.vec2 = node.GetFloat2("Value");			break;
	//	case  UniformType::INT_VEC3:	uniform.vec3 = node.GetFloat3("Value");			break;
	//	case  UniformType::INT_VEC4:	uniform.vec4 = node.GetFloat4("Value");			break;
	//	case  UniformType::FLOAT_VEC2:	uniform.vec2 = node.GetFloat2("Value");			break;
	//	case  UniformType::FLOAT_VEC3:	uniform.vec3 = node.GetFloat3("Value");			break;
	//	case  UniformType::FLOAT_VEC4:	uniform.vec4 = node.GetFloat4("Value");			break;
	//	}

	//	shader->uniforms.push_back(uniform);
	//}

	return ret;
}