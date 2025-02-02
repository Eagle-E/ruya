#version 460 core

layout (location = 0) in vec3 localPosition; // coordinate of vertex in local space of its obj
layout (location = 1) in vec3 localNormal;
//layout (location = 2) in vec2 texCoords;

uniform mat4 MVP;

out VS_OUT {
    vec3 normal;
    vec3 localPosition;
} vs_out;




void main()
{
    gl_Position = MVP * vec4(localPosition, 1.0);
    vs_out.normal = localNormal;
    vs_out.localPosition = localPosition;
}