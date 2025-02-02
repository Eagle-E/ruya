#include "texture.h"
#include <iostream>
#include "io/stb_image.h"

ruya::Texture::Texture()
	: mWidth(0), mHeight(0), mChannels(0), mData(nullptr), mTextureID(0)
{
}

ruya::Texture::Texture(const char* texturePath)
{
	// load texture data
	stbi_set_flip_vertically_on_load(true);
	mData = stbi_load(texturePath, &mWidth, &mHeight, &mChannels, 0);

	// check if loading succeeded
	if (!mData)
	{
		std::cerr << "Error loading texture: " << stbi_failure_reason() << "\nTexture path: " << texturePath << std::endl;
		mData = nullptr;
		mTextureID = 0;
		return;
	}

	// create opengl texture
	glGenTextures(1, &mTextureID);
	glBindTexture(GL_TEXTURE_2D, mTextureID);
	
	// texture settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // wrap around in s- and t-axi
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // nearest neighbor filtering with best fitting mipmap when minifying
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // bilinear interpolation when magnifying

	// generate opengl texture and "move it to the GPU" (whether it actually gets moved is driver dependent)
	GLuint sourceColorType;
	mChannels == 3 ? sourceColorType = GL_RGB : sourceColorType = GL_RGBA;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, sourceColorType, GL_UNSIGNED_BYTE, mData);
	glGenerateMipmap(GL_TEXTURE_2D);

	/*
		Note:	activating a texture slot then binding this texture's ID will move the texture
				data to the corresponding texture slot space in the vram.
				
				For example:
					glActiveTexture(GL_TEXTURE9);
					glBindTexture(GL_TEXTURE_2D, mTextureID);

				The above code will move the data of mTextureID to slot GL_TEXTURE9.
				We can access the texture in the fragment shader by binding a uniform
				name to the texture slot's number. So if we do shader.setInt("ourTexture", 9);
				We can use "ourTexture" to access the texture data in the fragment shader by
				declaring it like "uniform sampler2D ourTexture;".
	*/
}

ruya::Texture::~Texture()
{
	stbi_image_free(mData);
}

/*
* Get the actual maximum number of texture slots available to the fragment shader
* that are supported by the GPU. 
* 
* Check function print_max_texture_slots_info() for a detailed overview of the
* available texture slots per shader.
*/
int ruya::Texture::get_num_texture_slots_fragment_shader()
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
void ruya::Texture::print_max_texture_slots_info()
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
