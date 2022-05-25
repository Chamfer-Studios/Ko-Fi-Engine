#include "SkyBox.h"

#include "Engine.h"


#include "Importer.h"

#include "R_Material.h"

#include "MathGeoLib/Math/float4x4.h"

#include "glew.h"


#include "stb_image.h"
#include <iostream>

//SkyBox::SkyBox(KoFiEngine* engine)
//{
//	this->engine = engine;
//
//	faces =
//	{
//	"EngineConfig/Skybox/right.jpg",
//	"EngineConfig/Skybox/ngleft.jpg",
//	"EngineConfig/Skybox/top.jpg",
//	"EngineConfig/Skybox/bottom.jpg",
//	"EngineConfig/Skybox/front.jpg",
//	"EngineConfig/Skybox/back.jpg"
//	};
//}

SkyBox::SkyBox()
{
	//faces =
	//{
	//"EngineConfig/Skybox/right.jpg",
	//"EngineConfig/Skybox/left.jpg",
	//"EngineConfig/Skybox/top.jpg",
	//"EngineConfig/Skybox/bottom.jpg",
	//"EngineConfig/Skybox/front.jpg",
	//"EngineConfig/Skybox/back.jpg"
	//};

	faces =
	{

	"EngineConfig/Skybox/DaySkybox/right.png",
	"EngineConfig/Skybox/DaySkybox/left.png",
	"EngineConfig/Skybox/DaySkybox/top.png",
	"EngineConfig/Skybox/DaySkybox/bottom.png",
	"EngineConfig/Skybox/DaySkybox/front.png",
	"EngineConfig/Skybox/DaySkybox/back.png"

	};
}


SkyBox::~SkyBox()
{
	CleanUp();
}

void SkyBox::Start()
{

	InitSkyBox();
}

void SkyBox::Update(float dt)
{
}

void SkyBox::CleanUp()
{
}

void SkyBox::InspectorDraw()
{
	static const char* types[]{ "SkyBox", "SolidColor", "Depth", "Don't Clear"};
	static int selectedItem = 0;
	ImGui::Text("Clear Flags");
	if (ImGui::Combo("fadf", &selectedItem, types, IM_ARRAYSIZE(types)))
	{
		if (selectedItem == 0)
			SetClearFlag(ClearFlag::SKYBOX);
		if (selectedItem == 1)
			SetClearFlag(ClearFlag::SOLIDCOLOR);
	
		ImGui::EndCombo();
	}

}

void SkyBox::InitSkyBox()
{
	InitMesh();
	InitMaterial();
	InitTexture();
}

void SkyBox::DrawSkyBox()
{
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void SkyBox::InitMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

}

void SkyBox::InitMaterial()
{
	material = new R_Material();
	material->SetAssetPath("EngineConfig/Skybox/skybox.glsl");
	Importer::GetInstance()->materialImporter->LoadAndCreateShader("EngineConfig/Skybox/skybox.glsl", material);
}

void SkyBox::InitTexture()
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

void SkyBox::SetClearFlag(ClearFlag flag)
{
	clearFlag = flag;
}
