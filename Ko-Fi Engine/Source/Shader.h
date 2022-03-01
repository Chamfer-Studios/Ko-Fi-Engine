#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include <vector>
#include "Globals.h"
#include "glew.h"
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"

class Shader
{
public:
	Shader();
	~Shader();

	// Functionality to be able to save/modify data of Shader
	void SetUniformMatrix4(std::string name, GLfloat* value);

	void SetUniformVec2f(std::string name, GLfloat* value);
	void SetUniformVec3f(std::string name, GLfloat* value);
	void SetUniformVec4f(std::string name, GLfloat* value);

	void SetUniformVec2i(std::string name, GLint* value);
	void SetUniformVec3i(std::string name, GLint* value);
	void SetUniformVec4i(std::string name, GLint* value);

	void SetUniform1f(std::string name, GLfloat value);
	void SetUniform1i(std::string name, GLint value);
	void SetBool(std::string name, bool value);

public:
	// TODO: we dont have vertext id or fragment id. Should we have this?
	uint ID = 0;
};

#endif // !__SHADER_H__