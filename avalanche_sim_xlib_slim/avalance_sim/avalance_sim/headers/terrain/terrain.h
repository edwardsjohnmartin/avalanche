/**
* terrain.h
* Header file designed to 
*/

#ifndef TERRAIN_H
#define TERRAIN_H

#include "../xlib-slim/xlib.h"
#include "terrainquad.h"
#include "terrainvertex.h"

struct Terrain {

	float	xCorner;
	float	yCorner;
	float	cellSize;
	xlib::ximage	terrainColor;
	xlib::xarray<float> heightMap;
	xlib::xarray<TerrainVertex> verts;
	xlib::xarray<TerrainQuad> quads;
	
	//Method designed to TODO
	void exportNormalMap(string filename) {
		cout << "Generating terrain lighting info." << endl;
		xlib::ximage normmap(heightMap.size_y(), heightMap.size_x());
		for (int i = 0; i < normmap.width(); i++) {
			for (int j = 0; j < normmap.height(); j++) {
				float ndotl;
				xlib::vec3 normal = computeNormal(i, j);
				ndotl = xlib::fclamp(0.25 + 0.75 * normal * xlib::vec3(1, 1, 1).normalized(), 0, 1);
				normal += 1.0;
				normal *= 0.5;
				normmap.setPixel(j, normmap.width() - i - 1, xlib::vec4(ndotl));
			}
		}
		cout << "Exporting terrain lighting info to: " << filename << endl;
		normmap.exportAs_BMP(filename);
	}

	//Method designed to TODO
	void loadFrom_DEM_ASCII(string fileName, bool genVerts = true) {

		int xSize;
		int ySize;
		double nodataValue;
		string trash;
		ifstream fin(fileName.c_str());
		if (fin.fail()) {
			cout << "Error: Failed to open file: " << fileName << endl;
			exit(1);
		}

		cout << "Loading ASCII GRID file: " << fileName << endl;
		fin >> trash >> xSize;
		fin >> trash >> ySize;
		fin >> trash >> xCorner;
		fin >> trash >> yCorner;
		fin >> trash >> cellSize;
		fin >> trash >> nodataValue;

		heightMap = xlib::xarray<float>(ySize, xSize);
		if (genVerts) {
			verts = xlib::xarray<TerrainVertex>(ySize, xSize);
		}

		float maxHeight = 0;
		float minHeight = 999999;
		for (int i = 0; i < ySize; i++) {
			for (int j = 0; j < xSize; j++) {
				fin >> heightMap(i,j);
				if (heightMap(i,j) <= nodataValue) {
					heightMap(i,j) = 0;
				}
				if (genVerts) {
					TerrainVertex v;
					v.position.x = j * cellSize;
					v.position.z = i * cellSize;
					v.position.y = heightMap(i, j);
					v.texcoords.x = j / float(xSize - 1);
					v.texcoords.y = 1.0 - (i) / float(ySize - 1);
					v.texcoords.z = heightMap(i, j);
					verts(i, j) = v;
				}
				if (heightMap(i, j) > maxHeight) {
					maxHeight = heightMap(i, j);
				}
				if (heightMap(i, j) < minHeight) {
					minHeight = heightMap(i, j);
				}
			}
		}
		if (genVerts) {
			quads = xlib::xarray<TerrainQuad>(ySize - 1, xSize - 1);
			for (int i = 0; i < ySize; i++) {
				for (int j = 0; j < xSize; j++) {
					verts(i, j).texcoords.z -= minHeight;
					verts(i, j).texcoords.z /= maxHeight;
					verts(i, j).position.y -= minHeight;
					heightMap(i, j) -= minHeight;
					//verts(i,j).position.y /= maxHeight;
				}
			}

			for (int i = 0; i < ySize-1; i++) {
				for (int j = 0; j < xSize-1; j++) {
					TerrainQuad q;
					q.a = j + (xSize) * i;
					q.b = j+1 + (xSize) * i;
					q.c = j+1 + (xSize) * (i + 1);
					q.d = j + (xSize) * (i + 1);
					quads(i, j) = q;
				}
			}
		}
	}

