var terrain; // bitmap
var particles; // array of particles

var particleGrid;

var sizeX;
var sizeY;
var cellSize;

var vertices = [];
var colors = [];
var data = {};

var gridSizeX;
var gridSizeZ;

var initialHeight = 100;
var timestep = 0.16667;
var gravity = -9.8;
var stickyness = 0.5;
var damping = 0.2;
var viscosity = 0.025;
var gridSize = 128;

var particleCount;

// Constructor
function Simulation() {
	this.terrainFromDEM = function(demFile, callback) {
		inputDEM(demFile, callback);
	}

	this.startzoneFromBMP = function(bmpFile, callback) {
		inputBMP(bmpFile, callback);
	}

	this.getTerrainData = function() {
		return getTerrainDataUtil();
	}

	this.getTerrain = function() {
		return terrain;
	}

	this.getParticleData = function() {
		return getParticleDataUtil();
	}

	this.getParticles = function() {
		return particles;
	}

    this.setParticleCount = function (count) {
        particleCount = count;
    }

	this.updateAllParticles = function() {
		updateAllParticlesUtil();
	}
}

// Update all particles in the simulation
function updateAllParticlesUtil() {
	clearGrid();

	var maxVel = 0;
	var index = -1;
	for(var i = 0; i < particles.length; i++) {
		updateParticle(i);
		var vel = particles[i].magnitude();
		if(vel > maxVel) {
			maxVel = vel;
			index = i;
		}
	}

	console.log(index + ": " + maxVel);
}

// Update a single particle, specified by the index into the particle array
function updateParticle(index) {
	var p = particles[index];

	// Add gravity
	p._velY += (gravity * timestep);

	var posX = p._x + p._velX * timestep;
	var posY = p._y + p._velY * timestep;
	var posZ = p._z + p._velZ * timestep;

	if(posX < 1 || posZ < 1 || Math.floor(posX / cellSize) >= terrain.width - 1 || Math.floor(posZ / cellSize) >= terrain.depth - 1)
		return;

	// Check for collision
	var hit = {};
	var collision = trace(p, [posX, posY, posZ], hit);

	// Compute density
	//var density = computeDensity(p._x, p._z) * 0.0001;

	//if(index === 0) console.log(particles[0]);
	// If collision process collision
	if(collision) {
		if(posY < hit.pos[1]) {
		 	p._y += 0.5 * (hit.pos[1] - posY);
		}

		// Compute normal
		var norm = computeNormal(hit.pos[0], hit.pos[2]);
		var mag = p.magnitude();

		var r = reflect([p._velX / mag, p._velY / mag, p._velZ / mag], norm);

		var hitVx = hit.pos[0] - p._x;
		var hitVy = hit.pos[1] - p._y;
		var hitVz = hit.pos[2] - p._z;

		var hitMag = Math.sqrt(hitVx * hitVx + hitVy * hitVy + hitVz * hitVz);
		var rdist = Math.abs(mag - hitMag);

		var preTime = timestep * (hitMag / mag);
		var postTime = timestep * (rdist / mag);

		mag *= 0.55;

		var vZ = r[2] * mag;
		var vX = r[0] * mag;
		var vY = r[1] * mag;

		p._x = hit.pos[0] + vX * postTime;
		p._y = hit.pos[1] + vY * postTime;
		p._z = hit.pos[2] + vZ * postTime;

		p._velX = r[0] * mag;
		p._velY = r[1] * mag;
		p._velZ = r[2] * mag;

		var height = terrain[Math.floor(p._z / cellSize)][Math.floor(p._x / cellSize)]._y + 1;

		if(p._y < height)
			p._y = height;

	} else {
		// Apply velocity to position
		p._x += p._velX * timestep;
		p._y += p._velY * timestep;
		p._z += p._velZ * timestep;

		p.velX += -p.velX * damping;
		p.velY += -p.velY * damping;
		p.velZ += -p.velZ * damping;
	}


	//registerParticleToGrid(p);
}

// Reflect the given vector over another vector
function reflect(v, norm) {
	var dot = v[0] * norm[0] + v[1] * norm[1] + v[2] * norm[2];
	var proj = norm;

	proj[0] *= 2*dot;
	proj[1] *= 2*dot;
	proj[2] *= 2*dot;

	return [v[0] - proj[0], v[1] - proj[1], v[2] - proj[2]];
}

