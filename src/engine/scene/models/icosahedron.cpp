#include "icosahedron.h"
#include "engine/scene/mesh.h"
#include <memory>
#include <vector>
#include <limits>
#include <iostream>

using ruya::Mesh;
using std::vector;
using glm::ivec2;
using ruya::models::Icosahedron;

namespace
{
	bool contains(const vector<int>& v, int n)
	{
		for (const int & e : v)
		{
			if (e == n) return true;
		}
		return false;
	}

	/*
	* Returns the euclidean distance between vector 1 and 2: ||v1-v2||
	*/
	float distance(const vec3& v1, const vec3& v2)
	{
		vec3 d = v1 - v2;
		return sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
	}

	/*
	* Returns index of vertex closest to vertex at index k that is not in the exclude list
	*/
	int closest_excluding(const vector<vec3>& vertices, int k, const vector<int>& exclude)
	{
		float min = std::numeric_limits<float>::max(); 
		int minIndex = -1;

		for (int i = 0; i < vertices.size(); i++)
		{
			if (i == k || contains(exclude, i))
				continue;

			float dist = distance(vertices[k], vertices[i]);
			if (dist < min)
			{
				min = dist;
				minIndex = i;
			}
		}

		return minIndex;
	}

	/*
	* Returns the indexes of the n vertices that are closest to vertex at index k
	*/
	vector<int> closest_n_neighbor_indexes(const vector<vec3>& vertices, int k, int n)
	{
		vector<int> neighbors;
		for (int i = 0; i < n; i++)
			neighbors.push_back(closest_excluding(vertices, k, neighbors));
		return neighbors;
	}

	/*
	* Get all pairs of vertices that are at a given distance from eachother
	*/
	vector<glm::ivec2> get_pairs_at_distance(const vector<vec3>& vertices, const vector<int>& indexes, float dist)
	{
		vector<glm::ivec2> pairs;
		for (int i = 0; i < indexes.size(); i++)
		{
			for (int j = i + 1; j < indexes.size(); j++)
			{
				float dist_ij = distance(vertices[indexes[i]], vertices[indexes[j]]);
				if (abs(dist_ij - dist) < 0.0001f)
					pairs.push_back(ivec2(indexes[i], indexes[j]));
			}
		}
		return pairs;
	}

	void swap(uvec3 &v, int i, int j)
	{
		unsigned int t = v[i];
		v[i] = v[j];
		v[j] = t;
	}

	/*
	* Inserts given face into faces with each index in "face" being in increasing order
	* Doesn't insert if given face already exists
	*/
	void insert_face_unique(vector<uvec3>& faces, uvec3 face)
	{
		uvec3 sf(0.0f);
		
		if (face.x > face.y) swap(face, 0, 1);
		if (face.y > face.z) swap(face, 1, 2);
		if (face.x > face.y) swap(face, 0, 1);
		if (face.y > face.z) swap(face, 1, 2);

		// quit if face already exists
		for (const uvec3& f : faces)
			if (face == f) return;

		// insert new face
		faces.push_back(face);
	}
}


std::shared_ptr<Mesh> Icosahedron::mMesh = create_icosahedron_mesh();

Icosahedron::Icosahedron()
{
	set_mesh(mMesh);
	
	// init normals
	//mMesh->update_surface_normals();


}

void Icosahedron::print_model_data() const
{
	auto printvec = [](vec3 v)
	{
		std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	};
	auto printuvec = [](uvec3 v)
	{
		std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")";
	};

	Mesh& mesh = *this->mMesh;
	std::cout << "vertices: \n";
	for (vec3& v : mesh.vertices)
	{
		std::cout << "\t";
		printvec(v);
		std::cout << "\n";
	}
	std::cout << "\n";
	std::cout << "faces: \n";
	for (uvec3& v : mesh.faces)
	{
		std::cout << "\t";
		printuvec(v);
		std::cout << "\n";
	}
}

