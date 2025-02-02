#ifndef CUBE_H
#define CUBE_H

#include "engine/scene/object.h"

namespace ruya::models
{

	class Cube : public ruya::Object
	{
	public:
		Cube();
		~Cube();

	private:
		static std::shared_ptr<Mesh> init_mesh();
		static std::shared_ptr<Mesh> mMesh;
	};

}
#endif
