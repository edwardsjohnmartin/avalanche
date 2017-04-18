#include "../headers/shaderloader.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace Core;

ShaderLoader::ShaderLoader(void){}
ShaderLoader::~ShaderLoader(void){}

//Method designed to read and return the shader file contents
std::string ShaderLoader::readShader(char* filename) {
	std::string shaderCode;
	std::ifstream file(filename, std::ios::in);

	//Check if the file is bad
	if (!file.good()) {
		std::cout << "Unable to read file " << std::endl;
		std::terminate();
	}

	//Resize the shaderCode string to the size of the file
	file.seekg(0, std::ios::end);
	shaderCode.resize((unsigned int)file.tellg());

	//Read the file into the shaderCode string
	file.seekg(0, std::ios::beg);
	file.read(&shaderCode[0], shaderCode.size());

	//Close the file
	file.close();

	//Return the shader
	return shaderCode;
}


//Method designed to create and compile the shader for return
GLuint ShaderLoader::createShader(GLenum shaderType, std::string source, char* shaderName) {
	int compileResult = 0;

	GLuint shader = glCreateShader(shaderType); //Creates an empty shader object of the given type
	const char* shaderCodePointer = source.c_str();
	const int shaderCodeSize = source.size();

	glShaderSource(shader, 1, &shaderCodePointer, &shaderCodeSize); //Loads the shader object with the given code, reads code from the array until it reaches null
	glCompileShader(shader); //Compiles the shader code
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileResult); //Check for errors

	//Check for errors
	if (compileResult == GL_FALSE) {
		int infoLogLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> shaderLog(infoLogLength);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderLog[0]);
		std::cout << "ERROR compiling shader: " << shaderName << std::endl << &shaderLog[0] << std::endl;
		return 0;
	}
	return shader;
}

//Method designed to return a program from the given shaders
GLuint ShaderLoader::createProgram(char* vertexShaderFilename, char* fragmentShaderFilename) {
	//Read the shader files and save the code
	std::string vertexShaderCode = readShader(vertexShaderFilename);
	std::string fragmentShaderCode = readShader(fragmentShaderFilename);

	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderCode, "vertex shader");
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderCode, "fragment shader");

	int linkResult = 0;

	//Create the program handle, attach the shaders and link it
	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &linkResult);

	//Check for link errors
	if (linkResult == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		std::vector<char> programLog(infoLogLength);
		glGetProgramInfoLog(program, infoLogLength, NULL, &programLog[0]);
		std::cout << "Shader Loader : LINK ERROR" << std::endl << &programLog[0] << std::endl;
		return 0;
	}
	return program;
}