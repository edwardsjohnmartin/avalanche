#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <fstream>

#include "sdl.h"
#include <iostream>
#include "xlib.h"
#include <windows.h>
#include <process.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "../marchingsquares.h"
#include "../terrain/terrain.h"
#include "../videosettings.h"

#include "camera.h"
#include "input.h" //TODO look into this ones dependencies
#include "inputkeymapping.h"
#include "uicontrols.h"

#include "../core/particle.h"
#include "../core/simulationsettings.h"
#include "../core/particlebucket.h"
#include "../core/massmovementsimulation.h"

using namespace std;
using namespace xmath;
using namespace xutil;

//TODO rename me
void ___exportASCII_GRID(string filename, xarray<float> heightMap, float cellSize, float xCorner, float yCorner) {
	ofstream fout(filename.c_str());
	fout << "ncols			" << heightMap.size_x() << endl;
	fout << "nrows			" << heightMap.size_y() << endl;
	fout << "xllcorner		" << xCorner << endl;
	fout << "yllcorner		" << yCorner << endl;
	fout << "cellsize		" << cellSize << endl;
	fout << "NODATA_value	-9999" << endl;
	for (int i = 0; i < heightMap.size_x(); i++) {
		for (int j = 0; j < heightMap.size_y(); j++) {
			fout << heightMap(i, j) << " ";
		}
		fout << endl;
	}
}

struct SimulationView {

	MassMovementSimulation simulator;

	SimulationSettings	args;				//simulation parameters
	ximage			particleStart;			//input image for where the initial set of particles are created
	ximage			pathImage;				//input image for the actual flow path (used for training)
	ximage			pathDistanceMap;		//input image that applies distance transform on the pathImage
	ximage			forceMap;				//output image that accumulates the motion of the particles
	Terrain			terrain;				//terrain map generated from the input DEM data

	//window display/input stuff TODO extract out
	SDL_Surface*	screen;
	int				viewWidth;
	int				viewHeight;
	Camera			cam;
	Input			input;
	InputKeyMapping keyMappings;
	UIPanel*		gui;
	unsigned int	terrainTex;
	unsigned int	particleTex;
	float			mouseX;
	float			mouseY;
	float			viewRotation;
	float			deltaMouseX;
	float			deltaMouseY;
	vec3			look;
	ofstream		fout;
	VideoSettings	VideoSettings;

	//gui stuff
	int				enableStereoChkBox;
	int				reverseStereoChkBox;
	int				stereoEffectScaleSlider;
	int				stereoEffectScaleCaption;
	int				stereoFocalDistanceSlider;
	int				stereoFocalDistanceCaption;

	int				initialHeightSlider;
	int				bounceFrictionSlider;
	int				stickynessSlider;
	int				dampingForceSlider;
	int				turbulanceForceSlider;
	int				clumpingFactorSlider;
	int				viscositySlider;
	int				gridSizeSlider;

	int				initialHeightCaption;
	int				bounceFrictionCaption;
	int				stickynessCaption;
	int				dampingForceCaption;
	int				turbulanceForceCaption;
	int				clumpingFactorCaption;
	int				viscosityCaption;
	int				gridSizeCaption;

	int				resetBtn;
	int				saveFMapBtn;
	int				saveSettingsBtn;

	void init(int enableDisplay = -1) {
		//initializes the simulator
		//be use to have the simulation settings loaded before calling this
		//if enableDisplay is left at -1 it uses the file settings
		//else if true (1) it enables the viewport
		//else if false (0) it disables the viewport

		args.parseSettings("simulationsettings.txt");

		VideoSettings.parseSettings("videosettings.txt");
		terrain.loadFrom_DEM_ASCII(args.elevationDEMFile);
		if (enableDisplay != -1) {
			VideoSettings.enableDisplay = enableDisplay;
		}
		if (VideoSettings.enableDisplay) {
			terrain.exportNormalMap("normalmap.bmp");
			terrain.terrainColor.importFrom_BMP(args.terrainColorFile);
			terrain.terrainColor = terrain.terrainColor.resizedTo(512, 512);
			viewWidth = VideoSettings.resX;
			viewHeight = VideoSettings.resY;
			initOpenGL();
			initUI();
		}
		if (args.pathFile != "") {
			if (args.verboseOutput) cout << "Loading input path file: " << args.pathFile << endl;
			pathImage.importFrom_BMP(args.pathFile);
		}

		simulator.init(args, terrain);
		simulator.initParticles();
	}

