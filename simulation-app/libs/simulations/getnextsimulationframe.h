/**
 * getnextsimulationframe.h
 * @fileoverview .h file designed to provide a method for retrieving the next frame in a simulation
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: April 4th, 2017
 */

#ifndef GETNEXTSIMULATIONFRAME_H
#define GETNEXTSIMULATIONFRAME_H

//Method designed to get the next frame for the simulation indexed by the given id
void getNextSimulationFrame(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch: Function requires (string id)");
        return;
    }

    //Extract params
    v8::String::Utf8Value param1(info[0]->ToString());
    string id = string(*param1);

    //Check if the id already exists
    if (simulations.count(id) == 0) {
        Nan::ThrowTypeError(("No simulation with id: " + id + " exists").c_str());
        return;
    }

    //v8 variables
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    MassMovementSimulator &simulator = simulations[id];

    //Update all particles
    simulations[id].updateAllParticles();

    //Build frame
    v8::Local<v8::Array> vertices = v8::Array::New(isolate, 0);
    v8::Local<v8::Array> colors = v8::Array::New(isolate, 0);

    v8::Local<v8::Array> vertex;
    v8::Local<v8::Array> color;

    float alpha;
    int alphai;
    float w;

    for (int index = 0; index < simulator.particles.size(); index++) { //TODO attempt to pick every 10th particle
        Particle &particle = simulator.particles[index];

        vertex = v8::Array::New(isolate, 3);
        color = v8::Array::New(isolate, 3);

        //Set vertex x,y,z
        vertex->Set(0, Nan::New(particle.position.x)); //TODO see what Edwards thinks
        vertex->Set(1, Nan::New(particle.position.y)); //TODO see what Edwards thinks
        vertex->Set(2, Nan::New(particle.position.z)); //TODO see what Edwards thinks

        //Add to vertices
        vertices->Set(index, vertex);

        //Determine color
        alpha = 0.0001 * simulator.computeDensity(particle.position.x, particle.position.z);
        alpha = pow(xlib::fclamp((6.0 - alpha) / 6.0, 0, 1.0), 2.0f) * 5.999;
        alphai = (int)alpha;
        w = alpha - alphai;
        
        color->Set(0, Nan::New(particleDensityColor[alphai].x * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].x * (w)));
        color->Set(1, Nan::New(particleDensityColor[alphai].y * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].y * (w)));
        color->Set(2, Nan::New(particleDensityColor[alphai].z * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].z * (w)));

        //Add to colors
        colors->Set(index, color);
    }

    //Create key strings
    v8::Local<v8::String> verticesStr = v8::String::NewFromUtf8(isolate, "vertices");
    v8::Local<v8::String> colorsStr = v8::String::NewFromUtf8(isolate, "colors");

    //Add to frame
    v8::Local<v8::Object> frame = v8::Object::New(isolate);
    frame->Set(context, verticesStr, vertices);
    frame->Set(context, colorsStr, colors);    

    //Set return Value
    info.GetReturnValue().Set(frame);
}

//Method designed to get the next simulation frame using the grid
void getNextSimulationFrameFromGrid(const Nan::FunctionCallbackInfo<v8::Value> &info) {
    //Params checking
    if (info.Length() != 1 || !info[0]->IsString()) {
        Nan::ThrowTypeError("Parameter Mismatch: Function requires (string id)");
        return;
    }

    //Extract params
    v8::String::Utf8Value param1(info[0]->ToString());
    string id = string(*param1);

    //Check if the id already exists
    if (simulations.count(id) == 0) {
        Nan::ThrowTypeError(("No simulation with id: " + id + " exists").c_str());
        return;
    }

    //v8 variables
    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Context> context = v8::Context::New(isolate);

    MassMovementSimulator &simulator = simulations[id];

    //Update all particles
    simulator.updateAllParticles();

    //Build frame
    v8::Local<v8::Array> vertices = v8::Array::New(isolate, 0);
    v8::Local<v8::Array> colors = v8::Array::New(isolate, 0);

    xlib::xarray<Particle> &particles = simulator.particles;
    int particleCount = 0;

    float alpha;
    int alphai;
    float w;

    for (int i = 0; i < simulator.particleGrid.size_x(); i++) {
        for (int j = 0; j < simulator.particleGrid.size_y(); j++) {
            std::vector<int> &particleBucket = simulator.particleGrid(i, j).particles;

            if (particleBucket.size() > 0) {
                int size = 0;
                if (particleBucket.size() > 4) { //TODO target density
                    size = 4;
                }
                else {
                    size = particleBucket.size();
                }
                for (int index = 0; index < size; index++) {
					Particle &particle = particles[particleBucket[index]];

                    v8::Local<v8::Array> vertex = v8::Array::New(isolate, 3);
                    v8::Local<v8::Array> color = v8::Array::New(isolate, 3);

                    vertex->Set(0, v8::Number::New(isolate, particle.position.x));
                    vertex->Set(1, v8::Number::New(isolate, particle.position.y));
                    vertex->Set(2, v8::Number::New(isolate, particle.position.z));

                    //Add to vertices
                    vertices->Set(particleCount, vertex);

                    //Determine color
                    alpha = 0.0001 * simulator.computeDensity(particle.position.x, particle.position.z);
                    alpha = pow(xlib::fclamp((6.0 - alpha) / 6.0, 0, 1.0), 2.0f) * 5.999;
                    alphai = (int)alpha;
                    w = alpha - alphai;
                    
                    color->Set(0, Nan::New(particleDensityColor[alphai].x * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].x * (w)));
                    color->Set(1, Nan::New(particleDensityColor[alphai].y * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].y * (w)));
                    color->Set(2, Nan::New(particleDensityColor[alphai].z * (1.0 - w) + particleDensityColor[xlib::clamp(alphai + 1, 0, 5)].z * (w)));

                    //Add to colors
                    colors->Set(particleCount, color);
                    particleCount++;
                }
            }
        }
    }

    //Create key strings
    v8::Local<v8::String> verticesStr = v8::String::NewFromUtf8(isolate, "vertices");
    v8::Local<v8::String> colorsStr = v8::String::NewFromUtf8(isolate, "colors");

    //Add to frame
    v8::Local<v8::Object> frame = v8::Object::New(isolate);
    frame->Set(context, verticesStr, vertices);
    frame->Set(context, colorsStr, colors);    

    //Set return Value
    info.GetReturnValue().Set(frame);
}

#endif