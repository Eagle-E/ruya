#version 460 core

struct Material 
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

struct Light 
{
    vec3 position;  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light; 

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPosInObjSpace;
uniform vec3 cameraPosInObjSpace;
in vec3 fragPositionInObjSpace;
in vec3 normalInLocalSpace;

out vec4 FragColor;

void main()
{
    // ambient color
    vec3 ambientComponent = light.ambient * material.ambient;

    // diffuse color
    vec3 norm = normalize(normalInLocalSpace);
    vec3 lightDir = normalize(fragPositionInObjSpace - lightPosInObjSpace);
    float diff = max(dot(-lightDir, norm), 0.0);
    vec3 diffuseComponent = light.diffuse * (diff * material.diffuse);

    // specular component
    vec3 viewDir = normalize(fragPositionInObjSpace - cameraPosInObjSpace);
    vec3 reflectionDir = reflect(lightDir, norm);
    float specularEffect = pow(max(dot(reflectionDir, -viewDir), 0.0), 32);
    vec3 specularComponent = light.specular * (specularEffect * material.specular); 

    // resulting fragment color
    vec3 result = (ambientComponent + diffuseComponent + specularComponent) * objColor;
    FragColor = vec4(result, 1.0);

} 


/*
    
    */