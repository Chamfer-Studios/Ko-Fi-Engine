#pragma once
#include <string>

namespace shader {
	struct ShaderProgramSource {
		std::string VertexSource;
		std::string FragmentSource;
	};
	
	unsigned int CompileShader(unsigned int type, const std::string& source);

	int CreateShader(const std::string& vertexShader, const std::string fragmentShader);
	ShaderProgramSource ParseShader(const std::string& filepath);

	void DeleteShader(unsigned int shader);

}