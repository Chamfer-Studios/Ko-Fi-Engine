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
out vec4 normal;
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
    //normals affected by model movement
    normal = normalize(model_matrix * vec4(normals, 0.0));
}

#shader fragment
#version 330 core

const int MAX_DIR_LIGHTS = 5;
const int MAX_POINT_LIGHTS = 5;
const int MAX_FOCAL_LIGHTS = 5; 

in vec4 ourColor;
in vec2 TexCoord;
in vec4 normal;
in vec3 fragPos;

out vec4 color;

uniform sampler2D ourTexture;

uniform int numOfDirectionalLights;
uniform int numOfPointLights;
uniform int numOfFocalLights;

//light definitions

struct DirLight {
    vec3 color;
    vec3 direction;
  
    float ambient;
    float diffuse;
    //float specular;

}; uniform DirLight dirLights[MAX_DIR_LIGHTS];

struct PointLight {   
    vec3 color;
    vec3 position;
    
    float ambient;
    float diffuse;
    //vec3 specular;

    float constant;
    float linear;
    float quadratic;  
      
}; uniform PointLight pointLights[MAX_POINT_LIGHTS];

struct FocalLight {   
    vec3 color;
    vec3 position;
    vec3 direction;
    float cutOffAngle; //cosine of the actual angle
    
    float ambient;
    float diffuse;
    //vec3 specular;

    float constant;
    float linear;
    float quadratic;  
      
}; uniform FocalLight focalLights[MAX_FOCAL_LIGHTS];

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
    vec3 ambient  = light.ambient * light.color;
    vec3 diffuse  = light.diffuse * diff * light.color;
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
    //vec3 reflectDir = reflect(-lightDir, vec3(normal));
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // --- attenuation --- 
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

    // --- combine results ---
    vec3 ambient  = light.ambient  * light.color;
    vec3 diffuse  = light.diffuse  * diff * light.color;
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    diffuse  *= attenuation;
    //specular *= attenuation;
    return (ambient + diffuse);
} 

vec3 CalcFocalLight(FocalLight light, vec3 normal, vec3 fragPos)
{
    //calculate vector between position of light source and fragment
    vec3 lightDir = normalize(light.position - fragPos);

    //cosinus of the angle between the previous vector and 
        //the direction of the focal light cone
    float theta = dot(lightDir, normalize(light.direction));

    if (theta > light.cutOffAngle)
    {
        // -- diffuse shading -- 
        float diff = max(dot(normal, lightDir), 0.0);

        // -- specular shading --
        //vec3 reflectDir = reflect(lightDir, vec3(normal));
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        // -- attenuation -- 
        float distance    = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    

        // -- combine results --
        vec3 ambient  = light.ambient  * light.color;
        vec3 diffuse  = light.diffuse  * diff * light.color;
        //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        diffuse  *= attenuation;
        //specular *= attenuation;

        // -- border diffumination --
        //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        //diffuse *= intensity;

        return (ambient + diffuse);
    }
    else //in case the fragment is outside the light cone, apply just the ambient 
    {
        vec3 ambient  = light.ambient  * light.color;
        return ambient;
    }
} 

void main() {

    //output color value 
    vec3 outputColor = vec3(0.0);

    // --- Add the directional light's contribution to the output color ---
    //loop dirLights
    for(int i = 0; i < numOfDirectionalLights; i++)
    {
        outputColor += CalcDirLight(dirLights[i], vec3(normal)); 
    }

    // --- Add the point light's contribution to the output color ---
    //loop dirLights
    for(int i = 0; i < numOfPointLights; i++)
    {
        outputColor += CalcPointLight(pointLights[i], vec3(normal), fragPos); 
    }

    // --- Add the focal light's contribution to the output color ---
    for(int i = 0; i < numOfFocalLights; i++)
    {
        outputColor += CalcFocalLight(focalLights[i], vec3(normal), fragPos); 
    }

    //---- Apply output to the texture ----//
    //texture
    vec4 textureColor = texture(ourTexture, TexCoord);

    //color + lighting
    color = textureColor * vec4(outputColor, 1.0);

}