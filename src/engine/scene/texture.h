#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

namespace ruya
{
	
	
	class Texture
	{
	public:
		Texture();
		Texture(const char* texturePath);
		~Texture();

		GLuint ID() const { return mTextureID; }
		int height() const { return mHeight; }
		int width() const { return mWidth; }
		int channels() const { return mChannels; }
		unsigned char* data() { return mData; }

		static void print_max_texture_slots_info();
		static int get_num_texture_slots_fragment_shader();

	private:

		int mWidth;
		int mHeight;
		int mChannels; // number of color channels: rgb = 3
		unsigned char* mData;
		GLuint mTextureID;
	};


}

#endif