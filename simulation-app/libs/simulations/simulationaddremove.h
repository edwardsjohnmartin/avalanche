/**
 * simulationaddremove.h
 * @fileoverview .h file designed to provide add and remove functionality for simulations
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: April 4th, 2017
 */

#ifndef SIMULATIONADDREMOVE_H
#define SIMULATIONADDREMOVE_H

//Method designed to add a new simulation using the supplied id
void addSimulation(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 3 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch: Function requires (string id, string datafile, string settingsfile)");
        return;
    }

    //Extract params
    v8::String::Utf8Value param1(info[0]->ToString());
    string id = string(*param1);

	v8::String::Utf8Value param2(info[1]->ToString());
	string datafile = string(*param2);

	v8::String::Utf8Value param3(info[2]->ToString());
	string settingsfile = string(*param3);

    //Check if the id already exists
    if (simulations.count(id) == 1) {
        Nan::ThrowTypeError(("A simulation with id: " + id + " already exists").c_str());
        return;
    }

    //Add simulation
    MassMovementSimulator simulator = buildSimulator(datafile, settingsfile);
    simulations.insert(pair<string, MassMovementSimulator>(id, simulator));

    logToFile("Simulation created with id: " + id);

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to remove a simulation matching the supplied id
void removeSimulation(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch: Function requires (string id)");
        return;
    }

    //Extract params
    v8::String::Utf8Value param1(info[0]->ToString());
    string id = string(*param1);

    //Check if the id exists
    if (simulations.count(id) == 0) {
        Nan::ThrowTypeError(("No simulation with id: " + id + " exists").c_str());
        return;
    }

    //Remove simulation
    simulations.erase(id);
    
    logToFile("Simulation deleted with id: " + id);

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

#endif