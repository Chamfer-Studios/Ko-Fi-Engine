#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normals;
layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

out vec4 ourColor;
out vec2 TexCoord;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 finalBonesMatrices[MAX_BONES];
uniform bool isAnimated;
uniform vec4 albedoTint;

void main() {
    vec4 totalPosition = vec4(0.0f);
    //vec4 totalPosition = vec4(0.0f);
    vec4 localPosition = vec4(0.0f);
    if(isAnimated == true) {
        for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {

            if(boneIds[i] >= MAX_BONES) {
                totalPosition = vec4(position, 1.0f);
                break;
            }

            localPosition = finalBonesMatrices[int(boneIds[i])] * vec4(position, 1.0f);
            totalPosition += localPosition * weights[i];
        }
    }
    if(isAnimated == false) {
        totalPosition = vec4(position, 1.0f);
    }

    mat4 viewModel = view * model_matrix;
    gl_Position = projection * viewModel * totalPosition;
//gl_Position = vec3(1.0f);
    ourColor = albedoTint;
    TexCoord = texCoord;
}

#shader fragment
#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main() {
      //color = texture(ourTexture, TexCoord)*vec4(1.0f,1.0f,1.0f,1.0f);
    color = texture(ourTexture, TexCoord) * ourColor;
     // color = texture(ourTexture, TexCoord);
}