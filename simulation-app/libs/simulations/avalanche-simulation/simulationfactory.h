/**
* simulationfactory.h
* @fileoverview .h file designed to build simulations from the given params
* @author Zackary Hall (hallzac2@isu.edu)
* Created: March 16th, 2017
*/

#ifndef SIMULATIONFACTORY_H
#define SIMULATIONFACTORY_H

#include "massmovementsimulator.h"

//Method designed to assign variables of a simulation from a file
void parseSettings(const string &settingsFile, MassMovementSimulator &simulator) {
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
		if (param.empty()) {
			continue;
		}
		if (param[0] == '#') {
			continue;
		}

		//TODO refactor if statement, potentially use map to fix this
		if (param == "elevationDEMFile") {
			line >> simulator.elevationDEMFile;
		}
		else if (param == "terrainColorFile") {
			line >> simulator.terrainColorFile;
		}
		else if (param == "startingZoneFile") {
			line >> simulator.startingZoneFile;
		}
		else if (param == "flowPathOutputFile") {
			line >> simulator.flowPathOutputFile;
		}
		else if (param == "pathFile") {
			line >> simulator.pathFile;
		}
		else if (param == "pathDistanceMap") {
			line >> simulator.pathDistanceMapFile;
		}
		else if (param == "initialHeight") {
			line >> simulator.initialHeight;
		}
		else if (param == "bounceFriction") {
			line >> simulator.bounceFriction;
		}
		else if (param == "stickyness") {
			line >> simulator.stickyness;
		}
		else if (param == "dampingForce") {
			line >> simulator.dampingForce;
		}
		else if (param == "turbulanceForce") {
			line >> simulator.turbulanceForce;
		}
		else if (param == "clumpingFactor") {
			line >> simulator.clumpingFactor;
		}
		else if (param == "viscosity") {
			line >> simulator.viscosity;
		}
		else if (param == "gridSize") {
			line >> simulator.gridSize;
		}
		else if (param == "maxIterations") {
			line >> simulator.maxIterations;
		}
		else if (param == "verboseOutput") {
			line >> simulator.verboseOutput;
		}
		else if (param == "framesPerSecond") {
			line >> simulator.framesPerSecond;
		}
	}
}

//Method designed to return an initialized simulation
MassMovementSimulator buildSimulator(string fileName = "") {
	MassMovementSimulator simulator;
	if (fileName != "") {
		parseSettings(fileName, simulator);
	}
	simulator.initTerrain();
	simulator.initParticles();
	return simulator;
}

#endif