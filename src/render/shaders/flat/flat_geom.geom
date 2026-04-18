#version 460 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 normal;
    vec3 localPosition;
} gs_in[];

in vec3 normal[]; // 3 normals for triangle, one for each vertex
out vec3 surfaceNormalInLocalSpace;
out vec3 fragPositionInObjSpace;

void main() {    
    // triangle vertices 
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;
    vec4 v2 = gl_in[2].gl_Position;
    
    // take average of vertex normals. If normals are surface normals the result will be the same.
    // otherwise vertex normals (used for smooth shading) will be converted to surface normals;
    vec3 surfaceNormal = (gs_in[0].normal + gs_in[1].normal + gs_in[2].normal) / 3.0;

    // all fragments in triangle will be processed the same as the fragment at the center
    // of the triangle, this way fragment positions aren't interpolated.
    vec3 fragmentPosition = (gs_in[0].localPosition + gs_in[1].localPosition + gs_in[2].localPosition)/3.0;
    
    
    // pass through triangle
    gl_Position = v0;
    surfaceNormalInLocalSpace  = surfaceNormal;
    fragPositionInObjSpace = fragmentPosition;
    EmitVertex();   

    gl_Position = v1;
    surfaceNormalInLocalSpace  = surfaceNormal;
    fragPositionInObjSpace = fragmentPosition;
    EmitVertex();   

    gl_Position = v2;
    surfaceNormalInLocalSpace  = surfaceNormal;
    fragPositionInObjSpace = fragmentPosition;
    EmitVertex();   
    EndPrimitive();
}  