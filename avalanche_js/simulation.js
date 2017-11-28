var terrain; // bitmap
var particles; // array of particles
var particleCount;

var particleGrid;
var particleGridComplete;

var particleVelGrid;
var particleVelGridComplete;

var sizeX;
var sizeY;
var cellSize;

var vertices = [];
var colors = [];
var data = {};

var gridSizeX;
var gridSizeZ;

var timestep = 0.16667;
var maxSteps = 3000;
var stepCount = 0;

var gravity = -9.8;
var initialHeight = 25;
var restitution = 0.6;
var clumpingFactor = 0.3;
var viscosity = 0.25;
var turbulance = 0.2;
var stickyness = 0.5;
var damping = 0.02;
var gridSize = 128;

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
	if(stepCount >= maxSteps)
		return;

	stepCount++;
	clearGrid();

	for(var i = 0; i < particles.length; i++) {
		updateParticle(i);
	}

	for(var i = 0; i < gridSizeZ; i++) {
		for(var j = 0; j < gridSizeX; j++) {
			var count = particleGrid[i][j];
			particleGridComplete[i][j] = count;

			if(count <= 0) count = 1;

			var v = particleVelGrid[i][j];
			var u = particleVelGridComplete[i][j];

			u.x = v.x / count;
			u.y = v.y / count;
			u.z = v.z / count;
		}
	}
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

	var hit = {};
	var collision = trace(p, [posX, posY, posZ], hit);

	if(collision) {
		if(posY < hit.pos[1]) {
		 	p._y += 0.5 * (hit.pos[1] - posY);
		}

		var norm = computeNormal(hit.pos[0], hit.pos[2]);
		var mag = p.magnitude();

		var r = reflect([p._velX / mag, p._velY / mag, p._velZ / mag], norm);

		var hitVx = hit.pos[0] - p._x;
		var hitVy = hit.pos[1] - p._y;
		var hitVz = hit.pos[2] - p._z;

		// Compute distance to collision point
		var hitMag = Math.sqrt(hitVx * hitVx + hitVy * hitVy + hitVz * hitVz);
		var rdist = Math.abs(mag - hitMag);

		// Divide timestep over total distance
		var preTime = timestep * (hitMag / mag);
		var postTime = timestep * (rdist / mag);

		mag *= restitution;

		var vX = r[0] * mag;
		var vY = r[1] * mag;
		var vZ = r[2] * mag;

		p._x = hit.pos[0] + vX * postTime;
		p._y = hit.pos[1] + vY * postTime;
		p._z = hit.pos[2] + vZ * postTime;

		var density = computeDensity(p._x, p._z);
		var gridVelocity = getGridVelocity(p._x, p._z);
		var count = getGridCount(p._x, p._z);
		if(count <= 0 ) count = 1;

		p._velX = r[0] * mag + (Math.random() - 0.5) * density * turbulance;
		p._velY = r[1] * mag;
		p._velZ = r[2] * mag + (Math.random() - 0.5) * density * turbulance;

		p._velX = (p._velX - gridVelocity.x * (viscosity / count)) * (1 - clumpingFactor) + gridVelocity.x * clumpingFactor;
		p._velY = (p._velY - gridVelocity.y * (viscosity / count)) * (1 - clumpingFactor) + gridVelocity.y * clumpingFactor;
		p._velZ = (p._velZ - gridVelocity.z * (viscosity / count)) * (1 - clumpingFactor) + gridVelocity.z * clumpingFactor;

		var hX = clamp(Math.floor(p._x / cellSize), 0, terrain.width - 1);
		var hZ = clamp(Math.floor(p._z / cellSize), 0, terrain.depth - 1);
		var height = terrain[hZ][hX]._y + 1;

		if(p._y < height)
			p._y = height;

	} else {
		p._x += p._velX * timestep;
		p._y += p._velY * timestep;
		p._z += p._velZ * timestep;

		p.velX += -p.velX * damping;
		p.velY += -p.velY * damping;
		p.velZ += -p.velZ * damping;
	}

	registerParticleToGrid(p);
}

