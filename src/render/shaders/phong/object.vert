#version 460 core

/*
`v_` prefix denotes a "varying" variable, meaning its value 
    is interpolated by previous shaders in the pipeline.
*/

layout (location = 0) in vec3 position; // coordinate of vertex in local space of its obj
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;       // texture coordinates

// uniform mat4 MVP;
uniform mat4 M;
uniform mat4 VP;
out vec3 v_position;
out vec3 v_normal;
out vec2 v_uv;

void main()
{
    vec4 world_pos = M * vec4(position, 1.0);
    v_position = world_pos.xyz;
    gl_Position = VP * world_pos;

    mat3 M_norm = mat3(transpose(inverse(M)));
    v_normal = M_norm * normal;
    
    v_uv = uv;
}
