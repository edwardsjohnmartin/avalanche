/**
 * script.js
 * @fileoverview Script used to define a Vue instance for the simulation
 * @author Zackary Hall (hallzac2@isu.edu) 
 * Created: March 7th, 2017
 */

//Global vars
var socket; //Web Socket

var reqTime; // Timing
var recTime;

var lastLoadedData;
var lastLoadedSettings;

var app = new Vue({
    el: "#simulation",
    data: {
        //Simulation Settings
        initialHeight: 0,
        bounceFriction: 0,
        stickiness: 0,
        dampingForce: 0,
        turbulanceForce: 0,
        clumpingFactor: 0,
        viscosity: 0.0,
        gridSize: 128,
        framesPerSecond: 10
    },

    //Functionality
    computed: {
        //Computed property designed to ensure that frame rate is always updated
        timePerFrame: function() {
            return 1000 / this.framesPerSecond;
        }
    },
    watch: {

    },
    methods: {
        //Method designed to set a given property using the specified event
        setProperty: function(eventName, value) {
            socket.emit(eventName, {value: parseFloat(value)});
        },
        //Method designed to show a success message
        showSuccess: function(msg, title = null) {
            toastr.success(msg, title);
        },
        //Method designed to show an info message
        showInfo: function(msg, title = null) {
            toastr.info(msg, title);
        },
        //Method designed to show a warning message
        showWarning: function(msg, title = null) {
            toastr.warning(msg, title);
        },
        //Method designed to show an error message
        showError: function(msg, title = null) {
            toastr.error(msg, title);
        }, 
        //Used to reset the current simulation TODO: fix loading issue on first reset
        resetSimulation: function() {
            var selectdata = document.getElementById("select-data");
            var selectsettings = document.getElementById("select-settings");
            
            toastr.success("reseting simulation", null);
            socket.emit("reset simulation", {});  
            
            // check if a new data file has been selected
            if(lastLoadedData !== selectdata.options[selectdata.selectedIndex].dataset.datafile) {
                this.showInfo("Requesting Terrain");
                lastLoadedData = selectdata.options[selectdata.selectedIndex].dataset.datafile
                requestTerrainData();
            }
            
            // check if a new settings file has been selected
            if(lastLoadedSettings !== selectsettings.options[selectsettings.selectedIndex].dataset.settingsfile) {
                this.showInfo("Requesting Initial Settings");
                lastLoadedSettings = selectsettings.options[selectsettings.selectedIndex].dataset.settingsfile;
                requestAllSettings();
            }
        },
    },

    //Lifecycle Hooks
    created: async function() {
        //Connect to socket and retrieve the initial settings
        try {
            socket = io.connect();
            
            socket.on("receive all settings", function(data) {
                app.initialHeight = data["initialHeight"];
                app.bounceFriction = data["bounceFriction"];
                app.stickiness = data["stickyness"];
                app.dampingForce = data["dampingForce"];
                app.turbulanceForce = data["turbulanceForce"];
                app.clumpingFactor = data["clumpingFactor"];
                app.viscosity = data["viscosity"];
                //app.gridSize = data["gridSize"];
                app.framesPerSecond = data["framesPerSecond"];
                app.showSuccess("Initial Settings Loaded");
            });

            socket.on("receive terrain data", function(data) {           
                terrainData = data;
                app.showSuccess("Terrain Loaded");
                setUpWebgl();
            });

            socket.on("receive next frame", function(data) {          
                recTime = new Date();
                //console.log(recTime - reqTime);
                
                nextFrame = data;
                nextFrameReady = true;
            });

            socket.on("property updated", function(data) {
                app.showSuccess(data.message);
            });
            
            this.showInfo("Requesting Initial Settings");
            requestAllSettings();
            requestNextFrame();
            //TODO fix this, currently a sleep is needed before loading terrain or JS old space on the server overflows
            await sleep(2000);
            //TODO add message
            this.showInfo("Requesting Terrain");
            requestTerrainData();

        } catch(err) {
            this.showError("Cannot connect to server", "ERROR");
        }
    },
    mounted: function() {
        
    },
    updated: function() {

    },
    destroyed: function() {

    },
});

//Method designed to request all simulation settings
function requestAllSettings() {
    socket.emit("request all settings", {});
}

//Method designed to request the terrain data
function requestTerrainData() {
    socket.emit("request terrain data", {});
}

// Used to changed the selected data file
function changeDataFile(select) {
    //console.log(select.options[select.selectedIndex].dataset.datafile);
    socket.emit("receive data file", {value: select.options[select.selectedIndex].dataset.datafile});
}

// Used to change the selected settings file
function changeSettingsFile(select) {
    //console.log(select.options[select.selectedIndex].dataset.settingsfile);
    socket.emit("receive settings file", {value: select.options[select.selectedIndex].dataset.settingsfile});
}

// Used to drop the given number of frames
function dropFrames(steps) {
    //console.log("dropping " + Math.floor(steps) + " frames");
    socket.emit("drop frames", {value: Math.floor(steps)});
}

//Method designed to request the next frame
function requestNextFrame() {
    reqTime = new Date();
    
    nextFrameReady = false;
    socket.emit("request next frame", {});
}

//Method designed to sleep for the given time in milliseconds
function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}
