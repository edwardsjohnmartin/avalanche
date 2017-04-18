#ifndef AVALANCHESIM_H
#define AVALANCHESIM_H

#include <fstream>

#include "sdl.h"
#include <iostream>
#include "xlib.h"
#include <windows.h>
#include <process.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "marchingsquares.h"
#include "terrain/terrain.h"
#include "videosettings.h"

#include "view/camera.h"
#include "view/input.h" //TODO look into this ones dependencies
#include "view/inputkeymapping.h"
#include "view/uicontrols.h"

#include "core/particle.h"
#include "core/simulationsettings.h"
#include "core/particlebucket.h"

const int numThreads = 8;

using namespace std;
using namespace xmath;
using namespace xutil;

void exportASCII_GRID(string filename, xarray<float> heightMap, float cellSize, float xCorner, float yCorner) {
	ofstream fout(filename.c_str());
	fout << "ncols			" << heightMap.size_x() << endl;   
	fout << "nrows			" << heightMap.size_y() << endl;
    fout << "xllcorner		" << xCorner << endl;
	fout << "yllcorner		" << yCorner << endl;
	fout << "cellsize		" << cellSize << endl;
	fout << "NODATA_value	-9999" << endl;
	for (int i = 0; i < heightMap.size_x(); i++) {
		for (int j = 0; j < heightMap.size_y(); j++) {
			fout << heightMap(i,j) << " ";
		}
		fout << endl;
	}
}

struct MassMovementSimulator {
	//Main class for the the mass movement simulation
	//includes the particle simulator as well as the display window

	//simulation stuff
	SimulationSettings	args;				//simulation parameters
	xarray<Particle> particles;				//the actual particles themselves
	xarray<ParticleBucket>	particleGrid;	//2d grid used for fluid dynamics calculations
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
	
	

	struct MTArgs {
		//struct use for multithreading
		MassMovementSimulator* world;
		int threadIndex;
		int start;
		int end;
		bool complete;
		MTArgs() {
			world = NULL;
			threadIndex = 0;
			complete = false;
		}
	};

	void init(int enableDisplay = -1) {
		//initializes the simulator
		//be use to have the simulation settings loaded before calling this
		//if enableDisplay is left at -1 it uses the file settings
		//else if true (1) it enables the viewport
		//else if false (0) it disables the viewport

		VideoSettings.parseSettings("videosettings.txt");
		terrain.loadFrom_DEM_ASCII(args.elevationDEMFile);
		if (enableDisplay != -1) {
			VideoSettings.enableDisplay = enableDisplay;
		}
		if (VideoSettings.enableDisplay) {
			terrain.exportNormalMap("normalmap.bmp");
			terrain.terrainColor.importFrom_BMP(args.terrainColorFile);
			terrain.terrainColor = terrain.terrainColor.resizedTo(512,512);
			viewWidth = VideoSettings.resX;
			viewHeight = VideoSettings.resY;
			initOpenGL();
			initUI();
		}
		if (args.pathFile != "") {
			if (args.verboseOutput) cout << "Loading input path file: " << args.pathFile << endl;
			pathImage.importFrom_BMP(args.pathFile);
		}
		initParticles();
	}

	//TODO extract out
	void initUI() {
		//initializes all the GUI elements
		//todo: clean up the mess from all these new statements to prevent memory leaks
		if (args.verboseOutput) {
			cout << "Initializing GUI elements" << endl;
		}

		//Create the Panel Window
		gui = new UIPanel();
		gui->position = vec3(viewWidth-300,viewHeight-60,0);
		gui->setSize(gui->width,520);

		//Create a window text captions
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 80,0);
		((UICaption*)gui->childControls.back())->setCaption("3D Settings");

