#ifndef TRAINING_H
#define TRAINING_H

#include "avalanchesim.h"
#include "terrain/terrain.h"
#include "polyraster.h"
#include "csvload.h"
#include "genetic_prog.h"
#include <xutil.h>
#include <vector>
using namespace std;


struct TrainingPolygonData {
	vector<polygon2d>		paths;
	vector<polygon2d>		startzones;
	void loadData(const string &pathFileName, const string &startzoneFileName) {
		ifstream fin(pathFileName.c_str());
		string line;
		getline(fin,line);
		int currentID = 0;
		polygon2d currentPoly;
		while (!fin.eof()) {
			stringstream ss;
			getline(fin,line);
			ss << line;
			int id;
			ss >> id;
			
			double x;
			double y;
			string trash;
			
			if (id != currentID) {
				paths.push_back(currentPoly);
				currentPoly.points.clear();
				currentID = id;
			}
			ss >> x >> y >> trash >> trash >> trash >> trash >> currentPoly.id;
			currentPoly.points.push_back(dvec2(x,y));
		}
		paths.push_back(currentPoly);

		fin.close();
		fin.clear();

		fin.open(startzoneFileName.c_str());
		getline(fin,line);
		currentID = 0;
		currentPoly = polygon2d();
		while (!fin.eof()) {
			stringstream ss;
			getline(fin,line);
			ss << line;
			int id;
			ss >> id;
			
			double x;
			double y;
			string trash;
			
			if (id != currentID) {
				startzones.push_back(currentPoly);
				currentPoly.points.clear();
				currentID = id;
			}
			ss >> x >> y >> trash >> trash >> trash >> trash >> trash >> trash >> trash >> currentPoly.id;
			currentPoly.points.push_back(dvec2(x,y));
		}
		startzones.push_back(currentPoly);

	}
};


struct MapPathToStartZones {
	int  pathIndex;
	vector<int> startZoneIndices;
};



void generateTrainingSets() {
	TrainingPolygonData polygonData;
	
	vector<MapPathToStartZones> zoneMappingTable;
	vector<vector<string> > pathTable;
	vector<vector<string> > startZoneTable;
	
	Terrain baseTerrain;
	cout << "Loading Polygon Data..." << endl;
	polygonData.loadData("training_data/paths.txt","training_data/startzones.txt");
	
	cout << "Loading DEM Data..." << endl;

	ximage test;
	float tmpx,tmpy;
	polyToRaster(polygonData.startzones[0].points,test,2.0001,tmpx,tmpy);
	test.exportAs_BMP("test.bmp");

	baseTerrain.loadFrom_DEM_ASCII("training_data/rogers_pass_compnew2.asc",false);

	cout << "Generating Data sets..." << endl;
	for (int i = 0; i < polygonData.paths.size(); i++) {
		cout << "  Generating Dataset: " << i << endl;
		ximage pathRaster;
		ximage startzoneRaster;
		float cellSize = 10.001;
		float xCorner;
		float yCorner;
		polyToRaster(polygonData.paths[i].points,pathRaster,cellSize,xCorner,yCorner);
		startzoneRaster = ximage(pathRaster.width(),pathRaster.height());
		startzoneRaster.fill(vec4(0,0,0,0));
		
		for (int j = 0; j < polygonData.startzones.size(); j++) {
			if (polygonData.startzones[j].id == polygonData.paths[i].id) {
				polyToRaster(polygonData.startzones[j].points,startzoneRaster,cellSize,xCorner,yCorner,true);
			}
		}


		startzoneRaster.exportAs_BMP("training_data/data_" + toString(i) + "_startzones.bmp");
		pathRaster.exportAs_BMP("training_data/data_" + toString(i) + "_path.bmp");

		int size = pathRaster.width()*cellSize/baseTerrain.cellSize;
		xarray<float> heightMap(size,size);
		int xOffset = (xCorner - baseTerrain.xCorner)/baseTerrain.cellSize;
		int yOffset = (yCorner - baseTerrain.yCorner)/baseTerrain.cellSize;
		int clampSize = baseTerrain.heightMap.size_x() - 1;
		cout << xOffset << " " << yOffset << endl;
		for (int x = 0; x < size; x++) {
			for (int y = 0; y < size; y++) {			
				
				double hval = baseTerrain.heightMap(clamp((int)baseTerrain.heightMap.size_x()-size-yOffset+y-1,0,clampSize),clamp(xOffset+x,0,clampSize));
				heightMap(y,x) = hval;
			}
		}
		exportASCII_GRID("training_data/data_" + toString(i) + "_dem.txt",heightMap,baseTerrain.cellSize,xCorner,yCorner);
	}
}


#endif