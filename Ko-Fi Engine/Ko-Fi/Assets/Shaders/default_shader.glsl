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
out vec4 fragPosLightSpace;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

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
    fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

}

#shader fragment
#version 330 core

const int MAX_DIR_LIGHTS = 32;
const int MAX_POINT_LIGHTS = 32;
const int MAX_FOCAL_LIGHTS = 32; 

in vec4 ourColor;
in vec2 TexCoord;
in vec4 normal;
in vec3 fragPos;
in vec4 fragPosLightSpace;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;

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
    float range; //range of the light
    
    float ambient;
    float diffuse;
    //vec3 specular;

    float constant;
    float linear;
    float quadratic;  
      
}; uniform FocalLight focalLights[MAX_FOCAL_LIGHTS];

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, DirLight light, sampler2D shadowMap)
{
    // perform perspective divide
    //When using an orthographic projection matrix the w component of a 
    //vertex remains untouched so this step is actually quite meaningless.
    //However, it is necessary when using perspective projection so 
    //keeping this line ensures it works with both projection matrices.
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
    projCoords = projCoords * 0.5 + 0.5; 
    float closestDepth = texture(shadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z; 

    vec3 lightDir = normalize(-light.direction);
    //the bias is to reduce artifacts when looking at shadows from an angle.
    //simply offsets the depth of the shadow map a little
    float bias = 0.01;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  

    //old way
    //float bias = max(0.05 * (1.0 - dot(normal, light.direction)), 0.005); 
    
    //loop 9 times and grab the surrounding texels to average the shadows and make them smoother
    float shadow = 0.0;
    //grab the size of one sigle texel to use it to grab the ones near it
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
         for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
            
    return (1.0 - shadow);
}

vec3 CalcDirLight(DirLight light, vec3 normal, float shadow)
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
    ret = (ambient + diffuse) * shadow; //+ specular

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
    float distance = length(light.position - fragPos);

    //cosinus of the angle between the previous vector and 
        //the direction of the focal light cone
    float theta = dot(lightDir, normalize(light.direction));

    if (theta > light.cutOffAngle && distance < light.range)
    {
        // -- diffuse shading -- 
        float diff = max(dot(normal, lightDir), 0.0);

        // -- specular shading --
        //vec3 reflectDir = reflect(lightDir, vec3(normal));
        //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

        // -- attenuation -- 
        float denom = (light.constant + light.linear * distance + light.quadratic * (distance * distance));  
        float attenuation = 1.0 / denom;
        if (denom == 0.0) {
            attenuation = 1.0;
        }

        // -- combine results --
        vec3 ambient  = light.ambient  * light.color;
        vec3 diffuse  = light.diffuse  * 1.0 * light.color;
        //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        diffuse  *= attenuation;
        //specular *= attenuation;

        // -- border diffumination --
        //float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        //diffuse *= intensity;

        return (ambient + diffuse); //vec3(1.0, 0.0, 0.0);
    }
    else //in case the fragment is outside the light cone, apply just the ambient 
    {
        vec3 ambient = light.ambient  * light.color; //vec3(0.0, .0, 1.0);
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
       // Calculate shadow (temporarily only on light 0)
       float shadow = 1;
       
       if(i == 0)
       {
           shadow = ShadowCalculation(fragPosLightSpace, vec3(normal), dirLights[i], shadowMap);
       }

        outputColor += CalcDirLight(dirLights[i], vec3(normal), shadow); 
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