	//Method designed to TODO
	xlib::vec3 computeNormal(int i, int j) {
		if (i < 0) i = 0;
		if (j < 0) j = 0;
		if (i >= heightMap.size_x()) i = heightMap.size_x() - 1;
		if (j >= heightMap.size_y()) j = heightMap.size_y() - 1;
		float dx, dz;
		dx = heightMap(i, (j != 0 ? (j-1) : j)) - heightMap(i,j);
		dz = heightMap((i != 0 ? (i-1) : i) ,j) - heightMap(i,j);
		xlib::vec3 n;
		n.x = dx;
		n.z = dz;
		n.y = cellSize;
		n.normalize();
		return n;
	}

	//Method designed to TODO
	xlib::vec3 computeColor(int i, int j) {
		xlib::vec3 color;
		xlib::vec3 norm;
	}

	//Method designed to TODO
	bool trace(xlib::vec3 start, xlib::vec3 end, xlib::vec3 &hit, xlib::vec3 &normal) {
		//trace a ray to test for the intersect between the terrain surface and the ray
		//uses the DDA stepping ray marching algorithm
		int x,z;
		int endx, endz;
		float y = start.y;
		xlib::vec3 dir = end - start;
		dir.normalize();
		float s = sqrt(dir.x*dir.x + dir.z*dir.z);
		dir.x /= s;
		dir.y /= s;
		dir.z /= s;
		x = (start.x)/cellSize;
		z = (start.z)/cellSize;
		endx = end.x/cellSize;
		endz = end.z/cellSize;
		x = xlib::clamp(x, 0, (int)heightMap.size_y() - 1);
		z = xlib::clamp(z, 0, (int)heightMap.size_x() - 1);
		int prevSign = (heightMap(z,x) + cellSize - start.y) < 0 ? -1 : 1;

		if (x == endx && z == endz) {
			if (x >= heightMap.size_y() || z >= heightMap.size_x() || x < 0 || z < 0) {
				return false;
			}
			int sign = (heightMap(z,x) -  end.y) < 0 ? -1 : 1;
			if (prevSign == -1) return false;
			hit.x = start.x;
			hit.z = start.z;
			hit.y = heightMap(z,x);
			normal = computeNormal(z,x);
			return true;
		}
		int dx = (dir.x) < 0 ? -1 : 1;
		int dz = (dir.z) < 0 ? -1 : 1;
		int signY = (dir.y) < 0 ? -1 : 1;
		xlib::vec3 error(0, 0, 0);
		float dy = dir.y;
	
		for(;;) {
			if (x >= heightMap.size_y() || z >= heightMap.size_x() || x < 0 || z < 0 || (x == endx && z == endz)) {
				break;
			}
			x = xlib::clamp(x, 0, (int)heightMap.size_y() - 1);
			z = xlib::clamp(z, 0, (int)heightMap.size_x() - 1);
			
			float height = heightMap(z,x);
			
			int sign = (height  + cellSize - y) < 0 ? -1 : 1;
			
			if (sign ==-1) {
				if (x >= heightMap.size_y() || z >= heightMap.size_x() || x < 0 || z < 0) {
					return false;
				}
				hit.x = (x+error.x)*cellSize;
				hit.z = (z+error.z)*cellSize;
				hit.y = height + cellSize;
				normal = computeNormal(z,x);
				return true;
			}

			sign = prevSign;

			error.x += dir.x;
			error.z += dir.z;
			if ((int)error.x) { 
				x += dx;
				error.x -= dx;
			}
			if ((int)error.z) {
				z += dz;
				error.z -= dz;
			}
			y += dir.y*cellSize;
		}
		return false;
	}
};

#endif