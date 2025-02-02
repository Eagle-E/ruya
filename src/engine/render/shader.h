#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include "glad/glad.h"
#include <glm/glm.hpp>

using std::vector;

namespace ruya
{

	class Shader
	{
	public:
		// CONSTRUCTORS & DESTRUCTOR
		Shader();
		Shader(const char * vertexShaderPath, const char * fragmentShaderPath);
		Shader(const char * vertexShaderPath, const char* geometryShaderPath, const char * fragmentShaderPath);
		~Shader();
		
		// MANIPULATORS
		void use();

		// UNIFORMS
		void setInt(const std::string& uniformName, int value);
		void setFloat(const std::string& uniformName, float value);
		void setVec3(const std::string& uniformName, const glm::vec3& vec);
		void setVec3(const std::string& uniformName, float x, float y, float z);
		void setMatrix4D(const std::string& uniformName, const glm::mat4& matrix);

		// GETTERS
		GLuint id() { return mProgramID; }

	private:
		GLuint mProgramID; // the shader id
		GLuint mVertexShaderID, mFragmentShaderID, mGeometryShaderID;

		// HELPER FUNCTIONS
		enum class Type{VERTEX_SHADER, GEOMETRY_SHADER, FRAGMENT_SHADER};
		void setShaders(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath);
		void setShader(Type shaderType, const char* shaderPath);

		GLuint createShader(GLenum shaderType, const std::string& shaderContent);
		GLuint createShaderProgram();
	};

}



#endif // !SHADER_H
