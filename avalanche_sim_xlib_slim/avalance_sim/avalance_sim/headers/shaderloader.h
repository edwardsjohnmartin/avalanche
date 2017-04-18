#pragma once

#include "GL/glew.h"
#include "GL/freeglut.h"
#include <iostream>

namespace Core {

	class ShaderLoader {
	private:
		std::string readShader(char* filename); //Returns the contents of the specified shader file
		GLuint createShader(GLenum shaderType, std::string source, char* shaderName); //Creates and compiles a shader

	public:
		ShaderLoader(void); //Constructor
		~ShaderLoader(void); //Destructor
		GLuint createProgram(char* vertexShaderFilename, char* fragmentShaderFilename); //Creates a program using the provided shaders
	};
}