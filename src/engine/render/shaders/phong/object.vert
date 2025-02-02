#version 460 core

layout (location = 0) in vec3 vertexLocalPos; // coordinate of vertex in local space of its obj
layout (location = 1) in vec3 inpNormal;
//layout (location = 2) in vec2 texCoords;

uniform mat4 MVP;
out vec3 fragPositionInObjSpace;
out vec3 normalInLocalSpace;

void main()
{
    gl_Position = MVP * vec4(vertexLocalPos, 1.0);
    normalInLocalSpace = inpNormal;
    fragPositionInObjSpace = vertexLocalPos;
}