// ------------------------------------------------------------
// GRID MECHANICS
// ------------------------------------------------------------

// Initialize particle grid
function initGrid() {
	gridSizeX = terrain.width * gridSize / 512;
	gridSizeZ = terrain.depth * gridSize / 512;

	particleGrid = [];
	particleGridComplete = [];

	particleVelGrid = [];
	particleVelGridComplete = [];

	for(var i = 0; i < gridSizeZ; i++) {
		particleGrid[i] = [];
		particleGridComplete[i] = [];

		particleVelGrid[i] = [];
		particleVelGridComplete[i] = [];

		for(var j = 0; j < gridSizeX; j++) {
			particleGrid[i][j] = 0;
			particleGridComplete[i][j] = 0;

			var v = {};
			v.x = v.y = v.z = 0;
			particleVelGrid[i][j] = v;

			var u = {};
			u.x = u.y = u.z = 0;
			particleVelGridComplete[i][j] = u;
		}
	}
}

// Used to clear the particle grid
function clearGrid() {
	for(var i = 0; i < gridSizeZ; i++) {
		for(var j = 0; j < gridSizeX; j++) {
			particleGrid[i][j] = 0;

			var v = particleVelGrid[i][j];
			v.x = v.y = v.z = 0;
		}
	}
}

// Maps a particle to the appropriate grid cell
function registerParticleToGrid(particle) {
	var x = particle._x * gridSizeX / (terrain.width * cellSize);
	var z = particle._z * gridSizeZ / (terrain.depth * cellSize);

	var rX = Math.floor(x);
	var rZ = Math.floor(z);

	var addX = (x - rX > 0.5) ? rX + 1 : rX - 1;
	var addZ = (z - rZ > 0.5) ? rZ + 1 : rZ - 1;

	addX = clamp(addX, 0, gridSizeX - 1);
	addZ = clamp(addZ, 0, gridSizeZ - 1);

	rX = clamp(rX, 0, gridSizeX - 1);
	rZ = clamp(rZ, 0, gridSizeZ - 1);

	particleGrid[rZ][rX] += 1;
	particleGrid[rZ][addX] += 1;
	particleGrid[addZ][rX] += 1;
	particleGrid[addZ][addX] += 1;

	var v1 = particleVelGrid[rZ][rX];
	v1.x += particle._velX;
	v1.y += particle._velY;
	v1.z += particle._velZ;

	var v2 = particleVelGrid[rZ][addX];
	v2.x += particle._velX;
	v2.y += particle._velY;
	v2.z += particle._velZ;

	var v3 = particleVelGrid[addZ][rX];
	v3.x += particle._velX;
	v3.y += particle._velY;
	v3.z += particle._velZ;

	var v4 = particleVelGrid[addZ][addX];
	v4.x += particle._velX;
	v4.y += particle._velY;
	v4.z += particle._velZ;
}

// Get the velocity of the corresponding grid cell
function getGridVelocity(posX, posZ) {
	var x = posX * gridSizeX / (terrain.width * cellSize);
	var z = posZ * gridSizeZ / (terrain.depth * cellSize);

	x = clamp(Math.floor(x), 0, gridSizeX - 1);
	z = clamp(Math.floor(z), 0, gridSizeZ - 1);

	return particleVelGridComplete[z][x];
}

// Get the numer of particles in the corresponding grid cell
function getGridCount(posX, posZ) {
	var x = posX * gridSizeX / (terrain.width * cellSize);
	var z = posZ * gridSizeZ / (terrain.depth * cellSize);

	x = clamp(Math.floor(x), 0, gridSizeX - 1);
	z = clamp(Math.floor(z), 0, gridSizeZ - 1);

	return particleGridComplete[z][x];
}

