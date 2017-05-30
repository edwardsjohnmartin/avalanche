/**
 * simulationmanager.cpp
 * @fileoverview .cpp file designed to manage simulations
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: March 13th, 2017
 */

//Includes
#include <nan.h>
#include <iostream>
#include <fstream>
#include <istream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <map>
#include <utility>

//Custom files
#include "xlib.h"
#include "massmovementsimulator.h"
#include "simulationfactory.h"

using namespace std;

//Simulation and density colors maps
map<string, MassMovementSimulator> simulations;
vector<xlib::vec3> particleDensityColor;

//Method designed to log the given string to a log file
void logToFile(string info) {
    ofstream logFile;
    logFile.open("logFile.txt", ios::out | ios::app);
    logFile << info << endl;
    logFile.close();
}

//Functions separated into their own files because of the length of the function(s)
#include "simulationaddremove.h"
#include "getnextsimulationframe.h"
#include "getsimulationterraindata.h"
#include "simulationgetset.h"

//Method designed to initialize the addon
void Init(v8::Local<v8::Object> exports) { 
    //Exports
    exports->Set(Nan::New("addSimulation").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(addSimulation)->GetFunction());
    exports->Set(Nan::New("removeSimulation").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(removeSimulation)->GetFunction());
    exports->Set(Nan::New("getNextSimulationFrame").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getNextSimulationFrame)->GetFunction());
    exports->Set(Nan::New("getNextSimulationFrameFromGrid").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getNextSimulationFrameFromGrid)->GetFunction());
	exports->Set(Nan::New("skipSimulationFrames").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(skipSimulationFrames)->GetFunction());
    exports->Set(Nan::New("getSimulationTerrainData").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationTerrainData)->GetFunction());
    exports->Set(Nan::New("getAllSimulationSettings").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getAllSimulationSettings)->GetFunction());
    exports->Set(Nan::New("getSimulationInitialHeight").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationInitialHeight)->GetFunction());
    exports->Set(Nan::New("setSimulationInitialHeight").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationInitialHeight)->GetFunction());
    exports->Set(Nan::New("getSimulationBounceFriction").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationBounceFriction)->GetFunction());
    exports->Set(Nan::New("setSimulationBounceFriction").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationBounceFriction)->GetFunction());
    exports->Set(Nan::New("getSimulationStickyness").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationStickyness)->GetFunction());
    exports->Set(Nan::New("setSimulationStickyness").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationStickyness)->GetFunction());
    exports->Set(Nan::New("getSimulationDampingForce").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationDampingForce)->GetFunction());
    exports->Set(Nan::New("setSimulationDampingForce").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationDampingForce)->GetFunction());
    exports->Set(Nan::New("getSimulationTurbulanceForce").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationTurbulanceForce)->GetFunction());
    exports->Set(Nan::New("setSimulationTurbulanceForce").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationTurbulanceForce)->GetFunction());
    exports->Set(Nan::New("getSimulationClumpingFactor").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationClumpingFactor)->GetFunction());
    exports->Set(Nan::New("setSimulationClumpingFactor").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationClumpingFactor)->GetFunction());
    exports->Set(Nan::New("getSimulationViscosity").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationViscosity)->GetFunction());
    exports->Set(Nan::New("setSimulationViscosity").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationViscosity)->GetFunction());
    exports->Set(Nan::New("getSimulationFramesPerSecond").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSimulationFramesPerSecond)->GetFunction());
    exports->Set(Nan::New("setSimulationFramesPerSecond").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setSimulationFramesPerSecond)->GetFunction());

    //Init particle density color map
    particleDensityColor.push_back(xlib::vec3(1.0, 1.0, 1.0));
    particleDensityColor.push_back(xlib::vec3(1.0, 0.0, 0.0));
    particleDensityColor.push_back(xlib::vec3(1.0, 1.0, 0.0));
    particleDensityColor.push_back(xlib::vec3(0.0, 1.0, 0.0));
    particleDensityColor.push_back(xlib::vec3(0.0, 1.0, 1.0));
    particleDensityColor.push_back(xlib::vec3(0.0, 0.0, 1.0));
}

//Macro
NODE_MODULE(simulationmanager, Init)  