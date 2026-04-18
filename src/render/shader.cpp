#include "shader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>
#include "io/file_io.hpp"


ruya::render::Shader::Shader() : _program_id(0), _vertex_shader_id(0), _fragment_shader_id(0), _geometry_shader_id(0)
{
	
}

ruya::render::Shader::Shader(const fs::path& vertex_shader_path, const fs::path& fragment_shader_path)
	: _program_id(0), _vertex_shader_id(0), _fragment_shader_id(0), _geometry_shader_id(0)
{
	set_shader(Type::VERTEX_SHADER, vertex_shader_path.string().c_str());
	set_shader(Type::FRAGMENT_SHADER, fragment_shader_path.string().c_str());
	create_shader_program();
}


ruya::render::Shader::Shader(const fs::path& vertex_shader_path, const fs::path& geometry_shader_path, const fs::path& fragment_shader_path)
: _program_id(0), _vertex_shader_id(0), _fragment_shader_id(0), _geometry_shader_id(0)
{
	set_shader(Type::VERTEX_SHADER, vertex_shader_path.string().c_str());
	set_shader(Type::FRAGMENT_SHADER, fragment_shader_path.string().c_str());
	set_shader(Type::GEOMETRY_SHADER, geometry_shader_path.string().c_str());
	create_shader_program();
}

ruya::render::Shader::~Shader()
{
	glDeleteShader(_vertex_shader_id);
	glDeleteShader(_fragment_shader_id);
	glDeleteShader(_geometry_shader_id);
}

/*
* Creates an opengl shader program with the given vertex and fragment shaders
* @throws std::runtime_error when something went wrong, appropriate message is passed with the exception obj
*/
void ruya::render::Shader::set_shaders(const fs::path& vert_path, const fs::path& geom_path, const fs::path& frag_path)
{
	try
	{
		// get contents of the vertex and fragment shaders
		std::string vert_str = "";	  
		std::string frag_str = "";  
		std::string geom_str = "";  

		if (vert_path != "")		vert_str = ruya::io::file_contents(vert_path);
		if (geom_path != "")	frag_str = ruya::io::file_contents(geom_path);
		if (frag_path != "")	geom_str = ruya::io::file_contents(frag_path);


		// create the shaders and the shader program
		GLuint vert_id = 0, frag_id = 0, geom_id = 0;
		if (vert_str != "") vert_id = create_shader(GL_VERTEX_SHADER, vert_str);
		if (frag_str != "")	frag_id = create_shader(GL_FRAGMENT_SHADER, frag_str);
		if (geom_str != "")	geom_id = create_shader(GL_GEOMETRY_SHADER, geom_str);
		_program_id = create_shader_program();
		
		// delete shaders, not needed afer successful linkage
		glDeleteShader(vert_id);
		glDeleteShader(frag_id);
	}
	catch (std::ifstream::failure e)
	{
		std::string error_msg = "Exception: something went wrong while opening/reading/closing the vertex or fragment shader:\n=>";
		error_msg += e.what();
		throw std::runtime_error(error_msg);
	}
}

/*
* Sets the corresponding shader type to given shader file
* @pre: shader_path must be the path of a shader file, cannot be ""
*/
void ruya::render::Shader::set_shader(Type shader_type, const fs::path& shader_path)
{
	try
	{
		if (shader_type == Type::VERTEX_SHADER)
		{
			if (_vertex_shader_id > 0) glDeleteShader(_vertex_shader_id);
			_vertex_shader_id = create_shader(GL_VERTEX_SHADER, ruya::io::file_contents(shader_path));
		}
		else if (shader_type == Type::GEOMETRY_SHADER)
		{
			if (_geometry_shader_id > 0) glDeleteShader(_geometry_shader_id);
			_geometry_shader_id = create_shader(GL_GEOMETRY_SHADER, ruya::io::file_contents(shader_path));
		}
		else if (shader_type == Type::FRAGMENT_SHADER)
		{
			if (_fragment_shader_id > 0) glDeleteShader(_fragment_shader_id);
			_fragment_shader_id = create_shader(GL_FRAGMENT_SHADER, ruya::io::file_contents(shader_path));
		}
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << '\n';
	}
}

/*
* Makes the shader program current -> OpenGL will render with the shaders of current program.
*/
void ruya::render::Shader::use()
{
	glUseProgram(_program_id);
}

void ruya::render::Shader::set_int(const std::string& uniform_name, int value)
{
	glUniform1i(glGetUniformLocation(_program_id, uniform_name.c_str()), value);
}

void ruya::render::Shader::set_mat4(const std::string& uniform_name, const glm::mat4& matrix)
{
	unsigned int loc = glGetUniformLocation(_program_id, uniform_name.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ruya::render::Shader::set_vec3(const std::string& uniform_name, const glm::vec3& vec)
{
	unsigned int loc = glGetUniformLocation(_program_id, uniform_name.c_str());
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

void ruya::render::Shader::set_vec3(const std::string& uniform_name, float x, float y, float z)
{
	unsigned int loc = glGetUniformLocation(_program_id, uniform_name.c_str());
	glUniform3f(loc, x, y, z);
}

void ruya::render::Shader::set_float(const std::string& uniform_name, float value)
{
	unsigned int loc = glGetUniformLocation(_program_id, uniform_name.c_str());
	glUniform1f(loc, value);
}

/*####################################################################################################################################
*
*	Helper Functions
* 
####################################################################################################################################*/

/*
* Creates a shader of the given type in the OpenGL context.
* @throws std::runtime_error if something went wrong.
* @returns shader id in opengl context if the shader was successfully created and compiled.
*/
GLuint ruya::render::Shader::create_shader(GLenum shader_type, const std::string& shader_content)
{
	GLuint shader_id;
	shader_id = glCreateShader(shader_type);
	const char* temp = shader_content.c_str();
	glShaderSource(shader_id, 1, &temp, NULL);
	glCompileShader(shader_id);
	
	// error checking for shader compilation
	int success;
	char text_buffer[512];
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader_id, 512, NULL, text_buffer);
		
		// compose error message and throw std::runtime_error
		std::string error_msg = "";
		error_msg = "[ruya::render::Shader::create_shader()] ";
		if (shader_type == GL_VERTEX_SHADER) error_msg += "Vertex ";
		else if (shader_type == GL_FRAGMENT_SHADER) error_msg += "Fragment ";
		error_msg += "shader compilation failed.\n";
		error_msg += text_buffer;
		throw std::runtime_error(error_msg);
	}

	return shader_id;
}



/*
* Creates a shader program, then attaches shaders created so far (with set_shader()), then links program.
*	- Also deletes current program if there is one, and sets _program_id to the newly created program.
* @throws std::runtime_error is linkage failed.
* @returns the program id of the created shader program.
*/
GLuint ruya::render::Shader::create_shader_program()
{
	// link shaders into one shader program to be used for the render calls
	if (_program_id > 0) glDeleteProgram(_program_id);
	_program_id = glCreateProgram();

	for (const GLuint shader_id : {_vertex_shader_id, _fragment_shader_id, _geometry_shader_id})
	{
		if (shader_id > 0) glAttachShader(_program_id, shader_id);
	}
	glLinkProgram(_program_id);

	// errror checking for program linkage
	int success;
	char text_buffer[512];
	glGetProgramiv(_program_id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(_program_id, 512, NULL, text_buffer);
		std::string error_msg = "[ruya::render::Shader::create_shader_program()] Shader program linkage failed.\n";
		error_msg += text_buffer;
		error_msg += "\n";
		throw std::runtime_error(error_msg);
	}

	return _program_id;
}