	void initUI() {
		//initializes all the GUI elements
		//todo: clean up the mess from all these new statements to prevent memory leaks
		if (args.verboseOutput) {
			cout << "Initializing GUI elements" << endl;
		}

		//Create the Panel Window
		gui = new UIPanel();
		gui->position = vec3(viewWidth - 300, viewHeight - 60, 0);
		gui->setSize(gui->width, 520);

		//Create a window text captions
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 80, 0);
		((UICaption*)gui->childControls.back())->setCaption("3D Settings");

		//create sliders
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 120, 0);
		((UICaption*)gui->childControls.back())->setCaption("Stereo Offset");

		//stereo effect slider
		stereoEffectScaleSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 120, 0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 100.0;
		((UISlider*)gui->childControls.back())->setValue(VideoSettings.stereoOffset);

		//focal distance slider
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 100, 0);
		((UICaption*)gui->childControls.back())->setCaption("Focal Distance");

		stereoFocalDistanceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 100, 0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 10000.0;
		((UISlider*)gui->childControls.back())->sliderMinValue = 100.0;
		((UISlider*)gui->childControls.back())->setValue(VideoSettings.stereoFocalDistance);

		stereoEffectScaleCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 120, 0);

		stereoFocalDistanceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 100, 0);

		//checkboxes
		enableStereoChkBox = gui->childControls.size();
		gui->childControls.push_back(new UICheckBox());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 140, 0);
		((UICheckBox*)gui->childControls.back())->setChecked(VideoSettings.stereoEnable);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 265, viewHeight - 140, 0);
		((UICaption*)gui->childControls.back())->setCaption("Enable Stereo");

		reverseStereoChkBox = gui->childControls.size();
		gui->childControls.push_back(new UICheckBox());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 160, 0);
		((UICheckBox*)gui->childControls.back())->setChecked(VideoSettings.stereoReverse);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 265, viewHeight - 160, 0);
		((UICaption*)gui->childControls.back())->setCaption("Reverse Stereo");

		//simulation setting sliders
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 180, 0);
		((UICaption*)gui->childControls.back())->setCaption("Simulation Settings");

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 200, 0);
		((UICaption*)gui->childControls.back())->setCaption("initialHeight");

		initialHeightCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 200, 0);

		initialHeightSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 200, 0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 500.0;
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.0;
		((UISlider*)gui->childControls.back())->setValue(args.initialHeight);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 220, 0);
		((UICaption*)gui->childControls.back())->setCaption("bounceFriction");

		bounceFrictionCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 220, 0);

		bounceFrictionSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 220, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.bounceFriction);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 240, 0);
		((UICaption*)gui->childControls.back())->setCaption("stickyness");

		stickynessCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 240, 0);

		stickynessSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 240, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 5.00;
		((UISlider*)gui->childControls.back())->setValue(args.stickyness);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 260, 0);
		((UICaption*)gui->childControls.back())->setCaption("dampingForce");

		dampingForceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 260, 0);

		dampingForceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 260, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.dampingForce);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 280, 0);
		((UICaption*)gui->childControls.back())->setCaption("turbulanceForce");

		turbulanceForceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 280, 0);

		turbulanceForceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 280, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 5.00;
		((UISlider*)gui->childControls.back())->setValue(args.turbulanceForce);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 300, 0);
		((UICaption*)gui->childControls.back())->setCaption("clumpingFactor");

		clumpingFactorCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 300, 0);

		clumpingFactorSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 300, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.clumpingFactor);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 320, 0);
		((UICaption*)gui->childControls.back())->setCaption("viscosity");

		viscosityCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 320, 0);

		viscositySlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 320, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.viscosity);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 340, 0);
		((UICaption*)gui->childControls.back())->setCaption("gridSize");

		gridSizeCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100, viewHeight - 340, 0);

		gridSizeSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 340, 0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 32.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 512.00;
		((UISlider*)gui->childControls.back())->setValue(args.gridSize);

		//create buttons
		resetBtn = gui->childControls.size();
		gui->childControls.push_back(new UIButton());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 500, 0);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270, viewHeight - 500, 0);
		((UICaption*)gui->childControls.back())->setCaption("Reset");
		((UICaption*)gui->childControls.back())->setColor(vec4(0, 0, 0, 1));

		saveFMapBtn = gui->childControls.size();
		gui->childControls.push_back(new UIButton());
		gui->childControls.back()->position = vec3(viewWidth - 290, viewHeight - 520, 0);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 280, viewHeight - 520, 0);
		((UICaption*)gui->childControls.back())->setCaption("Save Path");
		((UICaption*)gui->childControls.back())->setColor(vec4(0, 0, 0, 1));
	}

	void initOpenGL() {
		//initialize the opengl context within SDL
		if (args.disableView) return;
		if (args.verboseOutput) {
			cout << "Initializing OpenGL Viewport" << endl;
		}
		SDL_Init(SDL_INIT_VIDEO);
		screen = SDL_SetVideoMode(viewWidth, viewHeight, 32, SDL_OPENGL | (VideoSettings.fullscreen ? SDL_FULLSCREEN : 0));
		SDL_ShowCursor(0);
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, viewWidth, viewHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		cam.setPosition(vec3(4000, 500, 2000));
		cam.lookAt(vec3(1000, 500, 1000));
		gluPerspective(cam.fov, float(viewWidth) / viewHeight, cam.nearZ, cam.farZ);
		glMatrixMode(GL_MODELVIEW);

		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &terrainTex);
		glBindTexture(GL_TEXTURE_2D, terrainTex);
		if (args.pathFile != "") {
			ximage tmpImage;
			tmpImage.importFrom_BMP(args.pathFile);
			terrain.terrainColor = terrain.terrainColor * 0.5 + tmpImage * 0.5;
		}

		//Set background color
		glClearColor(0.72f, 0.82f, 1.0f, 1.0f);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain.terrainColor.width(), terrain.terrainColor.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain.terrainColor.getDataSource());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		ximage particleTexImg;
		particleTexImg.importFrom_BMP("resources/sprite.bmp");
		glGenTextures(1, &particleTex);
		glBindTexture(GL_TEXTURE_2D, particleTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, particleTexImg.width(), particleTexImg.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, particleTexImg.getDataSource());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	void handleEvents() {
		SDL_Event evnt;
		static bool skip = false;
		while (SDL_PollEvent(&evnt)) {
			switch (evnt.type) {
			case SDL_MOUSEBUTTONDOWN:
				input.mouseDownEvent(evnt.button.button);
				gui->mouseDown(evnt.button.x, viewHeight - evnt.button.y);
				break;
			case SDL_MOUSEBUTTONUP:
				input.mouseUpEvent(evnt.button.button);
				gui->mouseUp(evnt.button.x, viewHeight - evnt.button.y);
				break;
			case SDL_MOUSEMOTION:
				gui->updateMouse(evnt.motion.x, viewHeight - evnt.motion.y);
				input.mouseX = evnt.motion.x;
				input.mouseY = viewHeight - evnt.motion.y;
				break;
			case SDL_KEYUP:
				input.keyUpEvent(evnt.key.keysym.sym);
				if (evnt.key.keysym.sym == 27) {
					quit();
				}
				break;
			case SDL_KEYDOWN:
				input.keyDownEvent(evnt.key.keysym.sym);
				break;

			case SDL_QUIT:
				quit();
				break;
			}

		}
		skip = !skip;
	}

	//TODO extract out
	void handleInput() {
		if (input.keysDown[keyMappings.moveForwardKey]) {
			cam.setPosition(cam.getPosition() - cam.getForwardAxis() * 10);
		}
		else if (input.keysDown[keyMappings.moveBackwardKey]) {
			cam.setPosition(cam.getPosition() + cam.getForwardAxis() * 10);
		}
		if (input.keysDown[keyMappings.moveLeftKey]) {
			cam.setPosition(cam.getPosition() - cam.getRightAxis() * 10);
		}
		else if (input.keysDown[keyMappings.moveRightKey]) {
			cam.setPosition(cam.getPosition() + cam.getRightAxis() * 10);
		}
		if (input.keysDown[keyMappings.moveUpKey]) {
			cam.setPosition(cam.getPosition() + vec3(0, 10, 0));
		}
		else if (input.keysDown[keyMappings.moveDownKey]) {
			cam.setPosition(cam.getPosition() - vec3(0, 10, 0));
		}

		if (input.keysDown[keyMappings.rotateLeftKey]) {
			mat4 rot;
			rot.makeRotation(0.05, vec3(0, 1, 0));
			cam.transform = rot*(cam.transform & mat4(1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1)) + (cam.transform & mat4(0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0));
		}
		else if (input.keysDown[keyMappings.rotateRightKey]) {
			mat4 rot;
			rot.makeRotation(-0.05, vec3(0, 1, 0));
			cam.transform = rot*(cam.transform & mat4(1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1)) + (cam.transform & mat4(0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0));
		}
		if (input.keysDown[keyMappings.rotateUpKey]) {
			mat4 rot;
			rot.makeRotation(-0.05, cam.getRightAxis());
			cam.transform = rot*(cam.transform & mat4(1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1)) + (cam.transform & mat4(0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0));
		}
		else if (input.keysDown[keyMappings.rotateDownKey]) {
			mat4 rot;
			rot.makeRotation(0.05, cam.getRightAxis());
			cam.transform = rot*(cam.transform & mat4(1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1)) + (cam.transform & mat4(0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0));
		}
	}

	//TODO extract out
	void updateUI() {
		gui->updateMouse(input.mouseX, input.mouseY);

		VideoSettings.stereoOffset = ((UISlider*)gui->childControls[stereoEffectScaleSlider])->sliderValue;
		VideoSettings.stereoEnable = ((UICheckBox*)gui->childControls[enableStereoChkBox])->checked;
		VideoSettings.stereoReverse = ((UICheckBox*)gui->childControls[reverseStereoChkBox])->checked;
		VideoSettings.stereoFocalDistance = ((UISlider*)(gui->childControls[stereoFocalDistanceSlider]))->sliderValue;

		((UICaption*)(gui->childControls[stereoEffectScaleCaption]))->setCaption(toString(((UISlider*)(gui->childControls[stereoEffectScaleSlider]))->sliderValue));
		((UICaption*)(gui->childControls[stereoFocalDistanceCaption]))->setCaption(toString(((UISlider*)(gui->childControls[stereoFocalDistanceSlider]))->sliderValue));

		args.initialHeight = ((UISlider*)(gui->childControls[initialHeightSlider]))->sliderValue;
		args.bounceFriction = ((UISlider*)(gui->childControls[bounceFrictionSlider]))->sliderValue;
		args.stickyness = ((UISlider*)(gui->childControls[stickynessSlider]))->sliderValue;
		args.dampingForce = ((UISlider*)(gui->childControls[dampingForceSlider]))->sliderValue;
		args.turbulanceForce = ((UISlider*)(gui->childControls[turbulanceForceSlider]))->sliderValue;
		args.clumpingFactor = ((UISlider*)(gui->childControls[clumpingFactorSlider]))->sliderValue;
		args.viscosity = ((UISlider*)(gui->childControls[viscositySlider]))->sliderValue;

		((UICaption*)(gui->childControls[initialHeightCaption]))->setCaption(toString(((UISlider*)(gui->childControls[initialHeightSlider]))->sliderValue));
		((UICaption*)(gui->childControls[bounceFrictionCaption]))->setCaption(toString(((UISlider*)(gui->childControls[bounceFrictionSlider]))->sliderValue));
		((UICaption*)(gui->childControls[stickynessCaption]))->setCaption(toString(((UISlider*)(gui->childControls[stickynessSlider]))->sliderValue));
		((UICaption*)(gui->childControls[dampingForceCaption]))->setCaption(toString(((UISlider*)(gui->childControls[dampingForceSlider]))->sliderValue));
		((UICaption*)(gui->childControls[turbulanceForceCaption]))->setCaption(toString(((UISlider*)(gui->childControls[turbulanceForceSlider]))->sliderValue));
		((UICaption*)(gui->childControls[clumpingFactorCaption]))->setCaption(toString(((UISlider*)(gui->childControls[clumpingFactorSlider]))->sliderValue));
		((UICaption*)(gui->childControls[viscosityCaption]))->setCaption(toString(((UISlider*)(gui->childControls[viscositySlider]))->sliderValue));

		((UICaption*)(gui->childControls[gridSizeCaption]))->setCaption(toString(((UISlider*)(gui->childControls[gridSizeSlider]))->sliderValue));

		if (((UIButton*)(gui->childControls[resetBtn]))->pollPushed()) {
			simulator.initParticles(); //------------------------------------------------------------------------------------------------------------------------------
		}

		if (((UIButton*)(gui->childControls[saveFMapBtn]))->pollPushed()) {
			exportForceMapAsDEM(args.flowPathOutputFile + ".dem");
			forceMap.exportAs_BMP(args.flowPathOutputFile + ".bmp");
		}

		if (((UISlider*)(gui->childControls[gridSizeSlider]))->pollChanged()) {
			args.gridSize = (int)((UISlider*)(gui->childControls[gridSizeSlider]))->sliderValue;
			simulator.resetGrid(); //---------------------------------------------------------------------------------------------------------------------------------
		}
	}

	void mainLoop() {
		int itercount = 0;
		if (args.verboseOutput) {
			cout << "Starting Simulation" << endl;
		}
		while (true) {

			if (VideoSettings.enableDisplay) {
				handleEvents();
				handleInput();
				updateUI();
			}

			simulator.updateAllParticles(); //-------------------------------------------------------------------------------------------------------------------------

			if (VideoSettings.enableDisplay) {
				if (VideoSettings.stereoEnable) {
					render(1);
					render(2);
				}
				else {
					render(0);
				}
				SDL_GL_SwapBuffers(); //TODO SDL call
			}
			itercount++;

			if (itercount >= args.maxIterations && !VideoSettings.enableDisplay) {
				break;
			}
		}

		if (VideoSettings.enableDisplay) {
			quit();
		}
		else if (args.flowPathOutputFile != "") {
			forceMap.exportAs_BMP(args.flowPathOutputFile + ".bmp");
			exportForceMapAsDEM(args.flowPathOutputFile + ".dem");
		}
	}

	void exportForceMapAsDEM(string filename) {
		if (args.verboseOutput) cout << "Generating ASCII Grid File." << endl;
		xarray<float> fmap(terrain.heightMap.size_y(), terrain.heightMap.size_x());
		for (int i = 0; i < fmap.size_x(); i++) {
			for (int j = 0; j < fmap.size_y(); j++) {
				float val = forceMap.getPixelLerp(float(j) / fmap.size_y(), float(fmap.size_x() - i - 1) / fmap.size_x()).x;
				if (val > 0.0) {
					fmap(i, j) = val;
				}
				else {
					fmap(i, j) = -9999.0;
				}
			}
		}
		___exportASCII_GRID(filename, fmap, terrain.cellSize, terrain.xCorner, terrain.yCorner);
		if (args.verboseOutput) {
			cout << "Exported flow path to ACSII GRID file as " << filename << endl;
		}
	}

	void quit() {
		fout.close();
		exit(0);
	}

	void render(int renderIndex = 0) {
		static float theta = 0;
		float r = terrain.cellSize * (terrain.heightMap.size_x() + terrain.heightMap.size_y()) / 2;
		vec3 lookDir;

		theta += 0.02;

		/* Clear the color and depth buffers. */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		vec3 camStereoOffset(0, 0, 0);
		switch (renderIndex) {
		case 0:
			glViewport(0, 0, viewWidth, viewHeight);
			break;
		case 1:
			camStereoOffset = cam.getRightAxis() * VideoSettings.stereoOffset * (VideoSettings.stereoReverse ? -1 : 1);
			glViewport(0, 0, viewWidth / 2, viewHeight);
			break;
		case 2:
			camStereoOffset = cam.getRightAxis() * VideoSettings.stereoOffset * (VideoSettings.stereoReverse ? 1 : -1);
			glViewport(viewWidth / 2, 0, viewWidth / 2, viewHeight);
			break;
		}

		gluPerspective(cam.fov, float(viewWidth) / viewHeight, cam.nearZ, cam.farZ);
		glMatrixMode(GL_MODELVIEW);
		vec3 focalPoint;
		focalPoint = -cam.getForwardAxis()*VideoSettings.stereoFocalDistance + cam.getPosition();
		Camera oldCam = cam;
		cam.move(camStereoOffset);
		cam.lookAt(focalPoint);

		glLoadMatrixf(cam.transform.inverse().ptr);
		cam = oldCam;

		glColor3fv(VideoSettings.terrainColor.ptr);
		glScalef(1.0, VideoSettings.heightScaling, 1.0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(TerrainVertex), &terrain.verts[0]);
		glTexCoordPointer(3, GL_FLOAT, sizeof(TerrainVertex), &terrain.verts[0].texcoords.x);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, terrainTex);
		glDrawElements(GL_QUADS, 4 * terrain.quads.size(), GL_UNSIGNED_INT, &terrain.quads[0]);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glBindTexture(GL_TEXTURE_2D, 0 * particleTex);
		glPointSize(VideoSettings.particleSize);

		mat4 view;
		glGetFloatv(GL_MODELVIEW_MATRIX, view[0]);
		view.invertAffine();

		glBegin(GL_POINTS);
		for (int i = 0; i < simulator.particles.size(); i++) { //--------------------------------------------------------------------------------------------------------
			vec3 pos = simulator.particles[i].position;
			vec3 color;
			if (VideoSettings.showDensity) {
				float alpha = 0.0001 * simulator.computeDensity(simulator.particles[i].position.x, simulator.particles[i].position.z);
				alpha = (6.0 - alpha) / 6.0;
				alpha = fclamp(alpha, 0, 1.0);
				alpha = pow(alpha, 2.0f);
				alpha *= 5.999;
				int alphai = (int)alpha;
				float w = alpha - alphai;

				color = VideoSettings.particleDensityColor[alphai] * (1.0 - w) + VideoSettings.particleDensityColor[clamp(alphai + 1, 0, 5)] * (w);
			}
			else {
				color = VideoSettings.particleColor;
			}
			glColor3fv(color.ptr);
			glVertex3fv(pos.ptr);
		}
		glEnd();

		renderUIPanelOpenGL(gui, viewWidth, viewHeight);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}
};

#endif