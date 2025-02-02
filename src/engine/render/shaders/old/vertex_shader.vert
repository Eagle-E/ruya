#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

uniform mat4 MVP;

out vec2 ourTexCoord;

void main()
{
    gl_Position = MVP * vec4(pos.x, pos.y, pos.z, 1.0);
    ourTexCoord = tex;
}