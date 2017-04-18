/**
* particle.h
* @fileoverview .h file designed to define a particle struct
* @author Unknown
* Created: Unknown
*/

//TODO remove xlib

#ifndef PARTICLE_H
#define PARTICLE_H

#include "xlib.h"

struct Particle {
	xlib::vec3 position;
	xlib::vec3 velocity;
};

#endif