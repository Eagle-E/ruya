#include "cube.h"
#include "engine/scene/mesh.h"
#include "engine/scene/texture.h"
#include <memory>

using ruya::Mesh;
using ruya::Texture;

using ruya::models::Cube;


std::shared_ptr<Mesh> Cube::mMesh = init_mesh();


Cube::Cube()
{
	set_mesh(mMesh);
	//mMesh->update_surface_normals();
}

Cube::~Cube()
{
}

std::shared_ptr<Mesh> Cube::init_mesh()
{
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	
	mesh->vertices = {
		// front face
		vec3(  0.5f,  0.5f,  0.5f), // 0 rtf
		vec3(  0.5f, -0.5f,  0.5f), // 1 rbf
		vec3( -0.5f, -0.5f,  0.5f), // 2 lbf (left bottom front)
		vec3( -0.5f,  0.5f,  0.5f), // 3 ltf (left top front)

		// top face
		vec3( 0.5f,  0.5f,  0.5f), // 4
		vec3( 0.5f,  0.5f, -0.5f), // 5
		vec3(-0.5f,  0.5f, -0.5f), // 6
		vec3(-0.5f,  0.5f,  0.5f), // 7

		// bottom face
		vec3( 0.5f, -0.5f,  0.5f), // 8
		vec3( 0.5f, -0.5f, -0.5f), // 9
		vec3(-0.5f, -0.5f, -0.5f), // 10
		vec3(-0.5f, -0.5f,  0.5f), // 11

		// back face
		vec3( 0.5f,  0.5f, -0.5f),  // 12
		vec3( 0.5f, -0.5f, -0.5f),  // 13
		vec3(-0.5f, -0.5f, -0.5f), // 14
		vec3(-0.5f,  0.5f, -0.5f), // 15

		// right face
		vec3( 0.5f,  0.5f,  0.5f),  // 16
		vec3( 0.5f,  0.5f, -0.5f),  // 17
		vec3( 0.5f, -0.5f, -0.5f), // 18
		vec3( 0.5f, -0.5f,  0.5f), // 19

		// left face
		vec3(-0.5f,  0.5f,  0.5f),  // 20
		vec3(-0.5f,  0.5f, -0.5f),  // 21
		vec3(-0.5f, -0.5f, -0.5f), // 22
		vec3(-0.5f, -0.5f,  0.5f), // 23
	};

	mesh->textureCoordinates = {
		vec2( 1.0f, 1.0f ), // rt: right top
		vec2( 1.0f, 0.0f ), // rb: right bottom
		vec2( 0.0f, 0.0f ), // lb: left bottom
		vec2( 0.0f, 1.0f ), // lt

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),
	};
	
	mesh->faces = {
		uvec3(0, 1, 2), // front
		uvec3(2, 3, 0),

		uvec3(4, 5, 6), // top
		uvec3(4, 7, 6),

		uvec3(8, 9, 10),  // bottom
		uvec3(8, 11, 10),

		uvec3(12, 13, 14), // back
		uvec3(14, 15, 12),

		uvec3(16, 18, 19), // right
		uvec3(16, 17, 18),

		uvec3(20, 22, 23), // left
		uvec3(20, 21, 22),
	};

	mesh->normals = {
		// front
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),
		vec3(0.0f, 0.0f, 1.0f),

		// top 
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f),

		// bottom 
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),
		vec3(0.0f, -1.0f, 0.0f),

		// back 
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),
		vec3(0.0f, 0.0f, -1.0f),

		// right 
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),
		vec3(1.0f, 0.0f, 0.0f),

		// left 
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(-1.0f, 0.0f, 0.0f),
		vec3(-1.0f, 0.0f, 0.0f),
	};



	return mesh;
}
