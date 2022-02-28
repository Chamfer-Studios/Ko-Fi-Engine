#include "I_Shader.h"
#include "Shader.h"
#include "Log.h"

#include <fstream>
#include <sstream>
#include <MathGeoLib/Math/float2.h>
#include <MathGeoLib/Math/float3.h>
#include <MathGeoLib/Math/float4.h>
#include <glew.h>

I_Shader::I_Shader()
{
}

I_Shader::~I_Shader()
{
}

bool I_Shader::Import(const char* path, Shader* shader)
{
	bool ret = true;

	if (shader == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Shader* is nullptr.");
		return false;
	}

	std::ifstream stream(path);
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
		shader->shaderProgramID = glCreateProgram();

		// We create the vertex shader and the fragment shader
		unsigned int vShader = ImportShader(GL_VERTEX_SHADER, vertexSource);
		unsigned int fShader = ImportShader(GL_FRAGMENT_SHADER, fragmentSource);

		glAttachShader(shader->shaderProgramID, vShader);
		glAttachShader(shader->shaderProgramID, fShader);
		glLinkProgram(shader->shaderProgramID);

		// Check linking errors
		GLint success;
		glGetProgramiv(shader->shaderProgramID, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLchar info[512];
			glGetProgramInfoLog(shader->shaderProgramID, 512, NULL, info);
			glDeleteProgram(shader->shaderProgramID);
			glDeleteShader(vShader);
			glDeleteShader(fShader);
			CONSOLE_LOG("Shader compiling error: %s", info);
		}
		glValidateProgram(shader->shaderProgramID);

		ret = LoadUniforms(shader);

		glDetachShader(shader->shaderProgramID, vShader);
		glDetachShader(shader->shaderProgramID, fShader);

		glDeleteShader(vShader);
		glDeleteShader(fShader);
	}
	else
		CONSOLE_LOG("[ERROR] Vertex shader: %d or Fragment shader: %d are not correctly compiled.", vertexSource, fragmentSource);

	return ret;
}

unsigned int I_Shader::ImportShader(unsigned int type, const std::string& source)
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

bool I_Shader::LoadUniforms(Shader* shader)
{
	shader->uniforms.clear();
	GLint uniformsCount;

	glGetProgramiv(shader->shaderProgramID, GL_ACTIVE_UNIFORMS, &uniformsCount);

	if (uniformsCount == 0)
	{
		GLchar info[512];
		glGetProgramInfoLog(shader->shaderProgramID, 512, NULL, info);
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
			glGetActiveUniform(shader->shaderProgramID, (GLuint)i, bufSize, &length, &size, &type, name);

			if (CheckUniformName(name))
			{
				switch (type)
				{
				case GL_FLOAT:
				{
					UniformT<float>* uf = new UniformT<float>(name, type, 0.0f);
					shader->AddUniform(uf);
				}
				break;
				case GL_FLOAT_VEC2:
				{
					UniformT<float2>* uf2 = new UniformT<float2>(name, type, float2(1.0f, 1.0f));
					shader->AddUniform(uf2);
				}
				break;
				case GL_FLOAT_VEC3:
				{
					UniformT<float3>* uf3 = new UniformT<float3>(name, type, float3(1.0f, 1.0f, 1.0f));
					shader->AddUniform(uf3);
				}
				break;
				case GL_FLOAT_VEC4:
				{
					UniformT<float4>* uf4 = new UniformT<float4>(name, type, float4(1.0f, 1.0f, 1.0f, 1.0f));
					shader->AddUniform(uf4);
				}
				break;
				case GL_INT:
				{
					UniformT<int>* ui = new UniformT<int>(name, type, 0);
					shader->AddUniform(ui);
				}
				break;
				default:
					break;
				}
			}
		}
	}
}

bool I_Shader::CheckUniformName(std::string name)
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

bool I_Shader::Save(const Shader* shader, const char* path)
{
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

	return true;
}

bool I_Shader::Load(const char* path, Shader* shader)
{
	bool ret = true;
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