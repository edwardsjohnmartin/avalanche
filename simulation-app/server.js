/**
 * server.js
 * @fileoverview Script used to create and handle an ExpressJS app with Sockets.io support
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: March 2nd, 2017
 * Edited: May 18th, 2017 by Sandro Pawlidis
 */

//Import dependencies
var express = require("express");
var app = express(); //Create express app
var server = require("http").createServer(app); //Set up the server and socket.io
var io = require("socket.io").listen(server);
var path = require("path"); //Core Module, no need to install separately
var simulationManager = require("./build/Release/simulationmanager"); //Custom C++ addon

//Global Vars
var connections = []; //List of socket connections

//View Engine
app.set("view engine", "ejs");
app.set("views", path.join(__dirname, "views"));

//Set static path, this is for static resources
app.use(express.static(path.join(__dirname, "public")));

//Listen on the env port or 3000
server.listen(process.env.port || 3000, function() {
    console.log("Server running...");
});

//Avalanche Simulation page request
app.get("/", function(req, res) {
    res.render("avalance-simulator", {});
});

//Connection
io.sockets.on("connection", function(socket) {
    //Add a connection to the list
    connections.push(socket);
    console.log("connected: %s sockets connected", connections.length);
    var datafile = "libs/simulations/avalanche-simulation/resources/simdata.txt";
    var settingsfile = "libs/simulations/avalanche-simulation/resources/simsettings.txt";
     
    startSimulation(socket.id, datafile, settingsfile);
    
    socket.on("receive data file", function(data) {
        console.log("new data file: " + data.value);
        datafile = data.value;
    });
    
    socket.on("receive settings file" , function(data) {
        console.log("new settings file: " + data.value);
        settingsfile = data.value;
    });
    
    socket.on("reset simulation", function(data) {
        simulationManager.removeSimulation(socket.id);
        startSimulation(socket.id, datafile, settingsfile);
    });
    
    socket.on("request all settings", function(data) {
        socket.emit(
            "receive all settings", 
            simulationManager.getAllSimulationSettings(socket.id)
        );
    });

    socket.on("request terrain data", function(data) {
        socket.emit(
            "receive terrain data", 
            simulationManager.getSimulationTerrainData(socket.id)
        );
    });
    
    socket.on("drop frames", function(data) {
        if(data.value && data.value > 0) {
            simulationManager.skipSimulationFrames(socket.id, data.value);
        }
    });

    socket.on("request next frame", function(data) {
        socket.emit(
            "receive next frame", 
            //simulationManager.getNextSimulationFrameFromGrid(socket.id)
            simulationManager.getNextSimulationFrame(socket.id)
        );
    });
    
    socket.on("initial height changed", function(data) {
        simulationManager.setSimulationInitialHeight(socket.id, data.value);
        socket.emit("property updated", {message: "Initial Height updated successfully"});
    });

    socket.on("bounce friction changed", function(data) {
        simulationManager.setSimulationBounceFriction(socket.id, data.value);
        socket.emit("property updated", {message: "Bounce Friction updated successfully"});
    });

    socket.on("stickyness changed", function(data) {
        simulationManager.setSimulationStickyness(socket.id, data.value);
        socket.emit("property updated", {message: "Stickiness updated successfully"});
    });

    socket.on("damping force changed", function(data) {
        simulationManager.setSimulationDampingForce(socket.id, data.value);
        socket.emit("property updated", {message: "Damping Force updated successfully"});
    });

    socket.on("turbulance force changed", function(data) {
        simulationManager.setSimulationTurbulanceForce(socket.id, data.value);
        socket.emit("property updated", {message: "Turbulance Force updated successfully"});
    });

    socket.on("clumping factor changed", function(data) {
        simulationManager.setSimulationClumpingFactor(socket.id, data.value);
        socket.emit("property updated", {message: "Clumping Factor updated successfully"});
    });

    socket.on("viscosity changed", function(data) {
        simulationManager.setSimulationViscosity(socket.id, data.value);
        socket.emit("property updated", {message: "Viscosity updated successfully"});
    });

    socket.on("frames per second changed", function(data) {
        simulationManager.setSimulationFramesPerSecond(socket.id, data.value);
        socket.emit("property updated", {message: "Frames per Second updated successfully"});
    });

    //Disconnect
    socket.on("disconnect", function(data) {
        simulationManager.removeSimulation(socket.id);
        connections.splice(connections.indexOf(socket), 1);
        console.log("disconnected: %s sockets connected", connections.length);
    });
});

// used to start a new simulation
function startSimulation(id, datafile, settingsfile) {
    if(datafile !== "" && settingsfile !== "") {
        simulationManager.addSimulation(id, datafile, settingsfile);
    }
}