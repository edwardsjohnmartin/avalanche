/**
* videosettings.h
* Header file designed to define a struct that contains video settings
*/

#ifndef VIDEOSETTINGS_H
#define VIDEOSETTINGS_H

#include<fstream>
#include "xlib.h"

struct VideoSettings {

	//Stores settings for the 3D viewer
	int resX;
	int resY;
	bool fullscreen;
	bool stereoEnable;
	bool enableDisplay;
	bool stereoReverse;
	float stereoOffset;
	float stereoFocalDistance;
	float particleSize;
	float heightScaling;
	vec3 terrainColor;
	vec3 particleColor;
	vec3 particleDensityColor[6];
	bool showDensity;
	vec3 cameraPosition;
	vec3 cameraDirection;

	//Constructor
	VideoSettings() {
		resX = 800;
		resY = 600;
		fullscreen = false;
	}

	//Method designed to read the settings from a file and assign them
	//Assumes variable types and variable names match up in the text file (case sensitive)
	void parseSettings(const string &settingsFile) {
		ifstream fin(settingsFile.c_str());
		if (fin.fail()) {
			cout << "failed to open file: " << settingsFile << endl;
			return;
		}
		cout << "Loading video settings file: " << settingsFile << endl;
		
		while (!fin.eof()) {
			string tmpline;
			stringstream line;
			getline(fin, tmpline);
			line << tmpline;
			string param;
			line >> param;
			if (param.empty()) continue;
			if (param[0] == '#') continue;

			//TODO clean up, potentially with a map
			if (param == "resolution") {
				line >> resX >> resY;
			}
			else if (param == "fullscreen") {
				line >> fullscreen;
			}
			else if (param == "stereoEnable") {
				line >> stereoEnable;
			}
			else if (param == "stereoFocalDistance") {
				line >> stereoFocalDistance;
			}
			else if (param == "stereoOffset") {
				line >> stereoOffset;
			}
			else if (param == "enableDisplay") {
				line >> enableDisplay;
			}
			else if (param == "stereoReverse") {
				line >> stereoReverse;
			}
			else if (param == "heightScaling") {
				line >> heightScaling;
			}
			else if (param == "terrainColor") {
				line >> terrainColor.x >> terrainColor.y >> terrainColor.z;
			}
			else if (param == "particleSize") {
				line >> particleSize;
			}
			else if (param == "particleColor") {
				line >> particleColor.x >> particleColor.y >> particleColor.z;
			}
			else if (param == "particleDensityColor[0]") {
				line >> particleDensityColor[0].x >> particleDensityColor[0].y >> particleDensityColor[0].z;
			}
			else if (param == "particleDensityColor[1]") {
				line >> particleDensityColor[1].x >> particleDensityColor[1].y >> particleDensityColor[1].z;
			}
			else if (param == "particleDensityColor[2]") {
				line >> particleDensityColor[2].x >> particleDensityColor[2].y >> particleDensityColor[2].z;
			}
			else if (param == "particleDensityColor[3]") {
				line >> particleDensityColor[3].x >> particleDensityColor[3].y >> particleDensityColor[3].z;
			}
			else if (param == "particleDensityColor[4]") {
				line >> particleDensityColor[4].x >> particleDensityColor[4].y >> particleDensityColor[4].z;
			}
			else if (param == "particleDensityColor[5]") {
				line >> particleDensityColor[5].x >> particleDensityColor[5].y >> particleDensityColor[5].z;
			}
			else if (param == "showDensity") {
				line >> showDensity;
			}
			else if (param == "cameraPosition") {
				line >> cameraPosition.x >> cameraPosition.y >> cameraPosition.z;
			}
			else if (param == "cameraDirection") {
				line >> cameraDirection.x >> cameraDirection.y >> cameraDirection.z;
			}
		}
	}
};

#endif