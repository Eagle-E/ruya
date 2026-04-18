#version 460 core

uniform vec3 obj_color;

out vec4 FragColor;

void main()
{
    FragColor = vec4(obj_color, 1.0f);
} 
