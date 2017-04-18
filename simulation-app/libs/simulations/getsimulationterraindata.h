/**
 * getsimulationterraindata.h
 * @fileoverview .h file designed to provide a method to get the terrain data from a simulation
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: April 4th, 2017
 */

#ifndef GETSIMULATIONTERRAINDATA_H
#define GETSIMULATIONTERRAINDATA_H

//Method designed to get terrain vertices and texture coordinates
void getSimulationTerrainData(const Nan::FunctionCallbackInfo<v8::Value> &info) {
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

    Terrain &terrain = simulations[id].terrain;

    v8::Local<v8::Array> vertices = v8::Array::New(isolate, 0);
    v8::Local<v8::Array> textureCoordinates = v8::Array::New(isolate, 0);
    int nextAddIndex = 0;

    v8::Local<v8::Array> vertex;
    v8::Local<v8::Array> textureCoordinate;

    //Generate terrain vertices
    for (int i = 0; i < terrain.verts.size_x() - 1; i++) {
        for (int j = 0; j < terrain.verts.size_y() - 1; j++) {
            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);
            
            //Triangle 1 - Vertex 1
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i, j).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i, j).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i, j).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i, j).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i, j).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;

            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);

            //Triangle 1 - Vertex 2
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i + 1, j).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;

            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);

            //Triangle 1 - Vertex 3
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i, j + 1).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i, j + 1).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i, j + 1).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i, j + 1).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i, j + 1).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;

            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);

            //Triangle 2 - Vertex 1
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i, j + 1).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i, j + 1).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i, j + 1).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i, j + 1).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i, j + 1).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;

            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);

            //Triangle 2 - Vertex 2
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j + 1).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j + 1).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i + 1, j + 1).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j + 1).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j + 1).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;

            vertex = v8::Array::New(isolate, 3);
            textureCoordinate = v8::Array::New(isolate, 2);

            //Triangle 2 - Vertex 3
            vertex->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j).position.x));
            vertex->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j).position.y));
            vertex->Set(2, v8::Number::New(isolate, terrain.verts(i + 1, j).position.z));

            textureCoordinate->Set(0, v8::Number::New(isolate, terrain.verts(i + 1, j).texcoords.x));
            textureCoordinate->Set(1, v8::Number::New(isolate, terrain.verts(i + 1, j).texcoords.y));

            vertices->Set(nextAddIndex, vertex);
            textureCoordinates->Set(nextAddIndex, textureCoordinate);
            nextAddIndex++;
        }
    }

    //Create key strings
    v8::Local<v8::String> verticesStr = v8::String::NewFromUtf8(isolate, "vertices");
    v8::Local<v8::String> textureCoordinatesStr = v8::String::NewFromUtf8(isolate, "textureCoordinates");

    //Add to frame
    v8::Local<v8::Object> terrainData = v8::Object::New(isolate);
    terrainData->Set(context, verticesStr, vertices);
    terrainData->Set(context, textureCoordinatesStr, textureCoordinates);    

    //Set return Value
    info.GetReturnValue().Set(terrainData);
}

#endif