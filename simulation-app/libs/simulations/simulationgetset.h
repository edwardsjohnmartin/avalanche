/**
 * simulationgetset.h
 * @fileoverview .h file designed to provide get and set functionality for simulations
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: April 4th, 2017
 */

#ifndef SIMULATIONGETSET_H
#define SIMULATIONGETSET_H

//Method designed to get all of the settings of the given simulation
void getAllSimulationSettings(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //v8 variables
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    //Build map of properties
    MassMovementSimulator &simulator = simulations[id];
    v8::Local<v8::Object> simulationSettings = v8::Object::New(isolate);

    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "initialHeight"), Nan::New(simulator.initialHeight)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "bounceFriction"), Nan::New(simulator.bounceFriction)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "stickyness"), Nan::New(simulator.stickyness)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "dampingForce"), Nan::New(simulator.dampingForce)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "turbulanceForce"), Nan::New(simulator.turbulanceForce)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "clumpingFactor"), Nan::New(simulator.clumpingFactor)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "viscosity"), Nan::New(simulator.viscosity)); 
    simulationSettings->Set(context, v8::String::NewFromUtf8(isolate, "framesPerSecond"), Nan::New(simulator.framesPerSecond)); 
    
    //Set return
    info.GetReturnValue().Set(simulationSettings);
}

//Method designed to return the initial height of the given simulation
void getSimulationInitialHeight(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].initialHeight));
}

//Method designed to set the initial height of the given simulation
void setSimulationInitialHeight(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newInitialHeight)");
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

    //Set property
    simulations[id].initialHeight = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the bounce friction of the given simulation
void getSimulationBounceFriction(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].bounceFriction));
}

//Method designed to set the bounce friction of the given simulation
void setSimulationBounceFriction(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newBounceFriction)");
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

    //Set property
    simulations[id].bounceFriction = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the stickyness of the given simulation
void getSimulationStickyness(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].stickyness));
}

//Method designed to set the stickyness of the given simulation
void setSimulationStickyness(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newStickyness)");
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

    //Set property
    simulations[id].stickyness = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the damping force of the given simulation
void getSimulationDampingForce(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].dampingForce));
}

//Method designed to set the damping force of the given simulation
void setSimulationDampingForce(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newDampingForce)");
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

    //Set property
    simulations[id].dampingForce = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the turbulance force of the given simulation
void getSimulationTurbulanceForce(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].turbulanceForce));
}

//Method designed to set the turbulance force of the given simulation
void setSimulationTurbulanceForce(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newTurbulanceForce)");
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

    //Set property
    simulations[id].turbulanceForce = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the clumping factor of the given simulation
void getSimulationClumpingFactor(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].clumpingFactor));
}

//Method designed to set the clumping factor of the given simulation
void setSimulationClumpingFactor(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newClumpingFactor)");
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

    //Set property
    simulations[id].clumpingFactor = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the viscosity of the given simulation
void getSimulationViscosity(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].viscosity));
}

//Method designed to set the viscosity of the given simulation
void setSimulationViscosity(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newViscosity)");
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

    //Set property
    simulations[id].viscosity = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

//Method designed to return the frames per second of the given simulation
void getSimulationFramesPerSecond(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id)");
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

    //Set return value
    info.GetReturnValue().Set(Nan::New(simulations[id].framesPerSecond));
}

//Method designed to set the frames per second of the given simulation
void setSimulationFramesPerSecond(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 2 || !info[0]->IsString() || !info[1]->IsNumber()) {
        Nan::ThrowTypeError("Parameter Mismatch. Function requires (string id, number newFramesPerSecond)");
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

    //Set property
    simulations[id].framesPerSecond = info[1]->NumberValue();

    //Set return
    info.GetReturnValue().Set(Nan::New(true));
}

#endif