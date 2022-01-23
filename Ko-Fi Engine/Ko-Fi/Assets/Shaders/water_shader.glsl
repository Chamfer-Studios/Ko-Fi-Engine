#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 2) in vec2 texcoord;

out vec4 clipSpace;
out vec2 texcoords;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	
	clipSpace = projection * view * model_matrix * vec4(position, 1.0f);
	gl_Position = clipSpace;
	texcoords = texcoord;
}

#shader fragment
#version 330 core
in vec4 clipSpace;
in vec2 texcoords;

out vec4 color;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;
uniform sampler2D dudvTex;

uniform vec2 tile_size;
uniform float wave;
uniform float time;
const float water_speed = 0.1f;

void main()
{
	float n_time = time * water_speed;
	vec2 ndc = (clipSpace.xy/clipSpace.w)/2.0 + 0.5;
	vec2 refractTexCoord = vec2(ndc.x, ndc.y);
	vec2 reflectTexCoord = vec2(ndc.x, 1-ndc.y);

	vec2 tiled_coords = fract(texcoords/tile_size);
	vec2 distortion1 = (texture(dudvTex, vec2(tiled_coords.x + n_time, tiled_coords.y)).rg * 2.0 - 1.0) * wave;
	vec2 distortion2 = (texture(dudvTex, vec2(-tiled_coords.x + n_time, tiled_coords.y + n_time)).rg * 2.0 -1.0) * wave;

	vec2 total_dis = distortion1 + distortion2;

	refractTexCoord += total_dis;
	refractTexCoord = clamp(refractTexCoord, 0.001, 0.999);
	
	reflectTexCoord += total_dis;
	reflectTexCoord = clamp(reflectTexCoord, 0.001, 0.999);

	vec4 reflectColor = texture(reflectionTex, vec2(reflectTexCoord.x, reflectTexCoord.y));
	vec4 refractColor = texture(refractionTex, vec2(refractTexCoord.x, refractTexCoord.y));
	
	color = mix(reflectColor, refractColor, 0.5);
	color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}




