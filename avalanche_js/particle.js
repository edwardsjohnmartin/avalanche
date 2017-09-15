function Particle(x, y, z) {
	this._x = x;
	this._y = y;
	this._z = z;

	this._velX = 0;
	this._velY = 0;
	this._velZ = 0;

	// Calculate the magnitude of this particle
	this.magnitude = function() {
		var x = this._velX;
		var y = this._velY;
		var z = this._velZ;

		return Math.sqrt(x*x + y*y + z*z);
	}

	// Update the velocity of this particle
	this.updateVelocity = function(newX, newY, newZ) {
		this._velX = newX;
		this._velY = newY;
		this._velZ = newZ;
	}

	// Scale the velocity by the given scalar
	this.scaleVelocity = function(scalar) {
		this._velX *= scalar;
		this._velY *= scalar;
		this._velZ *= scalar;
	}
}
