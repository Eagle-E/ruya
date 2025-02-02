#include "shader.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <glm/gtc/type_ptr.hpp>

// hidden namespace for helper functions that are not really related to the class
namespace
{
	/*
	* Read the contents of the given filename/path.
	* @throws std::ifstream::failure if something went wrong while trying to read the file
	*/
	std::string readFileContents(const char* fileName)
	{
		// init file object
		std::ifstream vertexShaderFile;
		vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // throw exception when things fail
		vertexShaderFile.open(fileName);

		// copy file contents to string
		std::string vertexShaderText(
			(std::istreambuf_iterator<char>(vertexShaderFile)),
			(std::istreambuf_iterator<char>())
		);

		vertexShaderFile.close();
		return vertexShaderText;
	}
}

ruya::Shader::Shader() : mProgramID(0), mVertexShaderID(0), mFragmentShaderID(0), mGeometryShaderID(0)
{
	
}

ruya::Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
	: mProgramID(0), mVertexShaderID(0), mFragmentShaderID(0), mGeometryShaderID(0)
{
	//setShaders(vertexShaderPath, "", fragmentShaderPath);
	setShader(Type::VERTEX_SHADER, vertexShaderPath);
	setShader(Type::FRAGMENT_SHADER, fragmentShaderPath);
	createShaderProgram();
}


ruya::Shader::Shader(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath)
	: mProgramID(0), mVertexShaderID(0), mFragmentShaderID(0), mGeometryShaderID(0)
{
	setShader(Type::VERTEX_SHADER, vertexShaderPath);
	setShader(Type::FRAGMENT_SHADER, fragmentShaderPath);
	setShader(Type::GEOMETRY_SHADER, geometryShaderPath);
	createShaderProgram();
}

ruya::Shader::~Shader()
{
	glDeleteShader(mVertexShaderID);
	glDeleteShader(mFragmentShaderID);
	glDeleteShader(mGeometryShaderID);
}

/*
* Creates an opengl shader program with the given vertex and fragment shaders
* @throws std::runtime_error when something went wrong, appropriate message is passed with the exception obj
*/
void ruya::Shader::setShaders(const char* vertexShaderPath, const char* geometryShaderPath, const char* fragmentShaderPath)
{
	try
	{
		// get contents of the vertex and fragment shaders
		std::string vertexShaderText = "";	  
		std::string fragmentShaderText = "";  
		std::string geometryShaderText = "";  

		if (vertexShaderPath != "")		vertexShaderText = readFileContents(vertexShaderPath);
		if (fragmentShaderPath != "")	fragmentShaderText = readFileContents(fragmentShaderPath);
		if (geometryShaderPath != "")	geometryShaderText = readFileContents(geometryShaderPath);


		// create the shaders and the shader program
		GLuint vertexShaderID = 0, fragmentShaderID = 0, geometryShaderID = 0;
		if (vertexShaderText != "")		vertexShaderID = createShader(GL_VERTEX_SHADER, vertexShaderText);
		if (fragmentShaderText != "")	fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fragmentShaderText);
		if (geometryShaderText != "")	geometryShaderID = createShader(GL_GEOMETRY_SHADER, geometryShaderText);
		mProgramID = createShaderProgram();
		
		// delete shaders, not needed afer successful linkage
		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);
	}
	catch (std::ifstream::failure e)
	{
		std::string errorMsg = "Exception: something went wrong while opening/reading/closing the vertex or fragment shader:\n=>";
		errorMsg += e.what();
		throw std::runtime_error(errorMsg);
	}
}

