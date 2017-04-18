/**
* massmovementsimulator.h
* @fileoverview .h file designed to house a simulation struct
* @author Unknown
* Created: Unknown
*/

//TODO remove xlib

#ifndef MASSMOVEMENTSIMULATOR_H
#define MASSMOVEMENTSIMULATOR_H

#include "terrain.h"
#include "particle.h"
#include "simulationsettings.h"
#include "particlebucket.h"
#include "xlib.h"

struct MassMovementSimulator {

	string elevationDEMFile;
	string terrainColorFile;
	string startingZoneFile;
	string flowPathOutputFile;
	string pathFile;
	string pathDistanceMapFile; //TODO

	float initialHeight;
	float bounceFriction;
	float stickyness;
	float dampingForce;
	float turbulanceForce;
	float clumpingFactor;
	float viscosity;
	float framesPerSecond;

	bool  disableView; //TODO do I need this?
	bool  verboseOutput; //TODO do I need this?

	int	  gridSize;
	int	  maxIterations;

	xlib::xarray<Particle> particles;				//the actual particles themselves
	xlib::xarray<ParticleBucket> particleGrid;	//2d grid used for fluid dynamics calculations
	xlib::ximage particleStart;			//input image for where the initial set of particles are created
	xlib::ximage pathImage;				//input image for the actual flow path (used for training)
	xlib::ximage pathDistanceMap;		//input image that applies distance transform on the pathImage
	xlib::ximage forceMap;				//output image that accumulates the motion of the particles
	Terrain terrain;				//terrain map generated from the input DEM data

	//Constructor
	MassMovementSimulator() {
		elevationDEMFile = "libs/simulations/avalanche-simulation/resources/dem.txt";
		terrainColorFile = "libs/simulations/avalanche-simulation/resources/terrain.bmp";
		startingZoneFile = "libs/simulations/avalanche-simulation/resources/startzone.bmp";
		flowPathOutputFile = "libs/simulations/avalanche-simulation/resources/flowpath.bmp";
		initialHeight = 100;
		bounceFriction = 0.05;
		stickyness = 0.5;
		dampingForce = 0.02;
		turbulanceForce = 0.2;
		viscosity = 0.25;
		gridSize = 128;
		disableView = false;
		verboseOutput = true;
		maxIterations = 20000;
		framesPerSecond = 1;
	}

	//Method designed to initialize the terrain
	void initTerrain() {
		terrain.loadFrom_DEM_ASCII(elevationDEMFile);
		terrain.terrainColor.importFrom_BMP(terrainColorFile);
		terrain.terrainColor = terrain.terrainColor.resizedTo(512, 512);
	}

