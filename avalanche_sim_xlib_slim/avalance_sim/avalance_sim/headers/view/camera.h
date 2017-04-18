/**
* camera.h
* Header files designed to provide a camera struct
*/

#ifndef CAMERA_H
#define CAMERA_H

#include "xlib.h"

struct Camera {

	mat4 transform;
	float fov;
	float nearZ;
	float farZ;

	//Constructor
	Camera() {
		transform.makeIdentity();
		fov = 60;
		nearZ = 0.1;
		farZ = 10000;
	}

	//Method designed to return the right axis
	vec3 getRightAxis() {
		return vec3(transform[0]);
	}

	//Method designed to return the up axis
	vec3 getUpAxis() {
		return vec3(transform[1]);
	}

	//Method designed to return the forward axis
	vec3 getForwardAxis() {
		return vec3(transform[2]);
	}

	//Method designed to return the position
	vec3 getPosition() {
		return vec3(transform[3]);
	}

	//Method designed to set the position
	void setPosition(vec3 loc) {
		transform.setCol(3, vec4(loc.x, loc.y, loc.z, 1));
	}

	//Method designed to 
	void move(vec3 direction) {
		vec3 pos(transform[3]);
		pos += direction;
		setPosition(pos);
	}

	//Method designed to assign where the camera should look
	void lookAt(vec3 target) {
		vec3 forward(target - vec3(transform[3]));
		forward.normalize();
		vec3 up(0, 1, 0);
		vec3 right = forward ^ up;
		up = -(forward ^ right);
		up.normalize();
		forward.normalize();
		right.normalize();
		transform.setCol(2, vec4(-forward));
		transform.setCol(1, vec4(up));
		transform.setCol(0, vec4(right));

	}
};

#endif