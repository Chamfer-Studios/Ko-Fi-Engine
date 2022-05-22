#shader vertex
#version 440 core
layout (location = 0) in vec3 position;
uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;


void main()
{
gl_Position = projection * view * model_matrix * vec4(position, 1.0f);
}

#shader fragment
#version 440 core
layout(location = 0) out vec4 outputColor;

uniform vec4 color;

void main()
{
    outputColor = color;
}