/**
* inputkeymapping.h
* Header file designed to provide a struct for input key handling
*/

#ifndef INPUTKEYMAPPING_H
#define INPUTKEYMAPPING_H

//TODO determine how these controls really need to work. The camera controls seem like they need work.
struct InputKeyMapping {

	//Maps keyboard codes to actions
	int moveForwardKey;
	int moveBackwardKey;
	int moveLeftKey;
	int moveRightKey;
	int moveUpKey;
	int moveDownKey;
	int rotateUpKey;
	int rotateDownKey;
	int rotateLeftKey;
	int rotateRightKey;

	//Constructor
	InputKeyMapping() {
		moveForwardKey = 119;	//W
		moveLeftKey = 97;		//A
		moveBackwardKey = 115;	//S
		moveRightKey = 100;		//D

		moveUpKey = 114;		//R
		moveDownKey = 102;		//F

		rotateUpKey = 273;		//Up Arrow
		rotateLeftKey = 276;	//Left Arrow
		rotateDownKey = 274;    //Down Arrow
		rotateRightKey = 275;   //Right Arrow
	}
};

#endif