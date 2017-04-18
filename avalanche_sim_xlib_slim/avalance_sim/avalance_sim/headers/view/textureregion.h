/**
* textureregion.h
* Header file designed to define a texture region struct
*/

#ifndef TEXTUREREGION_H
#define TEXTUREREGION_H

#include"xlib.h"

struct TextureRegion {

	unsigned int texture;
	bool visible;
	vec4 color;
	vec3 position;
	vec2 size;
	vec2 textureBase;
	vec2 textureSize;

	//Constructor
	TextureRegion() {
		visible = true;
		color = vec4(1, 1, 1, 1);
		position = vec3(0, 0, 0);
		size = vec2(32, 32);
		texture = 0;
		textureBase = vec2(0, 0);
		textureSize = vec2(1, 1);
	}
};

#endif