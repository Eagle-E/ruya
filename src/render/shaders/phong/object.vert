#version 460 core

/*
`v_` prefix denotes a "varying" variable, meaning its value 
    is interpolated by previous shaders in the pipeline.
*/

layout (location = 0) in vec3 position; // coordinate of vertex in local space of its obj
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;       // texture coordinates

uniform mat4 MVP;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

void main()
{
    gl_Position = MVP * vec4(position, 1.0);
    v_position = position;
    v_normal = normal;
    v_uv = uv;
}
