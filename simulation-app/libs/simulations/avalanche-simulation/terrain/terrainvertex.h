/**
* terrainvertex.h
* @fileoverview .h file designed to define a terrain vertex struct
* @author Unknown
* Created: Unknown
*/

//TODO remove xlib

#ifndef TERRAINVERTEX_H
#define TERRAINVERTEX_H

#include "xlib.h"

struct TerrainVertex {
	xlib::vec3 position;
	xlib::vec3 normal;
	xlib::vec3 texcoords;
	xlib::vec3 color;
};

#endif