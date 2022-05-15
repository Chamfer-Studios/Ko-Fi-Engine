#shader vertex
#version 330 core
layout (location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

out vec2 TexCoord; 

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 totalPosition = vec4(position, 1.0f);
    mat4 viewModel = view * model_matrix;
    gl_Position = projection * viewModel * totalPosition;
    TexCoord = texCoord;
} 

#shader fragment
#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D depthMap;

void main()
{             
    float depthValue = texture(depthMap, TexCoord).r;
    FragColor = vec4(vec3(depthValue), 1.0);
}  