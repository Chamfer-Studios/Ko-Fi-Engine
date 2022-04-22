#include "I_Material.h"
#include "R_Material.h"
#include "Engine.h"
#include "M_FileSystem.h"
#include "M_ResourceManager.h"

#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "FSDefs.h"
#include "Globals.h"
#include "Color.h"
#include "Log.h"

I_Material::I_Material(KoFiEngine* engine) : engine(engine)
{
}

I_Material::~I_Material()
{
}

bool I_Material::Import(const char* assetsPath, R_Material* material)
{
	bool ret = true;

	if (material == nullptr)
	{
		CONSOLE_LOG("[ERROR] Importer: Could not Import Material! Error: R_Material* was nullptr.");
		return false;
	}

	ret = LoadAndCreateShader(assetsPath, material);

	return ret;
}

bool I_Material::Import(const aiMaterial* aiMaterial, R_Material* material)
{
	bool ret = true;

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

	UID forcedUid = engine->GetResourceManager()->GetForcedUIDFromMeta(material->GetAssetPath());
	if (forcedUid != 0)
		material->ForceUID(forcedUid);

	aiColor4D color;
	if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
	{
		material->diffuseColor = Color(color.r, color.g, color.b, color.a);
	}

	std::string defaultPath = ASSETS_SHADERS_DIR + std::string(DEFAULT_SHADER) + SHADER_EXTENSION;
	material->SetShaderPath(defaultPath.c_str());

	ret = LoadAndCreateShader(defaultPath.c_str(), material);

	return ret;
}

bool I_Material::Save(const R_Material* material, const char* path)
{
	return engine->GetFileSystem()->CopyFileTo(material->GetAssetPath(), path);
}

bool I_Material::LoadAndCreateShader(const char* shaderPath, R_Material* material)
{
	bool ret = true;

	std::ifstream stream(shaderPath);
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
		uint vShader = CreateShaderStage(GL_VERTEX_SHADER, vertexSource);
		uint fShader = CreateShaderStage(GL_FRAGMENT_SHADER, fragmentSource);

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
	{
		CONSOLE_LOG("[ERROR] Vertex shader: %d or Fragment shader: %d are not correctly compiled.", vertexSource, fragmentSource);
		ret = false;
	}
	
	return ret;
}

unsigned int I_Material::CreateShaderStage(unsigned int type, const std::string& source)
{
	uint id = glCreateShader(type);

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

bool I_Material::LoadUniforms(R_Material* material)
{
	material->uniforms.clear();
	material->uniforms.shrink_to_fit();

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
			
			std::string sName = name;
			if (sName.find("dirLights") != std::string::npos ||
				sName.find("pointLights") != std::string::npos ||
				sName.find("numOfDirectionalLights") != std::string::npos ||
				sName.find("numOfPointLights") != std::string::npos)
			{
				continue;
			}

			if (CheckUniformName(name))
			{
				switch (type)
				{
				case GL_INT:
				{
					UniformT<int>* ui = new UniformT<int>(name, type, 0);
					material-> AddUniform(ui);
				}
				break;
				case GL_FLOAT:
				{
					UniformT<float>* uf = new UniformT<float>(name, type, 0.0f);
					material->AddUniform(uf);
				}
				break;
				case GL_BOOL:
				{
					UniformT<bool>* ub = new UniformT<bool>(name, type, false);
					material->AddUniform(ub);
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
				//case GL_FLOAT_MAT4:
				//{
				//	UniformT<float4x4>* mat = new UniformT<float4x4>(name, type, float4x4(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f));
				//	material->AddUniform(mat);
				//}
				//break;
				default:
					break;
				}
			}
		}
		return true;
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