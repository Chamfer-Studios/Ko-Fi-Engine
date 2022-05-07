#ifndef __SKYBOX_H__
#define __SKYBOX_H__

#include "Globals.h"

#include <string>
#include <vector>

#include "MathGeoLib/Math/float4.h"

class R_Material;
class KoFiEngine;
class C_Camera;

class SkyBox {

    enum ClearFlag { // How the Camera Clears The BackGround
        SKYBOX,
        SOLIDCOLOR,
        DEPTH,
        NOTHING,
    };


public:
   // SkyBox(KoFiEngine* engine);
    SkyBox();
	~SkyBox();

    void Start();
	void Update(float dt);
	void CleanUp();
    void InspectorDraw();

	void InitSkyBox();
    void DrawSkyBox();

	void InitMesh();
	void InitMaterial();
	void InitTexture();

    void SetClearFlag(ClearFlag flag);

public:
    KoFiEngine *engine = nullptr;
    ClearFlag clearFlag = ClearFlag::SKYBOX;

    float skyboxVertices[108] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // SkyBox
    uint VAO = 0;
    uint VBO = 0;
    uint textureID = 0;

    R_Material* material = nullptr;

    std::vector<std::string> faces;

    int width, height, nrChannels;

    // Solid Color
    float4 Color;

};

#endif // __SKYBOX_H__
