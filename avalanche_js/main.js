var simulation = new Simulation();
var particles = new Array();

var texture;

function startSimulation() {
	console.log(simulation.getParticles()[0]);
	if(terrainData && currentFrame) {
		setUpWebgl();
	}
	else {
		alert("No DEM or BMP selected!");
	}
}

function setTerrain() {
	var demFile = document.getElementById('demInput').files[0];
	simulation.terrainFromDEM(demFile, loadTerrain);
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

function loadTerrain() {
	terrainData = simulation.getTerrainData();
}

function setStartzone() {
    var count = parseFloat(txtParticleCount.value);
    if(!count) {
        aler("Not a valid input for particle count!");
        return;
    }

  simulation.setParticleCount(count);
	var bmpFile = document.getElementById('bmpInput').files[0];
	simulation.startzoneFromBMP(bmpFile, loadStartzone);
}

function loadStartzone() {
	currentFrame = simulation.getParticleData();
}

function updateSimulation() {
	simulation.updateAllParticles();
	currentFrame = simulation.getParticleData();
}
