#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <iostream>
#include "io/stb_image.h"

namespace ruya::render
{
	struct Texture
	{
		GLuint id = 0;

		bool is_valid() const
        {
            return id != 0; // 0 is universally invalid in OpenGL
        }
	};


	/*
	* Get the actual maximum number of texture slots available to the fragment shader
	* that are supported by the GPU. 
	* 
	* Check function print_max_texture_units_info() for a detailed overview of the
	* available texture slots per shader.
	*/
	inline int get_max_texture_units()
	{
		int maxUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
		return maxUnits;
	}


	/*
	* Prints sample info about the number of available texture slots per shader. A sample output:
	* 
	Amount of available texture slots:
		* Fragment shader: 32                 enum: "GL_MAX_TEXTURE_IMAGE_UNITS"
		* Vertex shader: 32                   enum: "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS"
		* Geometry shader: 32                 enum: "GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS"
		* Compute shader: 32                  enum: "GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS"
		* Tesselation control shader: 32      enum: "GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS"
		* Tesselation evaluation shader: 32   enum: "GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS"
		* All combined: 192                   enum: "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS"
	*/
	inline void print_max_texture_units_info()
	{
		std::cout << "Amount of available texture slots: " << std::endl;

		//That is the number of image samplers that your GPU supports in the fragment shader.
		int MaxTextureImageUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
		std::cout	<< "  * Fragment shader: "
					<< MaxTextureImageUnits << "\t\t\tenum: \"GL_MAX_TEXTURE_IMAGE_UNITS\"" << std::endl;

		// The following is for the vertex shader
		int MaxVertexTextureImageUnits;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
		std::cout << "  * Vertex shader: "
			<< MaxVertexTextureImageUnits << "\t\t\tenum: \"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS\"" << std::endl;

		// The following is for the geometry shader
		int MaxGeometryTextureImageUnits;
		glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &MaxGeometryTextureImageUnits);
		std::cout << "  * Geometry shader: "
			<< MaxGeometryTextureImageUnits << "\t\t\tenum: \"GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS\"" << std::endl;


		// The following is for the compute shader
		int MaxComputeTextureImageUnits;
		glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &MaxComputeTextureImageUnits);
		std::cout << "  * Compute shader: "
			<< MaxComputeTextureImageUnits << "\t\t\tenum: \"GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS\"" << std::endl;


		// The following is for tesselation
		int MaxTessControlTextureImageUnits;
		int MaxTessEvalTextureImageUnits;
		glGetIntegerv(GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS, &MaxTessControlTextureImageUnits);
		glGetIntegerv(GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS, &MaxTessEvalTextureImageUnits);
		std::cout << "  * Tesselation control shader: "
			<< MaxTessControlTextureImageUnits << "\tenum: \"GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS\"" << std::endl;
		std::cout << "  * Tesselation evaluation shader: "
			<< MaxTessEvalTextureImageUnits << "\tenum: \"GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS\"" << std::endl;


		// The following is for all combined, in total
		int MaxCombinedTextureImageUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
		std::cout << "  * All combined: "
			<< MaxCombinedTextureImageUnits << "\t\t\tenum: \"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS\"" << std::endl;
	}


}

#endif