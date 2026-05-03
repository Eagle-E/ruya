#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <filesystem>


using std::vector;

namespace fs = std::filesystem;

namespace ruya::render
{

	class Shader
	{
	public:
		// CONSTRUCTORS & DESTRUCTOR
		Shader();
		Shader(const fs::path& vertex_shader_path, const fs::path& fragment_shader_path);
		Shader(const fs::path& vertex_shader_path, const fs::path& geometry_shader_path, const fs::path& fragment_shader_path);
		~Shader();
		
		// MANIPULATORS
		void use();

		// UNIFORMS
		void set_int(const std::string& uniform_name, int32_t value);
		void set_uint(const std::string& uniform_name, uint32_t value);
		void set_float(const std::string& uniform_name, float value);
		void set_vec3(const std::string& uniform_name, const glm::vec3& vec);
		void set_vec3(const std::string& uniform_name, float x, float y, float z);
		void set_mat4(const std::string& uniform_name, const glm::mat4& matrix);

		// GETTERS
		GLuint id() { return _program_id; }

	private:
		GLuint _program_id; // the shader id
		GLuint _vertex_shader_id, _fragment_shader_id, _geometry_shader_id;

		// HELPER FUNCTIONS
		enum class Type{VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};
		void set_shaders(const fs::path& vert_path, const fs::path& geom_path, const fs::path& frag_path);
		void set_shader(Type shader_type, const fs::path& shader_path);

		GLuint create_shader(GLenum shader_type, const std::string& shader_content);
		GLuint create_shader_program();
	};

}



#endif // !SHADER_H
