function TerrainVertex() {
	this._x;	// Left
	this._y; // Up
	this._z; // Depth

	this.textureCoordX;
	this.textureCoordY;
	this.textureCoordZ; // Not sure if this is necessary

	this.updatePosition = function(newX, newY, newZ) {
		this._x = newX;
		this._y = newY;
		this._z = newZ;
	}

	this.updateTextureCoords = function(newTextureCoordX, newTextureCoordY, newTextureCoordZ) {
		this.textureCoordX = newTextureCoordX;
		this.textureCoordY = newTextureCoordY;
		this.textureCoordZ = newTextureCoordZ;
	}

	this.height = function() {
		return this._y;
	}

	this.x = function() {
		return this._x;
	}

	this.z = function() {
		return this._z;
	}

	this.position = function() {
		return [this._x, this._y, this._z];
	}

	this.textureCoords = function() {
		return [this.textureCoordX, this.textureCoordY, this.textureCoordZ];
	}
}
