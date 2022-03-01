#include "Shader.h"
#include "FSDefs.h"
#include "ImGuiAppLog.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glew.h>
#include "Globals.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

void Shader::SetUniformMatrix4(std::string name, GLfloat* value)
{
	int uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniformMatrix4fv(uinformLoc, 1, GL_FALSE, value);
}

void Shader::SetUniformVec2f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform2fv(uinformLoc, 1, value);
}

void Shader::SetUniformVec3f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform3fv(uinformLoc, 1, value);
}

void Shader::SetUniformVec4f(std::string name, GLfloat* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform4fv(uinformLoc, 1, value);
}

void Shader::SetUniformVec2i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform2iv(uinformLoc, 1, value);
}

void Shader::SetUniformVec3i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform3iv(uinformLoc, 1, value);
}

void Shader::SetUniformVec4i(std::string name, GLint* value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform4iv(uinformLoc, 1, value);
}

void Shader::SetUniform1f(std::string name, GLfloat value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform1f(uinformLoc, value);
}

void Shader::SetUniform1i(std::string name, GLint value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform1i(uinformLoc, value);
}

void Shader::SetBool(std::string name, bool value)
{
	uint uinformLoc = glGetUniformLocation(ID, name.c_str());
	glUniform1i(uinformLoc, (GLint)value);
}