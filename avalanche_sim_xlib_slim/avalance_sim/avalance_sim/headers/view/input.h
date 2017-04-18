/**
* input.h
* Header file designed to provide a struct for handling input events
*/

#ifndef INPUT_H
#define INPUT_H

#include "uicontrols.h"

struct Input {

	bool keysDown[512];
	float mouseX;
	float mouseY;
	bool mouseDown[16];
	
	//Constructor
	Input() {
		mouseX = 0;
		mouseY = 0;
		memset(mouseDown, 0, sizeof(bool[16]));
		memset(keysDown, 0, sizeof(bool[512]));
	}
	
	//Method designed to handle a key down event
	void keyDownEvent(int key) {
		keysDown[key] = true;
	}

	//Method designed to handle a key up event
	void keyUpEvent(int key) {
		keysDown[key] = false;
	}

	//Method designed to handle a mouse down event
	void mouseDownEvent(int mouseBtn) {
		mouseDown[mouseBtn] = true;
	}

	//Method designed to handle a mouse down event
	void mouseUpEvent(int mouseBtn) {
		mouseDown[mouseBtn] = false;
	}

};

#endif