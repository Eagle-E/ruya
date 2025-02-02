#ifndef ICOSPHERE_H
#define ICOSPHERE_H

#include "engine/scene/object.h"
#include "engine/scene/models/icosahedron.h"
#include <unordered_map>


using std::unordered_map;
using std::vector;

namespace
{
	class Vec3HashFunction {
	public:

		// Use sum of lengths of first and last names
		// as hash function.
		size_t operator()(const vec3& v) const
		{
			// the coordinates are small so making the difference on the order of magnitude 10 is enough
			return std::hash<float>()(v.x*100.0f + v.y*10.0f + v.z);
		}
	};
}

namespace ruya::models
{
	class Icosphere : public Object
	{
	public:
		Icosphere(int level_of_detail = 5)
		{
			if (level_of_detail >= mMeshes.size())
			{
				mMeshes.resize(level_of_detail + 1);
			}

			if (!mMeshes[level_of_detail])
			{
				mMeshes[level_of_detail] = init_mesh(level_of_detail);
			}

			set_mesh(mMeshes[level_of_detail]);
		}

		~Icosphere(){}

	private:
		static std::shared_ptr<Mesh> mMesh;
		static std::vector<std::shared_ptr<Mesh>> mMeshes;


		/*
		* Basically: 
		*		take an icosahedron, divide each of its triangles into 4 smaller triangles by
		*		adding a new vertex at the center of each edge and forming new triangles with the
		*		old and new vertices. When the new triangles are created, normalize each vertex
		*		to make the result more spherish.
		* 
		*		Repeat this a couple times to get more detail
		*/
		static std::shared_ptr<Mesh> init_mesh(int levelOfDetail = 5)
		{
			std::shared_ptr<Mesh> icoMesh = Icosahedron::create_icosahedron_mesh();

			// add all vertices to "addedVecs" so we can check in constant time
			// whether a given vec3 is already in the vertices list or not.
			unordered_map<vec3, unsigned int, Vec3HashFunction> addedVecs;
			for (int i = 0; i < icoMesh->vertices.size(); i++)
				addedVecs[icoMesh->vertices[i]] = i;

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

			for (int i = 0; i < levelOfDetail; i++)
			{
				vector<uvec3> newFaces;
				for (uvec3& face : icoMesh->faces)
				{
					// get the face vertices and make new ones that are
					// in the middle of each edge of the triangle
					vec3 v0 = icoMesh->vertices[face[0]];
					vec3 v1 = icoMesh->vertices[face[1]];
					vec3 v2 = icoMesh->vertices[face[2]];
					vec3 v3 = (v0 + v1) / 2.0f;
					vec3 v4 = (v1 + v2) / 2.0f;
					vec3 v5 = (v2 + v0) / 2.0f;

					// indexes of the vertices
					unsigned int iv0 = face[0];
					unsigned int iv1 = face[1];
					unsigned int iv2 = face[2];
					unsigned int iv3 = add_if_not_added_and_return_index(icoMesh->vertices, v3, addedVecs);
					unsigned int iv4 = add_if_not_added_and_return_index(icoMesh->vertices, v4, addedVecs);
					unsigned int iv5 = add_if_not_added_and_return_index(icoMesh->vertices, v5, addedVecs);
					
					// add 4 new faces
					newFaces.push_back(uvec3(iv0, iv3, iv5)); // top triangle
					newFaces.push_back(uvec3(iv3, iv4, iv5)); // center
					newFaces.push_back(uvec3(iv3, iv1, iv4)); // right
					newFaces.push_back(uvec3(iv5, iv4, iv2)); // left
				}

				// update the "faces" array
				icoMesh->faces.assign(newFaces.begin(), newFaces.end());
				
				// normalize all vertices
				for (vec3& v : icoMesh->vertices)
					v = glm::normalize(v);
			}

			// init vertex normals: for all vertices get sum of all surface normals of the faces
			// it is part of then divide it by the number of faces it is part of
			icoMesh->normals.clear();
			icoMesh->normals.resize(icoMesh->vertices.size(), vec3(0));
			vector<int> useCounts(icoMesh->vertices.size(), 0);

			for (const uvec3& face : icoMesh->faces)
			{
				// calculate normal of face
				vec3 v0 = icoMesh->vertices[face[0]];
				vec3 v1 = icoMesh->vertices[face[1]];
				vec3 v2 = icoMesh->vertices[face[2]];
				vec3 normal = glm::normalize(v0 + v1 + v2);

				// add normal to all vertices of the face
				icoMesh->normals[face[0]] += normal;
				icoMesh->normals[face[1]] += normal;
				icoMesh->normals[face[2]] += normal;

				// increment the counts of the vertices 
				useCounts[face[0]]++;
				useCounts[face[1]]++;
				useCounts[face[2]]++;
			}

			// normalize
			for (int i = 0; i < icoMesh->normals.size(); i++)
				icoMesh->normals[i] /= useCounts[i];

			return icoMesh;
		}
	};

	// init static vars
	//std::shared_ptr<Mesh> ruya::Icosphere::mMesh = init_mesh();
	std::shared_ptr<Mesh> Icosphere::mMesh;
	vector<std::shared_ptr<Mesh>> Icosphere::mMeshes(10);

}


#endif