// Compute the density in a given gridCell
function computeDensity(posX, posZ) {
	var x = posX * gridSizeX / (terrain.width * cellSize);
	var z = posZ * gridSizeZ / (terrain.depth * cellSize);

	x = clamp(Math.floor(x), 0, gridSizeX - 1);
	z = clamp(Math.floor(z), 0, gridSizeZ - 1);

	var csize = cellSize / gridSizeZ;
	return particleGridComplete[z][x] / (csize * csize) * 0.0001;
}

// ------------------------------------------------------------
// COLLISION
// ------------------------------------------------------------

// Reflect the given vector over another vector
function reflect(v, norm) {
	var dot = v[0] * norm[0] + v[1] * norm[1] + v[2] * norm[2];
	var proj = norm;

	proj[0] *= 2*dot;
	proj[1] *= 2*dot;
	proj[2] *= 2*dot;

	return [v[0] - proj[0], v[1] - proj[1], v[2] - proj[2]];
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


// ------------------------------------------------------------
// UTIL
// ------------------------------------------------------------

// Ensure value is between min and max
function clamp(value, min, max) {
	if(value > max) return max;
	if(value < min) return min;
	return value;
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
	var normals = new Array();

	var index = 0;

	for(var i = 0; i < sizeX - 1; i++) {
		for(var j = 0; j < sizeY - 1; j++) {
			var curPosition;
			var curTextureCoords;
			var u, v, v1, v2, v3;
			var normal = {};

			// Triangle 1 - Vertex 2
			v1 = curPosition = terrain[i + 1][j].position();
			curTextureCoords = terrain[i + 1][j].textureCoords();

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];
			//normals.push(computeNormal(i + 1, j));

			// Triangle 1 - Vertex 1
			v2 = curPosition = terrain[i][j].position();
			curTextureCoords = terrain[i][j].textureCoords();
			//normals.push(computeNormal(i, j));

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];


			// Triangle 1 - Vertex 3
			v3 = curPosition = terrain[i][j + 1].position();
			curTextureCoords = terrain[i][j + 1].textureCoords();
			//normals.push(computeNormal(i, j + 1));


			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Compute triangle normal
			u = subtract(v2, v1);
			v = subtract(v3, v1);
			normal.x = (u[1] * v[2]) - (u[2] * v[1]);
			normal.y = (u[2] * v[0]) - (u[0] * v[2]);
			normal.z = (u[0] * v[1]) - (u[1] * v[0]);
			normal = normalize(vec3(normal.x, normal.y, normal.z));
			normals.push([normal[0], normal[1], normal[2]]);
			normals.push([normal[0], normal[1], normal[2]]);
			normals.push([normal[0], normal[1], normal[2]]);

			// Triangle 2 - Vertex 1
			v1 = curPosition = terrain[i][j + 1].position();
			curTextureCoords = terrain[i][j + 1].textureCoords();
			//normals.push(computeNormal(i, j + 1));

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 2 - Vertex 2
			v2 = curPosition = terrain[i + 1][j + 1].position();
			curTextureCoords = terrain[i + 1][j + 1].textureCoords();
			//normals.push(computeNormal(i + 1, j + 1));

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// Triangle 2 - Vertex 3
			v3 = curPosition = terrain[i + 1][j].position();
			curTextureCoords = terrain[i + 1][j].textureCoords();
			//normals.push(computeNormal(i + 1, j));

			vertices[index] = curPosition;
			textureCoords[index++] = [curTextureCoords[0], curTextureCoords[1]];

			// // Compute triangle normal
			u = subtract(v2, v1);
			v = subtract(v3, v1);
			normal.x = (u[1] * v[2]) - (u[2] * v[1]);
			normal.y = (u[2] * v[0]) - (u[0] * v[2]);
			normal.z = (u[0] * v[1]) - (u[1] * v[0]);
			normal = normalize(vec3(normal.x, normal.y, normal.z));
			normals.push([normal[0], normal[1], normal[2]]);
			normals.push([normal[0], normal[1], normal[2]]);
			normals.push([normal[0], normal[1], normal[2]]);
		}
	}

	var data = {};
	data.vertices = vertices;
	data.textureCoordinates = textureCoords;
	data.normals = normals;

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
