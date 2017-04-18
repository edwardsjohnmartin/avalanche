#ifndef MASSMOVEMENTSIMULATOR_H
#define MASSMOVEMENTSIMULATOR_H

#include "../terrain/terrain.h"
#include "particle.h"
#include "simulationsettings.h"
#include "particlebucket.h"

using namespace std;
using namespace xmath;
using namespace xutil;

struct MassMovementSimulation {

	//simulation stuff
	SimulationSettings	args;				//simulation parameters
	xarray<Particle> particles;				//the actual particles themselves
	xarray<ParticleBucket>	particleGrid;	//2d grid used for fluid dynamics calculations
	ximage			particleStart;			//input image for where the initial set of particles are created
	ximage			forceMap;				//output image that accumulates the motion of the particles
	Terrain			terrain;				//terrain map generated from the input DEM data

	void init(SimulationSettings argsIn, Terrain terrainIn) {
		args = argsIn;
		terrain = terrainIn;
		cout << "Success" << endl;
	}

	//TODO needs a terrain
	void initParticles() {
		//create all the particles for the simulator
		cout << "Initializing Particles" << endl;
		forceMap.fill(vec4(0, 0, 0, 0));
		particleStart.importFrom_BMP(args.startingZoneFile); //Inits particle start

		forceMap = ximage(particleStart.width(), particleStart.height(), 1, XIMAGE_FORMAT_GRAYSCALE_FLOAT32); //Inits Force Map
		forceMap.fill(vec4(0, 0, 0, 0));
		int numParticles = 0;
		float initialHeight = args.initialHeight;

		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i, j).xyz().lengthSqr();
				numParticles += nParticles;
			}
		}
		particles = xarray<Particle>(numParticles); //Inits Particles
		int index = 0;
		for (int i = 0; i < particleStart.width(); i++) {
			for (int j = 0; j < particleStart.height(); j++) {
				int nParticles = particleStart.getPixel(i, j).xyz().lengthSqr();
				for (int p = 0; p < nParticles; p++) {

					vec3 randval(0, 0, 0);
					for (int r = 0; r < 4; r++) {
						randval.x += (frand() - 0.5) / 2.0;
						randval.y += (frand() - 0.5) / 2.0;
						randval.z += (frand() - 0.5) / 2.0;
					}

					int x = terrain.heightMap.size_y() * i / particleStart.width();
					int y = terrain.heightMap.size_x() - terrain.heightMap.size_x() * j / particleStart.height();
					particles[index].position = vec3(randval.x * terrain.cellSize + x * terrain.cellSize, terrain.heightMap(y, x) + initialHeight, randval.z * terrain.cellSize + y * terrain.cellSize);
					particles[index].velocity = vec3(0, 0, 0);
					index++;
				}
			}
		}
		resetGrid();
	}

	void clearGrid() {
		//clear out all the particles from the grid
		for (int i = 0; i < particleGrid.size_x(); i++) {
			for (int j = 0; j < particleGrid.size_y(); j++) {
				particleGrid(i, j).particles.clear();
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
	}

	//TODO needs a terrain
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
						vec3(j * particleGrid.size_y() / (terrain.heightMap.size_y()*terrain.cellSize),
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

				for (int p = 0; p < particleGrid(i, j).particles.size(); p++) {
					particles[particleGrid(i, j).particles[p]].velocity += -avgVel*(args.viscosity / (count));
					particles[particleGrid(i, j).particles[p]].velocity = particles[particleGrid(i, j).particles[p]].velocity*(1.0 - args.clumpingFactor) + avgVel*args.clumpingFactor;
				}
			}
		}
	}

	void updateAllParticles() {
		clearGrid();
		for (int i = 0; i < particles.size(); i++) {
			updateParticle(i);
		}
		updateGrid();
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
		particles[i].velocity.y += dTime * -9.8;		//apply acceleration of gravity. TODO this should follow v = v0t + (1/2)*a*t^2, TODO this is wrong

		bool collision = terrain.trace(particles[i].position, particles[i].position + particles[i].velocity * dTime, hit, norm);

		float density = computeDensity(particles[i].position.x, particles[i].position.z) * 0.0001;

		if (!collision) {
			particles[i].velocity += -particles[i].velocity * damping;
		}
		else {
			float length = particles[i].velocity.length();
			vec3 v = particles[i].velocity.normalized();
			vec3 r = -(2.0*(norm*v)*norm - v);
			float dotNorm = r*norm;
			if (particles[i].position.y < hit.y) {
				particles[i].position.y += 0.5*(hit.y - particles[i].position.y);
			}
			particles[i].velocity = (r)*length*(1.0 - bounceFriction) + length*vec3(frand() - 0.5, 0, frand() - 0.5)*turbulance*density;
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
		if (xcoordi < forceMap.width() - 1 && xcoordi >= 0 && ycoordi < forceMap.height() - 1 && ycoordi >= 0) {
			vec3 force = particles[i].velocity&vec3(1, 0, 1);
			float mag = force.length();
			float drawColor = density * mag * 0.00025;
			*(float*)forceMap(xcoordi, ycoordi) += drawColor;
		}
	}
};

#endif