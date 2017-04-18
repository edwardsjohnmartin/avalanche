/**
* particle.h
* Header file designed to define a particle struct
*/

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../xlib-slim/xlib.h"

struct Particle {
	xlib::vec3 position;
	xlib::vec3 velocity;
};

#endif