// Compute the density at the given position
function computeDensity(posX, posZ) {
	if(posX < 1 || posZ < 1 || posX / cellSize >= terrain.width - 1 || posZ / cellSize >= terrain.depth - 1)
		return 1;

	var x = Math.floor(posX / (cellSize * cellSize));
	var z = Math.floor(posZ / (cellSize * cellSize));

	var count = particleGrid[z][x].length;

	var csize = cellSize / terrain.depth;
	return count / (csize * csize);
}

// Initialize particle grid
function initGrid() {
	gridSizeX = terrain.width * gridSize / 512;
	gridSizeZ = terrain.depth * gridSize / 512;

	particleGrid = [];
	for(var i = 0; i < gridSizeZ; i++) {
		particleGrid[i] = [];
		for(var j = 0; j < gridSizeX; j++) {
			particleGrid[i][j] = [];
		}
	}
}

// Used to clear the particle grid
function clearGrid() {
	for(var i = 0; i < gridSizeZ; i++) {
		for(var j = 0; j < gridSizeX; j++) {
			particleGrid[i][j] = [];
		}
	}
}

// NOTE: doesn't account for particles being above one another i.e. only x - z coordinates matter
// Maps a particle to the appropriate grid cell
function registerParticleToGrid(particle) {
	var x = Math.floor(particle._x * terrain.depth / (terrain.depth * cellSize));
	var z = Math.floor(particle._z * terrain.width / (terrain.width * cellSize));

	var index = particleGrid[x][z].length;
	particleGrid[x][z][index] = particle;


}

// Compute the normal of the given terrain position
function computeNormal(posX, posZ) {
	var x = Math.floor(posX / cellSize);
	var z = Math.floor(posZ / cellSize);

	if(x < 1 || z < 1 || x >= terrain.width - 1 || z >= terrain.depth - 1)
		return [0, 0, 0];

	var difX = terrain[z][x - 1]._y - terrain[z][x]._y;
	var difZ = terrain[z - 1][x]._y - terrain[z][x]._y;
	var y = cellSize;

	var magnitude = Math.sqrt(difX * difX + difZ * difZ + y * y);

	difX /= magnitude;
	difZ /= magnitude;
	y /= magnitude;

	return [difX, y, difZ];
}

// Draw a ray from start to end and check for collision by probing intermediate points
function trace(start, end, hit) {
	var startX = start._x / cellSize;
	var startZ = start._z / cellSize;

	var endX = end[0] / cellSize;
	var endZ = end[2] / cellSize;

	var difX = endX - startX;
	var difZ = endZ - startZ;

	var stepsX = Math.floor(Math.abs(difX));
	var stepsZ = Math.floor(Math.abs(difZ));

	// In case the end particle is straight below
	if(stepsX === 0 && stepsZ === 0) stepsX = stepsZ = 1;

	var signX = Math.sign(difX);
	var signZ = Math.sign(difZ);

	var slopeX = signX * (difZ) / (difX);
	var slopeZ = signZ * (difX) / (difZ);

	// In case the end particle is straight below
	if(!slopeX && !slopeZ) slopeX = slopeZ = 0;

	var steps = Math.max(stepsX, stepsZ); // Choose from which axis to probe
	var yStep = (end[1] - start._y) / steps; // Distance by which y decreases

	for(var i = 0; i < steps; i++) {
		var x, z, y;

		// Find a position between start and end to probe
		if(stepsX >= stepsZ) {
			x = Math.floor(startX) + (signX * i);
			z = Math.floor(slopeX*i + startZ);
		} else {
			x = Math.floor(slopeZ*i + startX);
			z = Math.floor(startZ) + (signZ * i);
		}

		if(x < 0 || z < 0 || x >= terrain.width - 1 || z >= terrain.depth - 1) {
			continue;
		}

		var height = terrain[z][x]._y + 1;
		var y = start._y + i * yStep;

		// Check for collision
		if(height >= y) {
			hit.pos = [x * cellSize, height, z * cellSize];
			return true;
		}
	}

	return false;
}

