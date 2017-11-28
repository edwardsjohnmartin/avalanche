var simulation = new Simulation();
var particles = new Array();

var demFile;
var bmpFile;
var texture;

window.onload = function() {
	setupOutput();
}

function startSimulation() {
	if(!demFile) {
		alert("No DEM selected!");
		return;
	}

	if(!terrainData) {
		simulation.terrainFromDEM(demFile, loadTerrain);
	} else {
		loadTerrain();
	}
}

function setTerrain() {
	demFile = document.getElementById('demInput').files[0];
}

function setTexture() {
	var file = document.getElementById('txtInput').files[0];

	if(file) {
		var reader = new FileReader();
		reader.onload = function(e) {
			texImage.src = e.target.result;
		}
		reader.readAsDataURL(file);
	}
}

function setStartzone() {
	bmpFile = document.getElementById('bmpInput').files[0];
}

function loadTerrain() {
	if(!terrainData) {
		terrainData = simulation.getTerrainData();
	}

	var count = parseFloat(txtParticleCount.value);
	if(!count) {
		aler("Not a valid input for particle count!");
		return;
	}

	simulation.setParticleCount(count);
	simulation.startzoneFromBMP(bmpFile, loadStartzone);
}

function loadStartzone() {
	currentFrame = simulation.getParticleData();

	if(!gl) {
		setUpWebgl();
	}
}

function updateSimulation() {
	simulation.updateAllParticles();
	currentFrame = simulation.getParticleData();
}
