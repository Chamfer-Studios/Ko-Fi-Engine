#ifndef __I_MATERIAL_H__
#define __I_MATERIAL_H__

#include <string>
#include <glew.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <fstream>
#include <sstream>
#include <vector>

#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"
#include "MathGeoLib/Math/float4.h"
#include "MathGeoLib/Math/float4x4.h"

#include "Assimp.h"


class R_Material;
class aiMaterial;

enum class ShaderType
{
	NONE = -1,
	VERTEX = 0,
	FRAGMENT = 1
};

class I_Material
{
public:
	I_Material();
	~I_Material();

	bool Import(const char* assetsPath, R_Material* material);
	bool Import(const aiMaterial* aiMaterial, R_Material* material);
	//bool Save(const R_Material* material);
	//bool Load(R_Material* material);

	bool LoadAndCreateShader(const char* shaderPath, R_Material* material);

private:
	unsigned int CreateShaderStage(unsigned int type, const std::string& source);
	bool LoadUniforms(R_Material* material);
	bool CheckUniformName(std::string name);

};

#endif // !__I_MATERIAL_H__