// Setup the array used to hold particle data for webgl
function setupParticleDataArray() {
	for(var i = 0; i < particles.length; i++) {
		vertices[i] = [0, 0, 0];
		colors[i] = [0, 0, 0];
	}
}

// Used to get the necessary data from the particles to be drawn in webgl
function getParticleDataUtil() {
	for(var i = 0; i < particles.length; i++) {
		var p = particles[i];

		vertices[i][0] = p._x;
		vertices[i][1] = p._y;
		vertices[i][2] = p._z;

		colors[i][0] = 1;
		colors[i][1] = 1;
		colors[i][2] = 1;
	}

	data.vertices = vertices;
	data.colors = colors;
	return data;
}

// Used to extract the necessary data from the terrain to be drawn in webgl
function getTerrainDataUtil() {
	var vertices = new Array();
	var textureCoords = new Array();

	var index = 0;

	for(var i = 0; i < sizeX - 1; i++) {
		for(var j = 0; j < sizeY - 1; j++) {
			var curPosition;
			var curTextureCoords;

			// Triangle 1 - Vertex 1
			curPosition = terrain[i][j].position();
			curTextureCoords = terrain[i][j].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 1 - Vertex 2
			curPosition = terrain[i + 1][j].position();
			curTextureCoords = terrain[i + 1][j].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 1 - Vertex 3
			curPosition = terrain[i][j + 1].position();
			curTextureCoords = terrain[i][j + 1].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 2 - Vertex 1
			curPosition = terrain[i][j + 1].position();
			curTextureCoords = terrain[i][j + 1].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 2 - Vertex 2
			curPosition = terrain[i + 1][j + 1].position();
			curTextureCoords = terrain[i + 1][j + 1].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 2 - Vertex 3
			curPosition = terrain[i + 1][j].position();
			curTextureCoords = terrain[i + 1][j].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];
		}
	}

	var data = {};
	data["vertices"] = vertices;
	data["textureCoordinates"] = textureCoords;

	return data;
}

// Used to create the particles of the startzone from arrayBuffer
function startzoneFromBMPUtil(bitmap) {
	particles = new Array();
	var index = 0;

  if(!particleCount) particleCount = 10000;

	var height = bitmap.header.height;
	var width = bitmap.header.width;
  var availablePixels = 0;

    // Find the number of available pixels
  for(var i = 0; i < height; i++) {
    for(var j = 0; j < width; j++) {
    	var pixel = bitmap.pixels[i][j];
      if(pixel[0] != 0 || pixel[1] != 0 || pixel[2] != 0)
      	availablePixels += 1;
      }
  }

  var count = particleCount;
  var particlesPerPixel = Math.ceil(count / availablePixels);

	for(var i = 0; i < height && count > 0; i++) {
		for(var j = 0; j < width && count > 0; j++) {
			var pixel = bitmap.pixels[i][j];
			// Determines the number of particles in a given cell by the square of rgb values
			var numParticles = Math.ceil(pixel[0] * pixel[0] +  pixel[1] * pixel[1] +  pixel[2] * pixel[2]);

      if(numParticles > 0)
      	numParticles = particlesPerPixel;

      count -= numParticles;

			for(var n = 0; n < numParticles; n++) {
				var rndX = Math.random() * cellSize;
				var rndZ = Math.random() * cellSize;

				var x = Math.floor(terrain.depth * i / width);
				var y = Math.floor(terrain.width - terrain.width * j / height);

				if(x < terrain.width && y < terrain.depth)
					particles[index++] = new Particle(y * cellSize + rndX, terrain[x][y]._y + initialHeight, x * cellSize + rndZ);
			}
		}
	}

	console.log(particles[0]);
}

