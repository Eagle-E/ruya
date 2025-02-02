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
in vec3 surfaceNormalInLocalSpace;

out vec4 FragColor;

void main()
{
    // ambient color
    vec3 ambientComponent = light.ambient * material.ambient;

    // diffuse color
    vec3 norm = normalize(surfaceNormalInLocalSpace);
    vec3 lightDir = normalize(lightPosInObjSpace - fragPositionInObjSpace);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuseComponent = light.diffuse * (diff * material.diffuse);

    // resulting fragment color
    vec3 result = (ambientComponent + diffuseComponent) * objColor;
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(fragPositionInObjSpace, 1.0);
} 
