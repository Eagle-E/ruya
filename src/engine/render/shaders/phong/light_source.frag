#version 460 core

uniform vec3 objColor;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(objColor, 1.0f);
    FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
} 