// Used to create the terrain from a given DEM string
function terrainFromDEMUtil(demText) {
	terrain = new Array();
	demText = demText.split('\n');

	// Process header
	sizeX = demText.shift().split(' ')[1];
	sizeY = demText.shift().split(' ')[1];
	var cornerX = demText.shift().split(' ')[1];
	var cornerY = demText.shift().split(' ')[1];
	cellSize = demText.shift().split(' ')[1];
	var nodata = demText.shift().split(' ')[1];


	var maxHeight = 0;
	var minHeight = 999999;

	terrain.depth = sizeY;
	terrain.width = sizeX;

	// Process height map
	for(var i = 0; i < sizeY; i++) {
		var row = demText[i].split(' ');
		terrain[i] = new Array();

		for(var j = 0; j < sizeX; j++) {
			var height = row[j];

			if(height <= nodata) {
				height = 0;
			}

			if (height > maxHeight) {
				maxHeight = height;
			}

			if (height < minHeight) {
				minHeight = height;
			}

			terrain[i][j] = new TerrainVertex();

			terrain[i][j].updatePosition(j * cellSize, height, i * cellSize);
			terrain[i][j].updateTextureCoords(j / (sizeX - 1), 1 - i / (sizeY - 1), height);

		}

	}

	// TO-DO: Ask edwards what he thinks
	for (var i = 0; i < sizeY; i++) {
		for (var j = 0; j < sizeX; j++) {
			var newPos = terrain[i][j].position();
			newPos[1] = newPos[1] - minHeight;

			terrain[i][j].updatePosition(newPos[0], newPos[1], newPos[2]);

			var newTextureCoords = terrain[i][j].textureCoords();
			newTextureCoords[2] = (newTextureCoords[2] - minHeight) / maxHeight;

			terrain[i][j].updateTextureCoords(newTextureCoords[0], newTextureCoords[1], newTextureCoords[2]);
		}
	}
}

// Used to read the file as an array buffer
function inputBMP(bmpFile, callback) {
	if(bmpFile) {
		var reader = new FileReader();

		reader.onload = function(e) {
			var bitmap = importFromBMP(reader.result);
			startzoneFromBMPUtil(bitmap);
			setupParticleDataArray();
			callback();
		}

		reader.readAsArrayBuffer(bmpFile);
	}
}

// Used to extract the text from a dem file
function inputDEM(demFile, callback) {
	if(demFile) {
		var reader = new FileReader();

		reader.onload = function(e) {
			terrainFromDEMUtil(reader.result);
			initGrid();
			callback();
		}

		reader.readAsText(demFile);
	}
}

// Create a bitmap object from the given array buffer
function importFromBMP(buffer) {
	var dataview = new DataView(buffer);
	var bitmap = {};

	// Process header
	bitmap.header = {};
	bitmap.header.filesize = dataview.getUint32(2, true); // Get the next 4 bytes specying the file size
	bitmap.header.startOffset = dataview.getUint32(10, true); // Offset of the actual pixel data
	bitmap.header.headersize = dataview.getUint32(14, true); // Size of the header
	bitmap.header.width = dataview.getUint32(18, true); // Bitmap width in pixels
	bitmap.header.height = dataview.getUint32(22, true); // Bitmap height in pixels
	bitmap.header.bpp = dataview.getUint16(28, true); // Bits per Pixel
	bitmap.header.imagesize = dataview.getUint32(34, true); // Image size

	// Read pixel data
	var height = bitmap.header.height;
	var width = bitmap.header.width;
	var bytesPerPixel = bitmap.header.bpp / 8.0;


	var pixels = new Array(height);
	var index = 0;

	var data = new Uint8Array(buffer, bitmap.header.startOffset);
	var rowsize = 4 * Math.ceil(bitmap.header.bpp * width / 32.0);

	for(var i = 0; i < height; i++) {
	 	pixels[i] = new Array(width);
	 	for(var j = 0; j < width; j++) {
	 		var dataIndex = i*rowsize + j * bytesPerPixel;

			pixels[i][j] = new Array(bytesPerPixel);

			if(bytesPerPixel === 3) {
				pixels[i][j][0] = data[dataIndex + 2] / 255.0 // r value
				pixels[i][j][1] = data[dataIndex + 1] / 255.0// g value
				pixels[i][j][2] = data[dataIndex] / 255.0 // b value
			}

			if(bytesPerPixel === 4) {
				pixels[i][j][0] = data[dataIndex + 3] / 255.0 // r value
				pixels[i][j][1] = data[dataIndex + 2] / 255.0// g value
				pixels[i][j][2] = data[dataIndex + 1] / 255.0 // b value
				pixels[i][j][3] = data[dataIndex] / 255.0 // alpha value
			}
		}
 	}

	bitmap.pixels = pixels;
	return bitmap;
}
