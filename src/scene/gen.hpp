#ifndef GEN_HPP
#define GEN_HPP

#include "scene/mesh.h"
#include <limits>
#include <vector>

using ruya::scene::Mesh;
using std::vector;
using glm::ivec2;


namespace
{
	class Vec3HashFunction 
    {
	public:
        // Use sum of lengths of first and last names as hash function.
        size_t operator()(const vec3& v) const
        {
            // the coordinates are small so making the difference on the order of magnitude 10 is enough
            return std::hash<float>()(v.x*100.0f + v.y*10.0f + v.z);
        }
	};

    inline bool contains(const vector<int>& v, int n)
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
	inline float distance(const vec3& v1, const vec3& v2)
	{
		vec3 d = v1 - v2;
		return sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
	}

    /*
	* Returns index of vertex closest to vertex at index k that is not in the exclude list
	*/
	inline int closest_excluding(const vector<vec3>& vertices, int k, const vector<int>& exclude)
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
	inline vector<int> closest_n_neighbor_indexes(const vector<vec3>& vertices, int k, int n)
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

namespace ruya::scene::gen
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
    [[nodiscard]] Mesh icosahedron()
    {
        Mesh mesh;
        float s1 = 1.0f; // short side
    	float s2 = (1.0f + sqrt(5.0f)) / 2; // long side

	
        mesh.vertices = {
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
        for (int i = 0; i < mesh.vertices.size(); i++)
        {
            // get the 5 closest vertices to current vertex
            vector<int> neighbors = closest_n_neighbor_indexes(mesh.vertices, i, 5);
            vector<ivec2> pairs = get_pairs_at_distance(mesh.vertices, neighbors, s1);
            for (const ivec2& pair : pairs)
            {
                insert_face_unique(mesh.faces, uvec3(i, pair.x, pair.y));
            }
        }

        // init vertex normals: for all vertices get sum of all surface normals 
        // it is part of then divide it by 5 (each vertex is part of 5 triangles 
        // in an icosahedron)
        mesh.normals.clear();
        mesh.normals.resize(mesh.vertices.size(), vec3(0));

        for (const uvec3& face : mesh.faces)
        {
            vec3 v0 = mesh.vertices[face[0]];
            vec3 v1 = mesh.vertices[face[1]];
            vec3 v2 = mesh.vertices[face[2]];
            vec3 normal = glm::normalize(v0 + v1 + v2);
            mesh.normals[face[0]] += normal;
            mesh.normals[face[1]] += normal;
            mesh.normals[face[2]] += normal;
        }

	for (int i = 0; i < mesh.normals.size(); i++)
	{
		mesh.normals[i] /= 5;
	}

        return mesh;
    }

    /*
    * Basically: 
    *		take an icosahedron, divide each of its triangles into 4 smaller triangles by
    *		adding a new vertex at the center of each edge and forming new triangles with the
    *		old and new vertices. When the new triangles are created, normalize each vertex
    *		to make the result more spherish.
    * 
    *		Repeat this a couple times to get more detail
    *
    * Arguments:
    *   - lod: level of detail, vertices roughly double or triple each level
    */
    [[nodiscard]] Mesh icosphere(int lod = 5)
    {
        Mesh ico_mesh = icosahedron();

        // add all vertices to "addedVecs" so we can check in constant time
        // whether a given vec3 is already in the vertices list or not.
        unordered_map<vec3, unsigned int, Vec3HashFunction> addedVecs;
        for (int i = 0; i < ico_mesh.vertices.size(); i++)
            addedVecs[ico_mesh.vertices[i]] = i;

        auto contains = [](const vector<vec3>& arr, const vec3 &v)
        {
            for (const vec3& e : arr)
                if (e == v) return true;
            return false;
        };


        auto add_if_not_added_and_return_index = [](vector<vec3>& arr, const vec3& v, unordered_map<vec3, unsigned int, Vec3HashFunction>& addedVecs) -> int
        {
            auto it = addedVecs.find(v);
            if (it != addedVecs.end())
            {
                return it->second;
            }
            else
            {
                arr.push_back(v);
                addedVecs[v] = arr.size() - 1;
                return arr.size() - 1;
            }

        };

        for (int i = 0; i < lod; i++)
        {
            vector<uvec3> newFaces;
            for (uvec3& face : ico_mesh.faces)
            {
                // get the face vertices and make new ones that are
                // in the middle of each edge of the triangle
                vec3 v0 = ico_mesh.vertices[face[0]];
                vec3 v1 = ico_mesh.vertices[face[1]];
                vec3 v2 = ico_mesh.vertices[face[2]];
                vec3 v3 = (v0 + v1) / 2.0f;
                vec3 v4 = (v1 + v2) / 2.0f;
                vec3 v5 = (v2 + v0) / 2.0f;

                // indexes of the vertices
                unsigned int iv0 = face[0];
                unsigned int iv1 = face[1];
                unsigned int iv2 = face[2];
                unsigned int iv3 = add_if_not_added_and_return_index(ico_mesh.vertices, v3, addedVecs);
                unsigned int iv4 = add_if_not_added_and_return_index(ico_mesh.vertices, v4, addedVecs);
                unsigned int iv5 = add_if_not_added_and_return_index(ico_mesh.vertices, v5, addedVecs);
                
                // add 4 new faces
                newFaces.push_back(uvec3(iv0, iv3, iv5)); // top triangle
                newFaces.push_back(uvec3(iv3, iv4, iv5)); // center
                newFaces.push_back(uvec3(iv3, iv1, iv4)); // right
                newFaces.push_back(uvec3(iv5, iv4, iv2)); // left
            }

            // update the "faces" array
            ico_mesh.faces.assign(newFaces.begin(), newFaces.end());
            
            // normalize all vertices
            for (vec3& v : ico_mesh.vertices)
                v = glm::normalize(v);
        }

        // init vertex normals: for all vertices get sum of all surface normals of the faces
        // it is part of then divide it by the number of faces it is part of
        ico_mesh.normals.clear();
        ico_mesh.normals.resize(ico_mesh.vertices.size(), vec3(0));
        vector<int> useCounts(ico_mesh.vertices.size(), 0);

        for (const uvec3& face : ico_mesh.faces)
        {
            // calculate normal of face
            vec3 v0 = ico_mesh.vertices[face[0]];
            vec3 v1 = ico_mesh.vertices[face[1]];
            vec3 v2 = ico_mesh.vertices[face[2]];
            vec3 normal = glm::normalize(v0 + v1 + v2);

            // add normal to all vertices of the face
            ico_mesh.normals[face[0]] += normal;
            ico_mesh.normals[face[1]] += normal;
            ico_mesh.normals[face[2]] += normal;

            // increment the counts of the vertices 
            useCounts[face[0]]++;
            useCounts[face[1]]++;
            useCounts[face[2]]++;
        }

        // normalize
        for (int i = 0; i < ico_mesh.normals.size(); i++)
            ico_mesh.normals[i] /= useCounts[i];

        return ico_mesh;
    }


    /* Generates a square mesh, centeredd at (0, 0, 0) with edge length of 1. */
    [[nodiscard]] Mesh square()
    {
        Mesh mesh;
	
        mesh.vertices = {
            vec3(-0.5f, -0.5f, 0.0f),   // bottom left
            vec3(-0.5f,  0.5f, 0.0f),   // top    left
            vec3(0.5f,  0.5f, 0.0f),    // top    right
            vec3(0.5f, -0.5f, 0.0f)     // bottom right
        };

        mesh.textureCoordinates = {
            vec2(0.0f, 0.0f), // left bottom
            vec2(0.0f, 1.0f), // left top
            vec2(1.0f, 1.0f), // right top
            vec2(1.0f, 0.0f)  // right bottom
        };
        
        mesh.faces = {
            uvec3(0, 1, 3),   // first triangle
		    uvec3(1, 2, 3)    // second triangle
        };

        mesh.normals = {
            vec3(0.0f, 0.0f, 1.0f), // two flat triangles parallel with z-axis
            vec3(0.0f, 0.0f, 1.0f)  //   => normal looks up
        };

        return mesh;
    }


    /* Generates a cube mesh, centeredd at (0, 0, 0) with edge length of 1. */
    [[nodiscard]] Mesh cube()
    {
        Mesh mesh;
	
        mesh.vertices = {
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

        mesh.textureCoordinates = {
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
        
        mesh.faces = {
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

        mesh.normals = {
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
}


#endif // GEN_HPP