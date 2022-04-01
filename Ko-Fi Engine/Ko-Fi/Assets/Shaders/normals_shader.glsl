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
out vec3 normal;
out vec3 fragPos;

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
    fragPos = vec3(model_matrix * totalPosition);
    ourColor = albedoTint;
    TexCoord = texCoord;
    normal = normals;
}

#shader fragment
#version 330 core

const int MAX_DIR_LIGHTS = 5;
const int MAX_POINT_LIGHTS = 5;
const int MAX_FOCAL_LIGHTS = 5; 

in vec4 ourColor;
in vec2 TexCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform int numOfDirectionalLights;
uniform vec3 lightDirections[MAX_DIR_LIGHTS];
uniform int numOfPointLights;
uniform vec3 positionsList[MAX_POINT_LIGHTS];

//light definitions
float ambientStrength = 0.1;
vec3 lightColor = vec3(1.0, 1.0, 1.0);

struct DirLight {
    vec3 direction;
  
    float ambient;
    float diffuse;
    //float specular;
}; 

struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    float ambient;
    float diffuse;
    //vec3 specular;
};  

vec3 CalcDirLight(DirLight light, vec3 normal)
{
    vec3 ret = vec3(0.0);

    vec3 lightDir = normalize(-light.direction);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   
    // combine results
    vec3 ambient  = light.ambient * lightColor;
    vec3 diffuse  = light.diffuse * diff * lightColor;
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ret = ambient + diffuse; //+ specular

    return ret;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // --- diffuse shading --- 
    float diff = max(dot(normal, lightDir), 0.0);

    // --- specular shading ---
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // --- attenuation --- 
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    // --- combine results ---
    vec3 ambient  = light.ambient  * lightColor;
    vec3 diffuse  = light.diffuse  * diff * lightColor;
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    //specular *= attenuation;
    return (ambient + diffuse);
} 

void main() {

    //output color value
    vec3 output = vec3(0.0);

    //---- Add the directional light's contribution to the output color ----//
    DirLight currentLight;
    currentLight.ambient = 0.2;
    currentLight.diffuse = 0.7;

    //loop dirLights
    for(int i = 0; i < numOfDirectionalLights; i++)
    {
        currentLight.direction = lightDirections[i];
        output += CalcDirLight(currentLight, normal); 
        //output += CalcDirLight(currentLight, normal, viewDir);  with viewDir for specular light
    }

    //---- Add the point light's contribution to the output color ----//
    PointLight currentPLight;
    currentPLight.ambient = 0.2;
    currentPLight.diffuse = 0.7;
    currentPLight.constant = 1.0;
    currentPLight.linear = .22;
    currentPLight.quadratic = .2;

    //loop dirLights
    for(int i = 0; i < numOfPointLights; i++)
    {
        currentPLight.position = positionsList[i];
        output += CalcPointLight(currentPLight, normal, fragPos); 
    }

    //---- Apply output to the texture ----//
    //texture
    vec4 textureColor = texture(ourTexture, TexCoord);

    //color + lighting
    color = textureColor * vec4(output, 1.0);

}