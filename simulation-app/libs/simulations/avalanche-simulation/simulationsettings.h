/**
* simulationsettings.h
* @fileoverview .h file designed to provide a struct for the Simulation Settings
* @author Unknown
* Created: Unknown
*/

#ifndef SIMULATIONSETTINGS_H
#define SIMULATIONSETTINGS_H

//TODO clean up
struct SimulationSettings {

	//stores settings for the model simulation
	string elevationDEMFile;
	string terrainColorFile;
	string startingZoneFile;
	string flowPathOutputFile;
	string pathFile;
	string pathDistanceMap;
	float initialHeight;
	float bounceFriction;
	float stickyness;
	float dampingForce;
	float turbulanceForce;
	float clumpingFactor;
	float viscosity;
	bool  disableView;
	bool  verboseOutput;
	int	  gridSize;
	int	  maxIterations;

	//Constructor
	SimulationSettings() {
		elevationDEMFile = "dem.txt";
		terrainColorFile = "terrain.bmp";
		startingZoneFile = "startzone.bmp";
		flowPathOutputFile = "flowpath.bmp";
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
	}

	//Method designed to load the simulation settings from a settings file
	//assumes variable types and variable names match up in the text file (case sensitive)
	void parseSettings(const string &settingsFile) {
		ifstream fin(settingsFile.c_str());
		if (fin.fail()) {
			cout << "ERROR: Failed to open file: " << settingsFile << endl;
			return;
		}
		cout << "Loading settings file: " << settingsFile << endl;

		while (!fin.eof()) {
			string tmpline;
			stringstream line;
			getline(fin, tmpline);
			line << tmpline;
			string param;
			line >> param;
			if (param.empty()) continue;
			if (param[0] == '#') continue;

			//TODO refactor if statement, potentially use map to fix this
			if (param == "elevationDEMFile") {
				line >> elevationDEMFile;
			}
			else if (param == "terrainColorFile") {
				line >> terrainColorFile;
			}
			else if (param == "startingZoneFile") {
				line >> startingZoneFile;
			}
			else if (param == "flowPathOutputFile") {
				line >> flowPathOutputFile;
			}
			else if (param == "pathFile") {
				line >> pathFile;
			}
			else if (param == "pathDistanceMap") {
				line >> pathDistanceMap;
			}
			else if (param == "initialHeight") {
				line >> initialHeight;
			}
			else if (param == "bounceFriction") {
				line >> bounceFriction;
			}
			else if (param == "stickyness") {
				line >> stickyness;
			}
			else if (param == "dampingForce") {
				line >> dampingForce;
			}
			else if (param == "turbulanceForce") {
				line >> turbulanceForce;
			}
			else if (param == "clumpingFactor") {
				line >> clumpingFactor;
			}
			else if (param == "viscosity") {
				line >> viscosity;
			}
			else if (param == "gridSize") {
				line >> gridSize;
			}
			else if (param == "maxIterations") {
				line >> maxIterations;
			}
			else if (param == "verboseOutput") {
				line >> verboseOutput;
			}
		}

	}
};

#endif