#include "mesh.h"

namespace
{
    /*
    * Size of number-typed vector in bytes
    */
    template <class T>
    long int vector_size_in_bytes(const vector<T>& vec)
    {
        if (vec.size() == 0) return 0;
        else return vec.size() * sizeof(vec[0]);
    }
}

/*
* Size of mesh in bytes.
* Includes byte sizes of:
*   - vertices
*   - face indexes
*   - texture coordinates
*/
long int ruya::Mesh::size() const
{
    long int total = 0;
    total += size_vertices();
    total += size_faces();
    total += size_texture_coords();
    total += size_normals();
    return total;
}

/*
* Size of vertices in bytes
*/
long int ruya::Mesh::size_vertices() const
{
    return vector_size_in_bytes(vertices);
}

/*
* Size of face indexes in bytes
*/
long int ruya::Mesh::size_faces() const
{
	return vector_size_in_bytes(faces);
}

long int ruya::Mesh::size_normals() const
{
    return vector_size_in_bytes(normals);
}

/*
* Size of texture coordinates in bytes
*/
long int ruya::Mesh::size_texture_coords() const
{
	return vector_size_in_bytes(textureCoordinates);
}

/*
* Will calculate the surface normals for each face stored in "faces" and store the results in "normals"
*   - each vertex in the "vertices" array will have an equivalent surface
*     normal at the same index in the "normals" array
*   - this will only work for objects that have vertex duplicates for each triangle
* @pre vertices and faces need to have been initialized before calling this
* @post this function will update the contents of the "normals" array with the surface normals
*/
void ruya::Mesh::update_surface_normals()
{
    normals.clear();
    normals.resize(vertices.size());

    for (uvec3& face : faces)
    {
        vec3 v1 = vertices[face[0]];
        vec3 v2 = vertices[face[1]];
        vec3 v3 = vertices[face[2]];
        vec3 normal = glm::normalize(glm::cross(v2-v1, v3-v1));
        normals[face[0]] = normal;
        normals[face[1]] = normal;
        normals[face[2]] = normal;
    }
}

/*
* Will calculate the vertex normals for each vertex in "vertices".
* @pre vertices and faces need to have been initialized before calling this
* @post this function will update the contents of the "normals" array with the vertex normals
*/
void ruya::Mesh::update_vertex_normals()
{
    normals.clear();
    normals.resize(vertices.size(), vec3(0));
    vector<unsigned int> counts(vertices.size(), 0);

    for (uvec3& face : faces)
    {
        vec3 v1 = vertices[face[0]];
        vec3 v2 = vertices[face[1]];
        vec3 v3 = vertices[face[2]];
        vec3 normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
        normals[face[0]] += normal; 
        normals[face[1]] += normal; 
        normals[face[2]] += normal; 
        counts[face[0]]++;
        counts[face[1]]++;
        counts[face[2]]++;
    }

    for (int i = 0; i < normals.size(); i++)
    {
        normals[i] /= counts[i];
    }
}