		//create sliders
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 120,0);
		((UICaption*)gui->childControls.back())->setCaption("Stereo Offset");

		//stereo effect slider
		stereoEffectScaleSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 120,0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 100.0;
		((UISlider*)gui->childControls.back())->setValue(VideoSettings.stereoOffset);

		//focal distance slider
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 100,0);
		((UICaption*)gui->childControls.back())->setCaption("Focal Distance");

		stereoFocalDistanceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 100,0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 10000.0;
		((UISlider*)gui->childControls.back())->sliderMinValue = 100.0;
		((UISlider*)gui->childControls.back())->setValue(VideoSettings.stereoFocalDistance);

		stereoEffectScaleCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 120,0);

		stereoFocalDistanceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 100,0);

		//checkboxes
		enableStereoChkBox = gui->childControls.size();
		gui->childControls.push_back(new UICheckBox());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 140,0);
		((UICheckBox*)gui->childControls.back())->setChecked(VideoSettings.stereoEnable);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 265,viewHeight - 140,0);
		((UICaption*)gui->childControls.back())->setCaption("Enable Stereo");

		reverseStereoChkBox = gui->childControls.size();
		gui->childControls.push_back(new UICheckBox());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 160,0);
		((UICheckBox*)gui->childControls.back())->setChecked(VideoSettings.stereoReverse);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 265,viewHeight - 160,0);
		((UICaption*)gui->childControls.back())->setCaption("Reverse Stereo");

		//simulation setting sliders
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 180,0);
		((UICaption*)gui->childControls.back())->setCaption("Simulation Settings");

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 200,0);
		((UICaption*)gui->childControls.back())->setCaption("initialHeight");

		initialHeightCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 200,0);

		initialHeightSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 200,0);
		((UISlider*)gui->childControls.back())->sliderMaxValue = 500.0;
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.0;
		((UISlider*)gui->childControls.back())->setValue(args.initialHeight);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 220,0);
		((UICaption*)gui->childControls.back())->setCaption("bounceFriction");

		bounceFrictionCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 220,0);

		bounceFrictionSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 220,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.bounceFriction);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 240,0);
		((UICaption*)gui->childControls.back())->setCaption("stickyness");

		stickynessCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 240,0);

		stickynessSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 240,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 5.00;
		((UISlider*)gui->childControls.back())->setValue(args.stickyness);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 260,0);
		((UICaption*)gui->childControls.back())->setCaption("dampingForce");

		dampingForceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 260,0);

		dampingForceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 260,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.dampingForce);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 280,0);
		((UICaption*)gui->childControls.back())->setCaption("turbulanceForce");

		turbulanceForceCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 280,0);

		turbulanceForceSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 280,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 5.00;
		((UISlider*)gui->childControls.back())->setValue(args.turbulanceForce);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 300,0);
		((UICaption*)gui->childControls.back())->setCaption("clumpingFactor");

		clumpingFactorCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 300,0);

		clumpingFactorSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 300,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.clumpingFactor);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 320,0);
		((UICaption*)gui->childControls.back())->setCaption("viscosity");

		viscosityCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 320,0);

		viscositySlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 320,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 0.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 1.00;
		((UISlider*)gui->childControls.back())->setValue(args.viscosity);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 340,0);
		((UICaption*)gui->childControls.back())->setCaption("gridSize");

		gridSizeCaption = gui->childControls.size();
		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 100,viewHeight - 340,0);

		gridSizeSlider = gui->childControls.size();
		gui->childControls.push_back(new UISlider());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 340,0);
		((UISlider*)gui->childControls.back())->sliderMinValue = 32.00;
		((UISlider*)gui->childControls.back())->sliderMaxValue = 512.00;
		((UISlider*)gui->childControls.back())->setValue(args.gridSize);

		//create buttons
		resetBtn = gui->childControls.size();
		gui->childControls.push_back(new UIButton());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 500,0);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 270,viewHeight - 500,0);
		((UICaption*)gui->childControls.back())->setCaption("Reset");
		((UICaption*)gui->childControls.back())->setColor(vec4(0,0,0,1));

		saveFMapBtn = gui->childControls.size();
		gui->childControls.push_back(new UIButton());
		gui->childControls.back()->position = vec3(viewWidth - 290,viewHeight - 520,0);

		gui->childControls.push_back(new UICaption());
		gui->childControls.back()->position = vec3(viewWidth - 280,viewHeight - 520,0);
		((UICaption*)gui->childControls.back())->setCaption("Save Path");
		((UICaption*)gui->childControls.back())->setColor(vec4(0,0,0,1));
	}

	//TODO extract out
	void initOpenGL() {
		//initialize the opengl context within SDL
		
		if (args.disableView) return;
		if (args.verboseOutput) {
			cout << "Initializing OpenGL Viewport" << endl;
		}
		SDL_Init(SDL_INIT_VIDEO);
		screen = SDL_SetVideoMode(viewWidth, viewHeight, 32, SDL_OPENGL  | (VideoSettings.fullscreen ? SDL_FULLSCREEN : 0));
		SDL_ShowCursor(0);
		//glewInit();
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

		glClearColor(0.72f, 0.82f, 1.0f, 1.0f);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain.terrainColor.width(), terrain.terrainColor.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain.terrainColor.getDataSource());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		ximage particleTexImg;
		particleTexImg.importFrom_BMP("resources/sprite.bmp");
		glGenTextures(1,&particleTex);
		glBindTexture(GL_TEXTURE_2D,particleTex);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,particleTexImg.width(), particleTexImg.height(),0,GL_RGB,GL_UNSIGNED_BYTE,particleTexImg.getDataSource());
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	}

	void initParticles() {
		//create all the particles for the simulator
		cout << "Initializing Particles" << endl;
		forceMap.fill(vec4(0,0,0,0));
		particleStart.importFrom_BMP(args.startingZoneFile);
		
		//particleStart = particleStart.convertedTo(XIMAGE_FORMAT_RGB24);
		forceMap = ximage(particleStart.width(),particleStart.height(),1,XIMAGE_FORMAT_GRAYSCALE_FLOAT32);
		forceMap.fill(vec4(0,0,0,0));
		int numParticles = 0;
		float initialHeight = args.initialHeight;
	
		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i,j).xyz().lengthSqr();
				numParticles += nParticles;
			}
		}
		particles = xarray<Particle>(numParticles);
		int index = 0;
		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i,j).xyz().lengthSqr();
				for (int p = 0; p < nParticles; p++) {

					vec3 randval(0,0,0);
					for (int r = 0; r < 4; r++) {
						randval.x += (frand() - 0.5)/2.0;
						randval.y += (frand() - 0.5)/2.0;
						randval.z += (frand() - 0.5)/2.0;
					}
					
					int x = terrain.heightMap.size_y() * i / particleStart.width();
					int y = terrain.heightMap.size_x() - terrain.heightMap.size_x() * j / particleStart.height();
					particles[index].position = vec3(randval.x * terrain.cellSize + x * terrain.cellSize, terrain.heightMap(y, x) + initialHeight, randval.z * terrain.cellSize + y * terrain.cellSize);
					particles[index].velocity = vec3(0,0,0);
					index++;
				}
			}
		}
		resetGrid();
	}

	static void updateParticlesMT(void* arg) {
		//multithreaded call for updating each particle
		//do not use, this randomly crashes for some reason
		//I think it has to do with accessing the std::vector class from multiple threads

		MTArgs* mtArgs = (MTArgs*)arg;
		int tIndex = mtArgs->threadIndex;
		int count = (mtArgs->world->particles.size()/numThreads)-1;
		int start = mtArgs->start;
		int end = mtArgs->end;
		for (int i = start; i < end; i++) {
			mtArgs->world->updateParticle(i);
		}
		mtArgs->complete = true;
	}

	void clearGrid() {
		//clear out all the particles from the grid
		for (int i = 0; i < particleGrid.size_x(); i++) {
			for (int j = 0; j < particleGrid.size_y(); j++) {
				particleGrid(i,j).particles.clear();
			}
		}
	}

	void resetGrid() {
		//resets all particle grid with a new size
		particleGrid = xarray<ParticleBucket>(int(double(args.gridSize) * terrain.heightMap.size_x() / 512.0), int(double(args.gridSize) * terrain.heightMap.size_y() / 512.0));
	}

	void registerParticleToGrid(int i) {
		//takes a particle and puts in which grid cell it belongs to
		float x, y;
		int ix, iy;
		float ex, ey;
		int x1, y1;
		x = particles[i].position.x * particleGrid.size_y() / (terrain.heightMap.size_y() * terrain.cellSize);
		y = particles[i].position.z * particleGrid.size_x() / (terrain.heightMap.size_x() * terrain.cellSize);
		
		ix = x;
		iy = y;
		ex = x - ix;
		ey = y - iy;

		if (ex >= 0.5) {
			x1 = ix + 1;
		}
		else {
			x1 = ix - 1;
		}
		if (ey >= 0.5) {
			y1 = iy + 1;
		}
		else {
			y1 = iy - 1;
		}

		ix = clamp(ix, 0, (int)particleGrid.size_y() - 1);
		iy = clamp(iy, 0, (int)particleGrid.size_x() - 1);

		x1 = clamp(x1, 0, (int)particleGrid.size_y() - 1);
		y1 = clamp(y1, 0, (int)particleGrid.size_x() - 1);

		particleGrid(iy, ix).particles.push_back(i);
		particleGrid(iy, x1).particles.push_back(i);
		particleGrid(y1, x1).particles.push_back(i);
		particleGrid(y1, ix).particles.push_back(i);

		//x = clamp((int)x,0,31);
		//y = clamp(y,0,31);
	}

	float computeDensity(float x, float z) {
		//computes the particle density giving a point location's x and z coords
		x = x * particleGrid.size_y() / (terrain.heightMap.size_y() * terrain.cellSize);
		z = z * particleGrid.size_x() / (terrain.heightMap.size_x() * terrain.cellSize);
		int i, j;
		i = (int)x;
		i = clamp(i, 0, (int)particleGrid.size_y() - 1);
		j = (int)z;
		j = clamp(j, 0, (int)particleGrid.size_x() - 1);
		float cellSize = terrain.cellSize / particleGrid.size_y();
		return ((float)particleGrid(j, i).particles.size()) / (cellSize * cellSize);
	}

	void updateGrid() {
		for (int i = 0; i < particleGrid.size_x(); i++) {
			for (int j = 0; j < particleGrid.size_y(); j++) {
				if (particleGrid(i, j).particles.empty()) {
					continue;
				}

				vec3 avgVel(0, 0, 0);
				float count = 0;
				float energy = 0;
				float cellSize = terrain.cellSize / particleGrid.size_y();
				
				for (int p = 0; p < particleGrid(i, j).particles.size(); p++) {
					float dist = vec3(particles[particleGrid(i, j).particles[p]].position - 
						vec3(	j * particleGrid.size_y() / (terrain.heightMap.size_y()*terrain.cellSize),
								particles[particleGrid(i, j).particles[p]].position.y,
								i * particleGrid.size_x() / (terrain.heightMap.size_x() * terrain.cellSize))
						).lengthSqr();
					
					avgVel += particles[particleGrid(i, j).particles[p]].velocity;
					count += 1.0;
				}
				if (count <= 0.01) {
					continue;
				}
				avgVel /= count;
				
				for (int p = 0; p < particleGrid(i,j).particles.size(); p++) {
					//vec3 pos = particles[particleGrid(i,j).particles[p]].position;
					particles[particleGrid(i,j).particles[p]].velocity += -avgVel*(args.viscosity/(count));
					particles[particleGrid(i,j).particles[p]].velocity = particles[particleGrid(i,j).particles[p]].velocity*(1.0 - args.clumpingFactor) + avgVel*args.clumpingFactor;
				}
			}
		}
	}

	void updateParticle(int i, float dTime = 0.1667) {
		if (particles[i].position.x < 0 || particles[i].position.z < 0 || particles[i].position.x > terrain.heightMap.size_y() * terrain.cellSize 
										|| particles[i].position.z > terrain.heightMap.size_x() * terrain.cellSize) {
			return;
		}

		float bounceFriction = args.bounceFriction;
		float stickyness = args.stickyness;
		float damping = args.dampingForce;
		float turbulance = args.turbulanceForce;
		vec3 hit, norm;
			
		//NOTE: the velocity that is being accessed is v0t, so we do not need to times by t, as far as I know
		particles[i].velocity.y += dTime * -9.8;		//apply acceleration of gravity. TODO this should follow v = v0t + (1/2)*a*t^2,

		bool collision = terrain.trace(particles[i].position, particles[i].position + particles[i].velocity * dTime, hit, norm);

		float density = computeDensity(particles[i].position.x, particles[i].position.z) * 0.0001;
			
		if (!collision) {
			particles[i].velocity += -particles[i].velocity * damping;
		} else {
			float length = particles[i].velocity.length();
			vec3 v = particles[i].velocity.normalized();
			vec3 r = -(2.0*(norm*v)*norm - v);
			float dotNorm = r*norm;
			if (particles[i].position.y < hit.y) {
				particles[i].position.y += 0.5*(hit.y - particles[i].position.y);
			}
			particles[i].velocity = (r)*length*(1.0 - bounceFriction) + length*vec3(frand() - 0.5,0,frand() - 0.5)*turbulance*density;
			if (length*dTime < stickyness) {
				particles[i].velocity *= 0.0;
			}
		}
		particles[i].position += particles[i].velocity * dTime;			//apply velocity to position
		registerParticleToGrid(i);	

		float xcoord = forceMap.width() * particles[i].position.x / (terrain.heightMap.size_y() * terrain.cellSize);
		float ycoord = forceMap.height() - forceMap.height() * particles[i].position.z / (terrain.heightMap.size_x() * terrain.cellSize);

		int xcoordi = xcoord;
		int ycoordi = ycoord;
		if (xcoordi < forceMap.width() - 1 && xcoordi >= 0 && ycoordi < forceMap.height() - 1&& ycoordi >= 0) {
			vec3 force = particles[i].velocity&vec3(1, 0, 1);
			//force = force;
			float mag = force.length();
			float drawColor = density * mag * 0.00025;
			*(float*)forceMap(xcoordi, ycoordi) += drawColor;
			/*
			//float alphax = xcoord - xcoordi;
			//float alphay = ycoord - ycoordi;
			*(float*)forceMap(xcoordi,ycoordi) += drawColor*(1-alphax)*(1-alphay);
			*(float*)forceMap(xcoordi+1,ycoordi) += drawColor*(alphax)*(1-alphay);
			*(float*)forceMap(xcoordi+1,ycoordi+1) += drawColor*(alphax)*(alphay);
			*(float*)forceMap(xcoordi,ycoordi+1) += drawColor*(1-alphax)*(alphay);*/
		}
	}

	//TODO extract out
	void handleEvents() {
		SDL_Event evnt;
		static bool skip = false;
		while (SDL_PollEvent(&evnt)) {
			switch(evnt.type) {
				case SDL_MOUSEBUTTONDOWN:
					input.mouseDownEvent(evnt.button.button);
					gui->mouseDown(evnt.button.x,viewHeight - evnt.button.y);
					break;
				case SDL_MOUSEBUTTONUP:
					input.mouseUpEvent(evnt.button.button);
					gui->mouseUp(evnt.button.x,viewHeight - evnt.button.y);
					break;
				case SDL_MOUSEMOTION:
					gui->updateMouse(evnt.motion.x,viewHeight - evnt.motion.y);
					input.mouseX = evnt.motion.x;
					input.mouseY = viewHeight - evnt.motion.y;
					break;
				case SDL_KEYUP:
					//cout << evnt.key.keysym.sym << endl;
					input.keyUpEvent(evnt.key.keysym.sym);
					if (evnt.key.keysym.sym == 27) {
						quit();
					}
					break;
				case SDL_KEYDOWN:
					input.keyDownEvent(evnt.key.keysym.sym);
					//cout << evnt.key.keysym.sym << endl;
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
			cam.setPosition(cam.getPosition() - cam.getForwardAxis()*10);
		} else if (input.keysDown[keyMappings.moveBackwardKey]) {
			cam.setPosition(cam.getPosition() + cam.getForwardAxis()*10);
		}
		if (input.keysDown[keyMappings.moveLeftKey]) {
			cam.setPosition(cam.getPosition() - cam.getRightAxis()*10);
		} else if (input.keysDown[keyMappings.moveRightKey]) {
			cam.setPosition(cam.getPosition() + cam.getRightAxis()*10);
		}
		if (input.keysDown[keyMappings.moveUpKey]) {
			cam.setPosition(cam.getPosition() + vec3(0,10,0));
		} else if (input.keysDown[keyMappings.moveDownKey]) {
			cam.setPosition(cam.getPosition() - vec3(0,10,0));
		}

		if (input.keysDown[keyMappings.rotateLeftKey]) {
			mat4 rot;
			rot.makeRotation(0.05,vec3(0,1,0));
			cam.transform = rot*(cam.transform & mat4(1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1)) + (cam.transform & mat4(0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0));
		} else if (input.keysDown[keyMappings.rotateRightKey]) {
			mat4 rot;
			rot.makeRotation(-0.05,vec3(0,1,0));
			cam.transform = rot*(cam.transform & mat4(1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1)) + (cam.transform & mat4(0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0));
		}
		if (input.keysDown[keyMappings.rotateUpKey]) {
			mat4 rot;
			rot.makeRotation(-0.05,cam.getRightAxis());
			cam.transform = rot*(cam.transform & mat4(1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1)) + (cam.transform & mat4(0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0));
		} else if (input.keysDown[keyMappings.rotateDownKey]) {
			mat4 rot;
			rot.makeRotation(0.05,cam.getRightAxis());
			cam.transform = rot*(cam.transform & mat4(1,1,1,0,1,1,1,0,1,1,1,0,1,1,1,1)) + (cam.transform & mat4(0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0));
		}
	}

	//TODO extract out
	void updateUI() {
		gui->updateMouse(input.mouseX,input.mouseY);
	
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
			initParticles();
		}

		if (((UIButton*)(gui->childControls[saveFMapBtn]))->pollPushed()) {
			exportForceMapAsDEM(args.flowPathOutputFile + ".dem");
			forceMap.exportAs_BMP(args.flowPathOutputFile + ".bmp");
		}

		if (((UISlider*)(gui->childControls[gridSizeSlider]))->pollChanged()) {
			args.gridSize = (int)((UISlider*)(gui->childControls[gridSizeSlider]))->sliderValue;
			resetGrid();
		}
	}

	void mainLoop() {
		int itercount = 0;
		if (args.verboseOutput) {
			cout << "Starting Simulation" << endl;
		}
		while(true) {

			//TODO extract out
			if (VideoSettings.enableDisplay) {
				handleEvents();
				handleInput();
				updateUI();
			}
			
			//clear the particle grid
			clearGrid();

			for (int i = 0; i < particles.size(); i++) {
				updateParticle(i);
			}
			updateGrid();

			//TODO extract out
			if (VideoSettings.enableDisplay) {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				if (VideoSettings.stereoEnable) {
					render(1);
					render(2);
				} else {
					render(0);
				}
				SDL_GL_SwapBuffers();
			}
	
			/*fout << iter << "\t";
			for (int i = 0; i < particles.size(); i++) {
				fout << particles[i].position.x << " " << particles[i].position.y << " " << particles[i].position.z << "\t";
			}
			fout << endl;
			iter++;*/
			itercount++;

			//TODO extract out
			if (!VideoSettings.enableDisplay) {
				//cout << "iteration: " << itercount << endl;
			}
			if (itercount >= args.maxIterations && !VideoSettings.enableDisplay) {
				break;
			}
		}

		//TODO extract out
		if (VideoSettings.enableDisplay) {
			quit();
		} else if (args.flowPathOutputFile != "") {
			forceMap.exportAs_BMP(args.flowPathOutputFile + ".bmp");
			exportForceMapAsDEM(args.flowPathOutputFile + ".dem");
		}
	}

	void exportForceMapAsDEM(string filename) {
		if (args.verboseOutput) cout << "Generating ASCII Grid File." << endl;
		//ximage tmp = forceMap.resizedTo(terrain.heightMap.size_y(),terrain.heightMap.size_x());
		xarray<float> fmap(terrain.heightMap.size_y(),terrain.heightMap.size_x());
		for (int i = 0; i < fmap.size_x(); i++) {
			for (int j = 0; j < fmap.size_y(); j++) {
				float val = forceMap.getPixelLerp(float(j)/fmap.size_y(),float(fmap.size_x() - i - 1)/fmap.size_x()).x;
				if (val > 0.0) {
					fmap(i,j) = val;
				} else {
					fmap(i,j) = -9999.0;
				}
			}
		}
		exportASCII_GRID(filename,fmap,terrain.cellSize,terrain.xCorner,terrain.yCorner);
		if (args.verboseOutput) cout << "Exported flow path to ACSII GRID file as " << filename << endl;
	}

	void quit() {
		if (args.flowPathOutputFile != "") {
			//forceMap.exportAs_BMP(args.flowPathOutputFile + ".bmp");
			//exportForceMapAsDEM(args.flowPathOutputFile + ".dem");
		}
		fout.close();
		exit(0);
	}

	void render(int renderIndex = 0) {
		static float theta = 0;
		float r = terrain.cellSize * (terrain.heightMap.size_x() + terrain.heightMap.size_y()) / 2;
		vec3 lookDir;
		//lookDir.x = cos(look.x)*cos(look.y);
		//lookDir.z = sin(look.x)*cos(look.y);
		//lookDir.y = sin(look.y);
		//cam.setPosition(vec3(r*cos(theta),0,r*sin(theta)));
		//cam.lookAt(cam.transform.getTranslation() + lookDir);
		//cam.lookAt(vec3(0,-1000,0));

		theta += 0.02;
		
		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(cam.fov, float(viewWidth)/viewHeight, cam.nearZ, cam.farZ);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glLoadMatrixf(cam.transform.inverse().ptr);
		gluLookAt(cos(viewRotation)*terrain.cellSize*terrain.heightMap.size_x(),0,sin(viewRotation)*terrain.cellSize*terrain.heightMap.size_x(),0,-1000,0,0,1,0);
		glColor3f(1.0,1.0,1.0);
		//glScalef(1.5,1.5,1.5);
		glTranslatef(-r/2,-1500,-r/2);*/

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
			camStereoOffset = cam.getRightAxis() * VideoSettings.stereoOffset * (VideoSettings.stereoReverse? -1 : 1);
			glViewport(0, 0, viewWidth / 2, viewHeight);
			break;
		case 2:
			camStereoOffset = cam.getRightAxis() * VideoSettings.stereoOffset * (VideoSettings.stereoReverse? 1 : -1);
			glViewport(viewWidth / 2, 0, viewWidth / 2, viewHeight);
			break;
		}
			
		//glOrtho(0,terrain.cellSize*terrain.heightMap.size_y(),0,terrain.cellSize*terrain.heightMap.size_x(),-5000,5000);
		gluPerspective(cam.fov,float(viewWidth)/viewHeight,cam.nearZ,cam.farZ);
		glMatrixMode(GL_MODELVIEW);
		vec3 focalPoint;
		focalPoint = -cam.getForwardAxis()*VideoSettings.stereoFocalDistance+cam.getPosition();
		Camera oldCam = cam;
		cam.move(camStereoOffset);
		cam.lookAt(focalPoint);

		glLoadMatrixf(cam.transform.inverse().ptr);
		cam = oldCam;

		//glLoadIdentity();
		//glTranslatef(0,0,-1000);
		//glTranslatef(0,100,0);
		//glRotatef(90,-1,0,0);
		//glScalef(1,-1,1);
		//glScalef(0.5,0.5,0.5);
				
		glColor3fv(VideoSettings.terrainColor.ptr);
		glScalef(1.0,VideoSettings.heightScaling,1.0);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3,GL_FLOAT,sizeof(TerrainVertex),&terrain.verts[0]);
		glTexCoordPointer(3,GL_FLOAT,sizeof(TerrainVertex),&terrain.verts[0].texcoords.x);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,terrainTex);
		glDrawElements(GL_QUADS,4*terrain.quads.size(),GL_UNSIGNED_INT, &terrain.quads[0]);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		
		/*vec3 start;
		vec3 end;
		vec3 norm;
		vec3 hit;
		start = vec3(1000,2000,500);
		end = vec3(1000  + frand()*3000,0,500+ frand()*3000);
		terrain.trace(start,end,hit,norm);

		glColor3f(1,0,0);
		glBegin(GL_LINES);
		glVertex3f(hit.x,hit.y,hit.z);
		glVertex3f(hit.x + norm.x*200, hit.y + norm.y*200, hit.z + norm.z*200);
		glEnd();

		glColor3f(0,1,0);
		glBegin(GL_LINES);
		glVertex3f(start.x,start.y,start.z);
		glVertex3f(end.x,end.y,end.z);
		glEnd();
		*/
		//glDisable(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0 * particleTex);

		//glEnable(GL_BLEND);
		//glDepthMask(GL_FALSE);
		//glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
		//glBlendFunc(GL_SRC_COLOR, GL_ONE);
		//glColor3f(0.75,0.25,0.25);
		glPointSize(VideoSettings.particleSize);
		mat4 view;
		glGetFloatv(GL_MODELVIEW_MATRIX, view[0]);
		view.invertAffine();

		glBegin(GL_POINTS);
		for (int i = 0; i < particles.size(); i++) {
			vec3 pos = particles[i].position;
			vec3 color;
			if (VideoSettings.showDensity) {
				float alpha = 0.0001*computeDensity(particles[i].position.x, particles[i].position.z);
				alpha = (6.0 - alpha) / 6.0;
				alpha = fclamp(alpha, 0, 1.0);
				alpha = pow(alpha, 2.0f);
				alpha *= 5.999;
				int alphai = (int)alpha;
				float w = alpha - alphai;
			
				color = VideoSettings.particleDensityColor[alphai] * (1.0 - w) + VideoSettings.particleDensityColor[clamp(alphai + 1, 0, 5)] * (w);
			} else {
				color = VideoSettings.particleColor;
			}


			
			glColor3fv(color.ptr);
			glVertex3fv(pos.ptr);

			//float size = 0.025*(vec3(view[3]) - pos).length();
			//vec3 right = size*vec3(view[0]);
			//vec3 up = size*vec3(view[1]);

			/*vec3 v1 = pos - right - up;
			vec3 v2 = pos + right - up;
			vec3 v3 = pos + right + up;
			vec3 v4 = pos - right + up;*/
			
			/*glTexCoord2f(0,0);
			glVertex3f(v1.x,v1.y,v1.z);
			glColor3fv(color.ptr);
			glTexCoord2f(1,0);
			glVertex3f(v2.x,v2.y,v2.z);
			glColor3fv(color.ptr);
			glTexCoord2f(1,1);
			glVertex3f(v3.x,v3.y,v3.z);
			glColor3fv(color.ptr);
			glTexCoord2f(0,1);
			glVertex3f(v4.x,v4.y,v4.z);
			glColor3fv(color.ptr);*/
		}
		glEnd();

		renderUIPanelOpenGL(gui,viewWidth,viewHeight);

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		/*
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float accuracy = 0.5;
		gluOrtho2D(0,forceMap.width()*accuracy,forceMap.height()*accuracy,0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST);
		vector<Polygon2d> polygons;
		marchingSquares(forceMap,0.01,accuracy,polygons);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1,1,1,1);
		
		//cout << polygons.size() << endl;

		for (int i = 0; i < polygons.size(); i++) {
			glBegin(GL_LINE_STRIP);
			for (int j = 0; j < polygons[i].points.size(); j++) {		
				glVertex2fv(&polygons[i].points[j].x);	
			}
			glEnd();
		}*/		
	}
};


#endif