#shader vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
layout (location = 2) in vec2 texCoord;
out vec4 ourColor;
out vec2 TexCoord;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 albedoTint;

void main()
{
gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
ourColor = albedoTint ;
TexCoord = texCoord;
}

#shader fragment
#version 330 core
in vec4 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
      //color = texture(ourTexture, TexCoord)*vec4(1.0f,1.0f,1.0f,1.0f);
      color = texture(ourTexture, TexCoord)*ourColor;
     // color = texture(ourTexture, TexCoord);
}






