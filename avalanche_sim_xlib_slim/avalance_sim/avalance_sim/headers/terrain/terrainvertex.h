/**
* terrainvertex.h
* Header file designed to provide a Terrain Vertex struct
*/

#ifndef TERRAINVERTEX_H
#define TERRAINVERTEX_H

#include "../xlib-slim/xlib.h"

struct TerrainVertex {
	xlib::vec3 position;
	xlib::vec3 normal;
	xlib::vec3 texcoords;
	xlib::vec3 color;
};

#endif