std::shared_ptr<Mesh> Icosahedron::create_icosahedron_mesh()
{

	/*
		well make the icohasedron with 3 orthogonal golden rectangles as described in 
		https://en.wikipedia.org/wiki/Regular_icosahedron
		This image:
		https://en.wikipedia.org/wiki/File:Icosahedron-golden-rectangles.svg

		1)	There are three rectangles that are orthogonal, or in other words perpendicular to
			each other.
				* The first lies flat on the xz plane, its long side along the x-axis and the short
				  one along the z-axis. The center of the rectangle is at the origin (0,0).
				* The second one, in a similar manner, is on the xy plane with its length along the y-axis
				* The third one is on the yz plane with its length along the z-axis
		
		2)	The icosahedron will consist of the vertices of the rectangles. 
		3)	The faces will be determined as follows"
				* For each vertex, find the 5 closest vertices, these are its neighbors.
				* For each pair of neighbors that are closest with eachother make a triangle
				  with the original vertex and the pair of neighbors. There are a total of 5
				  closest pairs of neighbors per vertex on the icosahedron.
		
	*/ 

	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
	float s1 = 1.0f; // short side
	float s2 = (1.0f + sqrt(5.0f)) / 2; // long side


	// init vertices
	mesh->vertices = {
		// rectangle 1
		vec3( -s2 / 2.0f,  0.0f,  s1/2.0f),	    // left front
		vec3( -s2 / 2.0f,  0.0f, -s1/2.0f),	    // left back
		vec3(  s2 / 2.0f,  0.0f, -s1/2.0f),	    // right back
		vec3(  s2 / 2.0f,  0.0f,  s1/2.0f),	    // right front

		// rectangle 2
		vec3(-s1 / 2.0f,  s2 / 2.0f, 0.0f),     // left top
		vec3( s1 / 2.0f,  s2 / 2.0f, 0.0f),     // right top
		vec3( s1 / 2.0f, -s2 / 2.0f, 0.0f),     // right bottom
		vec3(-s1 / 2.0f, -s2 / 2.0f, 0.0f),     // left bottom

		// rectangle 3
		vec3(0.0f,  s1 / 2.0f,  s2 / 2.0f),     // front top
		vec3(0.0f,  s1 / 2.0f, -s2 / 2.0f),      // back top
		vec3(0.0f, -s1 / 2.0f, -s2 / 2.0f),      // back bottom
		vec3(0.0f, -s1 / 2.0f,  s2 / 2.0f),     // front bottom
	};

	// find the faces as described in step 2) and 3)
	for (int i = 0; i < mesh->vertices.size(); i++)
	{
		// get the 5 closest vertices to current vertex
		vector<int> neighbors = closest_n_neighbor_indexes(mesh->vertices, i, 5);
		vector<ivec2> pairs = get_pairs_at_distance(mesh->vertices, neighbors, s1);
		for (const ivec2& pair : pairs)
		{
			insert_face_unique(mesh->faces, uvec3(i, pair.x, pair.y));
		}
	}

	// init vertex normals: for all vertices get sum of all surface normals 
	// it is part of then divide it by 5 (each vertex is part of 5 triangles 
	// in an icosahedron)
	mesh->normals.clear();
	mesh->normals.resize(mesh->vertices.size(), vec3(0));

	for (const uvec3& face : mesh->faces)
	{
		vec3 v0 = mesh->vertices[face[0]];
		vec3 v1 = mesh->vertices[face[1]];
		vec3 v2 = mesh->vertices[face[2]];
		vec3 normal = glm::normalize(v0 + v1 + v2);
		mesh->normals[face[0]] += normal;
		mesh->normals[face[1]] += normal;
		mesh->normals[face[2]] += normal;
	}

	for (int i = 0; i < mesh->normals.size(); i++)
	{
		mesh->normals[i] /= 5;
	}

	return mesh;
}