/*
* Sets the corresponding shader type to given shader file
* @pre: shaderPath must be the path of a shader file, cannot be ""
*/
void ruya::Shader::setShader(Type shaderType, const char* shaderPath)
{
	if (shaderType == Type::VERTEX_SHADER)
	{
		if (mVertexShaderID > 0) glDeleteShader(mVertexShaderID);
		mVertexShaderID = createShader(GL_VERTEX_SHADER, readFileContents(shaderPath));
	}
	else if (shaderType == Type::GEOMETRY_SHADER)
	{
		if (mGeometryShaderID > 0) glDeleteShader(mGeometryShaderID);
		mGeometryShaderID = createShader(GL_GEOMETRY_SHADER, readFileContents(shaderPath));
	}
	else if (shaderType == Type::FRAGMENT_SHADER)
	{
		if (mFragmentShaderID > 0) glDeleteShader(mFragmentShaderID);
		mFragmentShaderID = createShader(GL_FRAGMENT_SHADER, readFileContents(shaderPath));
	}
}

/*
* Makes the shader program current -> OpenGL will render with the shaders of current program.
*/
void ruya::Shader::use()
{
	glUseProgram(mProgramID);
}

void ruya::Shader::setInt(const std::string& uniformName, int value)
{
	glUniform1i(glGetUniformLocation(mProgramID, uniformName.c_str()), value);
}

void ruya::Shader::setMatrix4D(const std::string& uniformName, const glm::mat4& matrix)
{
	unsigned int loc = glGetUniformLocation(mProgramID, uniformName.c_str());
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void ruya::Shader::setVec3(const std::string& uniformName, const glm::vec3& vec)
{
	unsigned int loc = glGetUniformLocation(mProgramID, uniformName.c_str());
	glUniform3f(loc, vec.x, vec.y, vec.z);
}

void ruya::Shader::setVec3(const std::string& uniformName, float x, float y, float z)
{
	unsigned int loc = glGetUniformLocation(mProgramID, uniformName.c_str());
	glUniform3f(loc, x, y, z);
}

void ruya::Shader::setFloat(const std::string& uniformName, float value)
{
	unsigned int loc = glGetUniformLocation(mProgramID, uniformName.c_str());
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
GLuint ruya::Shader::createShader(GLenum shaderType, const std::string& shaderContent)
{
	GLuint shaderID;
	shaderID = glCreateShader(shaderType);
	const char* temp = shaderContent.c_str();
	glShaderSource(shaderID, 1, &temp, NULL);
	glCompileShader(shaderID);

	// error checking for shader compilation
	int success;
	char textBuffer[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, textBuffer);
		
		// compose error message and throw std::runtime_error
		std::string errorMsg = "";
		errorMsg = "[ruya::Shader::createShader()] ";
		if (shaderType == GL_VERTEX_SHADER) errorMsg += "Vertex ";
		else if (shaderType == GL_FRAGMENT_SHADER) errorMsg += "Fragment ";
		errorMsg += "shader compilation failed.\n";
		errorMsg += textBuffer;
		throw std::runtime_error(errorMsg);
	}

	return shaderID;
}



/*
* Creates a shader program, then attaches shaders created so far (with setShader()), then links program.
*	- Also deletes current program if there is one, and sets mProgramID to the newly created program.
* @throws std::runtime_error is linkage failed.
* @returns the program id of the created shader program.
*/
GLuint ruya::Shader::createShaderProgram()
{
	// link shaders into one shader program to be used for the render calls
	if (mProgramID > 0) glDeleteProgram(mProgramID);
	mProgramID = glCreateProgram();

	for (const GLuint shaderID : {mVertexShaderID, mFragmentShaderID, mGeometryShaderID})
	{
		if (shaderID > 0) glAttachShader(mProgramID, shaderID);
	}
	glLinkProgram(mProgramID);

	// errror checking for program linkage
	int success;
	char textBuffer[512];
	glGetProgramiv(mProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(mProgramID, 512, NULL, textBuffer);
		std::string errorMsg = "[ruya::Shader::createShaderProgram()] Shader program linkage failed.\n";
		errorMsg += textBuffer;
		errorMsg += "\n";
		throw std::runtime_error(errorMsg);
	}

	return mProgramID;
}
