/**
* avalanche_sim.cpp
* .cpp file designed to run a simulation of an avalanche
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> //Provides access to lookAt, perspective, translate, etc.

#include <iostream>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

#include "../headers/shaderloader.h"
#include "../simulation/simulationcontroller.h"
#include "../simulation/simulationfactory.h"
#include "../simulation/massmovementsimulator.h"
#include "../xlib-slim/xlib.h"

using namespace std;
using namespace Core;

SimulationController simulationController;

GLuint terrainVertexBuffer;
GLuint terrainTextureBuffer;

GLuint particleVertexBuffer;
GLuint particleColorBuffer;

GLuint terrainVAO;
GLuint particleVAO;

GLuint particleProgram;
GLuint particleModelMatrixLocation;
GLuint particleViewMatrixLocation;
GLuint particleProjectionMatrixLocation;

GLuint terrainProgram;
GLuint terrainModelMatrixLocation;
GLuint terrainViewMatrixLocation;
GLuint terrainProjectionMatrixLocation;
GLuint terrainTexture;

std::map<std::string, std::vector<glm::vec3>> terrainData;

float frameRate;

glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

//Frame - Has colors
std::map<std::string, std::vector<glm::vec3>> currentFrame;

//Method designed to control frame rate
void timer(int) {
	//currentFrame = simulationController.getNextFrame();
	currentFrame = simulationController.getNextFrameFromGrid();

	glutPostRedisplay();
	glutTimerFunc(frameRate, timer, 0);
}

//Method designed to render the scene that needs drawn
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Tells OpenGL to clear its buffers
	glClearColor(0.69, 0.81, 1.0, 1.0); //Specifies the clear values for the color buffers

	glUseProgram(particleProgram);
	glBindVertexArray(particleVAO);

	glUniformMatrix4fv(particleModelMatrixLocation, 1, false, &modelMatrix[0][0]);
	glUniformMatrix4fv(particleViewMatrixLocation, 1, false, &viewMatrix[0][0]);
	glUniformMatrix4fv(particleProjectionMatrixLocation, 1, false, &projectionMatrix[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, particleVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentFrame["vertices"].size(), &currentFrame["vertices"][0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * currentFrame["colors"].size(), &currentFrame["colors"][0], GL_STATIC_DRAW);
	glDrawArrays(GL_POINTS, 0, currentFrame["colors"].size());

	glUseProgram(terrainProgram);
	glBindVertexArray(terrainVAO);

	glUniformMatrix4fv(terrainModelMatrixLocation, 1, false, &modelMatrix[0][0]);
	glUniformMatrix4fv(terrainViewMatrixLocation, 1, false, &viewMatrix[0][0]);
	glUniformMatrix4fv(terrainProjectionMatrixLocation, 1, false, &projectionMatrix[0][0]);

	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);
	glDrawArrays(GL_TRIANGLES, 0, terrainData["vertices"].size());

	glutSwapBuffers(); //Buffer swapping
}

//Method designed to handle the initialization code
void init() {
	//Init the simulation
	simulationController = SimulationController("simulationsettings.txt");
	terrainData = simulationController.getTerrainData();

	//TODO set frame rate using property get method instead of this
	frameRate = 1000.00 / simulationController.simulator.framesPerSecond;

	// Enable depth
	glEnable(GL_DEPTH_TEST);

	//load and compile shaders
	ShaderLoader shaderLoader;
	terrainProgram = shaderLoader.createProgram("shaders/TerrainVertexShader.glsl", "shaders/TerrainFragmentShader.glsl");
	particleProgram = shaderLoader.createProgram("shaders/ParticleVertexShader.glsl", "shaders/ParticleFragmentShader.glsl");
	glGenVertexArrays(1, &terrainVAO);
	glGenVertexArrays(1, &particleVAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


	glGenBuffers(1, &terrainVertexBuffer);
	glGenBuffers(1, &particleVertexBuffer);

	glBindVertexArray(terrainVAO);
	glBindBuffer(GL_ARRAY_BUFFER, terrainVertexBuffer);

	GLint terrainPosAttrib = glGetAttribLocation(terrainProgram, "position");
	glEnableVertexAttribArray(terrainPosAttrib);
	glVertexAttribPointer(terrainPosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainData["vertices"].size(), &terrainData["vertices"][0], GL_STATIC_DRAW);

	glBindVertexArray(particleVAO);
	glBindBuffer(GL_ARRAY_BUFFER, particleVertexBuffer);

	GLint particlePosAttrib = glGetAttribLocation(particleProgram, "position");
	glEnableVertexAttribArray(particlePosAttrib);
	glVertexAttribPointer(particlePosAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//Color buffer
	glGenBuffers(1, &particleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer);

	//Particle color
	GLint particleColAttrib = glGetAttribLocation(particleProgram, "color");
	glEnableVertexAttribArray(particleColAttrib);
	glVertexAttribPointer(particleColAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//Set up terrain texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &terrainTexture);

	glBindVertexArray(terrainVAO);
	glBindTexture(GL_TEXTURE_2D, terrainTexture);

	xlib::ximage tmpImage;
	tmpImage.importFrom_BMP(simulationController.simulator.pathFile); //TODO switch to use getter
	simulationController.simulator.terrain.terrainColor = simulationController.simulator.terrain.terrainColor * 0.5 + tmpImage * 0.5; //TODO switch to use getter

	glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		simulationController.simulator.terrain.terrainColor.width(), 
		simulationController.simulator.terrain.terrainColor.height(), 
		0, 
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		(GLvoid*)simulationController.simulator.terrain.terrainColor.getDataSource()
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(terrainProgram, "tex"), 0);

	glGenBuffers(1, &terrainTextureBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, terrainTextureBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainData["textureCoordinates"].size(), &terrainData["textureCoordinates"][0], GL_STATIC_DRAW);
	GLint terrainTexAttrib = glGetAttribLocation(terrainProgram, "uvCoord");
	glEnableVertexAttribArray(terrainTexAttrib);
	glVertexAttribPointer(terrainTexAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

	//Set up initial matrices
	modelMatrix = glm::scale(glm::mat4(), glm::vec3(0.01, 0.01, 0.01));
	modelMatrix = glm::rotate(modelMatrix, -0.5f, glm::vec3(0.0, 1.0, 0.0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-1000.0, -500.0, 0.0));
	viewMatrix = glm::lookAt(
		glm::vec3(0, 0.0, -20.0), //eye
		glm::vec3(0.0, 0, 0), //center
		glm::vec3(0, 1, 0) //up
	);

	projectionMatrix = glm::perspective(
		45.0f, //fov
		4.0f / 3.0f, //aspect 
		0.00001f, //nearZ
		1000000000.0f //farZ
	);

	terrainModelMatrixLocation = glGetUniformLocation(terrainProgram, "modelMatrix");
	terrainViewMatrixLocation = glGetUniformLocation(terrainProgram, "viewMatrix");
	terrainProjectionMatrixLocation = glGetUniformLocation(terrainProgram, "projectionMatrix");

	particleModelMatrixLocation = glGetUniformLocation(particleProgram, "modelMatrix");
	particleViewMatrixLocation = glGetUniformLocation(particleProgram, "viewMatrix");
	particleProjectionMatrixLocation = glGetUniformLocation(particleProgram, "projectionMatrix");

	//Set the current frame
	currentFrame = simulationController.getNextFrame();
}

//Method designed to start the application, this is the entry point
int main(int argc, char **argv) {
	glutInit(&argc, argv); //Inits the glut library
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); //Used to to determine the OpenGL display mode for the new window
	glutInitWindowPosition(250, 0);//Optional window position
	glutInitWindowSize(1000, 750); //Optional window size
	glutCreateWindow("Avalanche Simulator"); //Creates the actual window
	glewInit();

	//Call init method
	init();

	//Register callbacks
	glutDisplayFunc(renderScene);
	glutTimerFunc(frameRate, timer, 0); //Animation and Frame Rate
	glutMainLoop(); //Enters the glut main loop, only interrupted by callbacks
	glDeleteProgram(particleProgram);
	glDeleteProgram(terrainProgram);

	return 0;
}