	//TODO clean up
	//Method designed to initialize the particles
	void initParticles() {
		//create all the particles for the simulator
		cout << "Initializing Particles" << endl;
		forceMap.fill(xlib::vec4(0, 0, 0, 0));
		particleStart.importFrom_BMP(startingZoneFile);

		//particleStart = particleStart.convertedTo(XIMAGE_FORMAT_RGB24);
		forceMap = xlib::ximage(particleStart.width(), particleStart.height(), 1, XIMAGE_FORMAT_GRAYSCALE_FLOAT32);
		forceMap.fill(xlib::vec4(0, 0, 0, 0));
		int numParticles = 0;

		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i, j).xyz().lengthSqr();
				numParticles += nParticles;
			}
		}
		particles = xlib::xarray<Particle>(numParticles);
		int index = 0;
		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i, j).xyz().lengthSqr();
				for (int p = 0; p < nParticles; p++) {
					xlib::vec3 randval(0, 0, 0);
					for (int r = 0; r < 4; r++) {
						randval.x += (xlib::frand() - 0.5) / 2.0;
						randval.y += (xlib::frand() - 0.5) / 2.0;
						randval.z += (xlib::frand() - 0.5) / 2.0;
					}

					int x = terrain.heightMap.size_y() * i / particleStart.width();
					int y = terrain.heightMap.size_x() - terrain.heightMap.size_x() * j / particleStart.height();
					particles[index].position = xlib::vec3(randval.x * terrain.cellSize + x * terrain.cellSize, terrain.heightMap(y, x) + initialHeight, randval.z * terrain.cellSize + y * terrain.cellSize);
					particles[index].velocity = xlib::vec3(0, 0, 0);
					index++;
				}
			}
		}
		resetGrid();
	}

	//Method designed to clear all the particles from the grid
	void clearGrid() {
		for (int i = 0; i < particleGrid.size_x(); i++) {
			for (int j = 0; j < particleGrid.size_y(); j++) {
				particleGrid(i, j).particles.clear();
			}
		}
	}

	//Method designed to reset the particle grid with a new size
	void resetGrid() {
		particleGrid = xlib::xarray<ParticleBucket>(int(double(gridSize) * terrain.heightMap.size_x() / 512.0), int(double(gridSize) * terrain.heightMap.size_y() / 512.0));
	}

	//Method designed to register a particle to a grid
	void registerParticleToGrid(int i) {
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

		ix = xlib::clamp(ix, 0, (int)particleGrid.size_y() - 1);
		iy = xlib::clamp(iy, 0, (int)particleGrid.size_x() - 1);

		x1 = xlib::clamp(x1, 0, (int)particleGrid.size_y() - 1);
		y1 = xlib::clamp(y1, 0, (int)particleGrid.size_x() - 1);

		particleGrid(iy, ix).particles.push_back(i);
		particleGrid(iy, x1).particles.push_back(i);
		particleGrid(y1, x1).particles.push_back(i);
		particleGrid(y1, ix).particles.push_back(i);
	}

	//Method designed to compute the particle density at a given (x, y) coordinate
	float computeDensity(float x, float z) {
		x = x * particleGrid.size_y() / (terrain.heightMap.size_y() * terrain.cellSize);
		z = z * particleGrid.size_x() / (terrain.heightMap.size_x() * terrain.cellSize);
		int i, j;
		i = (int)x;
		i = xlib::clamp(i, 0, (int)particleGrid.size_y() - 1);
		j = (int)z;
		j = xlib::clamp(j, 0, (int)particleGrid.size_x() - 1);
		float cellSize = terrain.cellSize / particleGrid.size_y();
		return ((float)particleGrid(j, i).particles.size()) / (cellSize * cellSize);
	}

	//Method designed to update the grid
	void updateGrid() {
		for (int i = 0; i < particleGrid.size_x(); i++) {
			for (int j = 0; j < particleGrid.size_y(); j++) {
				if (particleGrid(i, j).particles.empty()) {
					continue;
				}

				xlib::vec3 avgVel(0, 0, 0);
				float count = 0;
				float energy = 0;
				float cellSize = terrain.cellSize / particleGrid.size_y();

				for (int p = 0; p < particleGrid(i, j).particles.size(); p++) {
					float dist = xlib::vec3(particles[particleGrid(i, j).particles[p]].position -
						xlib::vec3(j * particleGrid.size_y() / (terrain.heightMap.size_y()*terrain.cellSize),
						particles[particleGrid(i, j).particles[p]].position.y,
						i * particleGrid.size_x() / (terrain.heightMap.size_x() * terrain.cellSize))
						).lengthSqr();

					avgVel += particles[particleGrid(i, j).particles[p]].velocity;
					count += 1.0;
				}
				if (count <= 0.01) {
					continue;
				}

				//TODO divide avgVel by frame rate to get velocity per frame

				avgVel /= count;

				for (int p = 0; p < particleGrid(i, j).particles.size(); p++) {
					particles[particleGrid(i, j).particles[p]].velocity += -avgVel * (viscosity / (count));
					particles[particleGrid(i, j).particles[p]].velocity = particles[particleGrid(i, j).particles[p]].velocity * (1.0 - clumpingFactor) + avgVel * clumpingFactor;
				}
			}
		}
	}

	//Method designed to update all particles
	void updateAllParticles() {
		clearGrid();
		for (int index = 0; index < particles.size(); index++) {
			updateParticle(index); //TODO determine if dtime is time per frame
		}
		updateGrid();
	}

	//TODO clean up
	//method designed to update a particle
	void updateParticle(int index, float dTime = 0.1667) {
		Particle &particle = particles[index];

		if (particle.position.x < 0 || particle.position.z < 0 || particle.position.x > terrain.heightMap.size_y() * terrain.cellSize
			|| particle.position.z > terrain.heightMap.size_x() * terrain.cellSize) {
			return;
		}

		float bounceFriction = bounceFriction;
		float stickyness = stickyness;
		float damping = dampingForce;
		float turbulance = turbulanceForce;
		xlib::vec3 hit, norm;

		//NOTE: v = v0 + a*t
		particle.velocity.y += dTime * -9.8;		

		bool collision = terrain.trace(particle.position, particle.position + particle.velocity * dTime, hit, norm);

		float density = computeDensity(particle.position.x, particle.position.z) * 0.0001;

		if (!collision) {
			particle.velocity += -particle.velocity * damping;
		}
		else {
			float length = particle.velocity.length();
			xlib::vec3 v = particle.velocity.normalized();
			xlib::vec3 r = -(2.0 * (norm * v) * norm - v);
			float dotNorm = r * norm;
			if (particle.position.y < hit.y) {
				particle.position.y += 0.5*(hit.y - particle.position.y);
			}
			particle.velocity = (r)* length * (1.0 - bounceFriction) + length * xlib::vec3(xlib::frand() - 0.5, 0, xlib::frand() - 0.5) * turbulance * density;
			if (length * dTime < stickyness) {
				particle.velocity *= 0.0;
			}
		}
		particle.position += particle.velocity * dTime;			//apply velocity to position
		registerParticleToGrid(index);

		float xcoord = forceMap.width() * particle.position.x / (terrain.heightMap.size_y() * terrain.cellSize);
		float ycoord = forceMap.height() - forceMap.height() * particle.position.z / (terrain.heightMap.size_x() * terrain.cellSize);

		int xcoordi = xcoord;
		int ycoordi = ycoord;
		if (xcoordi < forceMap.width() - 1 && xcoordi >= 0 && ycoordi < forceMap.height() - 1 && ycoordi >= 0) {
			xlib::vec3 force = particle.velocity&xlib::vec3(1, 0, 1);
			float mag = force.length();
			float drawColor = density * mag * 0.00025;
			*(float*)forceMap(xcoordi, ycoordi) += drawColor;
		}
	}
};

#endif