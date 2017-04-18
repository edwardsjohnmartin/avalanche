/**
* uicontrols.h
* Header file designed to define structs to be used for UI control
*/

#ifndef UICONTROLS_H
#define UICONTROLS_H

#include "sdl.h"
#include "xlib.h"
#include <gl/gl.h>
#include <gl/glu.h>
#include "textureregion.h"

bool checkBoundingBox(vec2 min, vec2 max, float x, float y) {
	if (x > max.x || x < min.x || y > max.y || y < min.y) {
		return false;
	}
	return true;
}

/**
* UIControl struct
* Struct designed to provide a default 
*/
struct UIControl {

	string name;
	vec3 position;
	float width;
	float height;
	bool selected;
	bool enabled;
	bool mouseCurrentlyOver;
	bool visible;
	bool clickDown;
	TextureRegion background;
	vector<TextureRegion> textures;
	unsigned int defaultTexture;

	//Constructor
	UIControl() {
		name = "unnamed";
		position = vec3(0,0,0);
		width = 32;
		height = 32;
		selected = false;
		enabled = true;
		mouseCurrentlyOver = false;
		clickDown = false;
		background.visible = false;

		ximage texImgTmp;
	
		//Open .bmp containing all the UI controls
		texImgTmp.importFrom_BMP("resources/uicontrols.bmp");
		ximage teximg(texImgTmp.width(), texImgTmp.height(), 1, XIMAGE_FORMAT_RGBA32);

		for (int i = 0; i < teximg.width(); i++) {
			for (int j = 0; j < teximg.height(); j++) {
				if (texImgTmp(i,j)[0] == 0 && texImgTmp(i,j)[1] == 0 && texImgTmp(i,j)[2] == (char)255) {
					teximg(i,j)[0] = 0;
					teximg(i,j)[1] = 0;
					teximg(i,j)[2] = 0;
					teximg(i,j)[3] = 0;
				} else {
					teximg(i,j)[0] = texImgTmp(i,j)[0];
					teximg(i,j)[1] = texImgTmp(i,j)[1];
					teximg(i,j)[2] = texImgTmp(i,j)[2];
					teximg(i,j)[3] = 255;
				}
			}
		}

		//unsigned int texIndex = 0;
		defaultTexture = 0;
		glGenTextures(1,&defaultTexture);
		glBindTexture(GL_TEXTURE_2D, defaultTexture);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,teximg.width(),teximg.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,teximg(0,0));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	//Method designed to handle various mouse events
	void updateMouse(float x, float y) {
		if (checkBoundingBox(position.xy(),position.xy() + vec2(width,height),x,y)) {
			if (!mouseCurrentlyOver) {
				mouseCurrentlyOver = true;
				onMouseEnterEvent(x,y);
			}
		} else {
			if (mouseCurrentlyOver) {
				mouseCurrentlyOver = false;
				onMouseExitEvent(x,y);
			}
		}
		if (clickDown) {
			mouseHold(x,y);
			mouseDrag(x,y);
		}
	}

	//Method designed to handle a mouse down event
	void mouseDown(float x, float y) {
		if (checkBoundingBox(position.xy(),position.xy() + vec2(width,height),x,y)) {
			onMouseDownEvent(x, y);
			clickDown = true;
		}
	}

	//Method designed to handle a mouse up event
	void mouseUp(float x, float y) {
		if (checkBoundingBox(position.xy(),position.xy() + vec2(width,height),x,y)) {
			onMouseUpEvent(x, y);
		}
		clickDown = false;
	}

	//Method designed to handle a mouse drag event
	void mouseDrag(float x, float y) {
		if (checkBoundingBox(position.xy(),position.xy() + vec2(width,height),x,y)) {
			onMouseDragEvent(x, y);
		}
	}

	//Method designed to add a mouse hold event
	void mouseHold(float x, float y) {
		if (checkBoundingBox(position.xy(),position.xy() + vec2(width,height),x,y) || clickDown) {
			onMouseHoldEvent(x, y);
		}
	}

	//Virtual methods to be overrided
	virtual void onMouseDragEvent(float x, float y) {}
	virtual void onMouseHoldEvent(float x, float y) {}
	virtual void onMouseDownEvent(float x, float y) {}
	virtual void onMouseUpEvent(float x, float y) {}
	virtual void onMouseEnterEvent(float x, float y) {}
	virtual void onMouseExitEvent(float x, float y) {}
};

/**
* UIPanel struct
* Struct designed to control and display a panel on the UI
*/
struct UIPanel: public UIControl {

	string title;
	int panelTopLeft;
	int panelTopMiddle;
	int panelTopRight;

	int panelMidLeft;
	int panelMidMiddle;
	int panelMidRight;

	int panelBotLeft;
	int panelBotMiddle;
	int panelBotRight;

	int minimizeBtn;
	int maximizeBtn;

	TextureRegion cursorTexture;

	bool maximized;
	vector<UIControl*> childControls;
	
	//Constructor
	UIPanel()  {
		background.visible = false;
		width = 270;
		height = 200;
		maximized = true;
		createTextures();
		childControls.push_back(this);
	}

	//Method designed to resize the panel
	void setSize(int newWidth, int newHeight) {
		width = newWidth;
		height = newHeight;
		createTextures();
	}

	//Method designed to minimize the panel
	void minimize() {
		if (!maximized) return;
		maximized = false;
		textures[minimizeBtn].visible = false;
		textures[maximizeBtn].visible = true;

		textures[panelMidLeft].visible = false;
		textures[panelMidMiddle].visible = false;
		textures[panelMidRight].visible = false;
		
		textures[panelBotLeft].visible = false;
		textures[panelBotMiddle].visible = false;
		textures[panelBotRight].visible = false;
	}

	//Method designed to maximize the panel
	void maximize() {
		if (maximized) return;
		maximized = true;
		textures[minimizeBtn].visible = true;
		textures[maximizeBtn].visible = false;

		textures[panelMidLeft].visible = true;
		textures[panelMidMiddle].visible = true;
		textures[panelMidRight].visible = true;
		
		textures[panelBotLeft].visible = true;
		textures[panelBotMiddle].visible = true;
		textures[panelBotRight].visible = true;
	}

	//Method designed to create textures for the panel
	void createTextures() {
		textures.clear();

		//TopLeft:
		panelTopLeft = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(18/256.0,19/256.0);
		textures.back().textureBase = vec2(0,1.0 - 37/256.0);
		textures.back().size = vec2(18,19);
		textures.back().position = vec3(0,0,0);

		//TopMiddle:
		panelTopMiddle = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(28/256.0,19/256.0);
		textures.back().textureBase = vec2(18/256.0,1.0 - 37/256.0);
		textures.back().size = vec2(width - 36,19);
		textures.back().position = vec3(18,0,0);

		//TopRight:
		panelTopRight = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(18/256.0,19/256.0);
		textures.back().textureBase = vec2(46/256.0,1.0 - 37/256.0);
		textures.back().size = vec2(18,19);
		textures.back().position = vec3(width - 18,0,0);

		//MidLeft:
		panelMidLeft = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(6/256.0,19/256.0);
		textures.back().textureBase = vec2(0,1.0 - 58/256.0);
		textures.back().size = vec2(6,height-19);
		textures.back().position = vec3(0,19-height,0);
		
		//MidMiddle:
		panelMidMiddle = textures.size();
		textures.push_back(TextureRegion());
		textures.back().color = vec4(1,1,1,0.5);
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(28/256.0,19/256.0);
		textures.back().textureBase = vec2(18/256.0,1.0 - 58/256.0);
		textures.back().size = vec2(width - 12,height-19);
		textures.back().position = vec3(6,19-height,0);

		//MidRight:
		panelMidRight = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(6/256.0,19/256.0);
		textures.back().textureBase = vec2(58/256.0,1.0 - 58/256.0);
		textures.back().size = vec2(6,height-19);
		textures.back().position = vec3(width - 6,19-height,0);

		//BotLeft:
		panelBotLeft = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(5/256.0,6/256.0);
		textures.back().textureBase = vec2(0,1.0 - 66/256.0);
		textures.back().size = vec2(5,6);
		textures.back().position = vec3(0,19-height-6,0);
		
		//BotMiddle:
		panelBotMiddle = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(28/256.0,6/256.0);
		textures.back().textureBase = vec2(18/256.0,1.0 - 66/256.0);
		textures.back().size = vec2(width - 10,6);
		textures.back().position = vec3(5,19-height-6,0);

		//BotRight:
		panelBotRight = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(5/256.0,6/256.0);
		textures.back().textureBase = vec2(59/256.0,1.0 - 66/256.0);
		textures.back().size = vec2(5,6);
		textures.back().position = vec3(width - 5,19-height-6,0);

		//Minimize Button
		minimizeBtn = textures.size();
		textures.push_back(TextureRegion());
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(14/256.0,13/256.0);
		textures.back().textureBase = vec2(64/256.0,1.0 - 31/256.0);
		textures.back().size = vec2(14,13);
		textures.back().position = vec3(width - 18,3,0);

		//Maximize Button
		maximizeBtn = textures.size();
		textures.push_back(TextureRegion());
		textures.back().visible = false;
		textures.back().texture = defaultTexture;
		textures.back().textureSize = vec2(14/256.0,13/256.0);
		textures.back().textureBase = vec2(78/256.0,1.0 - 31/256.0);
		textures.back().size = vec2(14,13);
		textures.back().position = vec3(width - 18,3,0);

		//Cursor
		cursorTexture.size = vec2(17,18);
		cursorTexture.textureSize = vec2(17/256.0,18/256.0);
		cursorTexture.textureBase = vec2(136/256.0,1.0-18/256.0);
		cursorTexture.texture = defaultTexture;
	}

	//Method designed to handle mouse events
	void updateMouse(float x, float y) {
		cursorTexture.position = vec3(x,y-18,0);
		if (!maximized) return;
		for (int i = 1; i < childControls.size(); i++) {
			childControls[i]->updateMouse(x,y);
		}
	}

	//Method designed to handle mouse down events
	void mouseDown(float x, float y) {
		if (!maximized) return;
		for (int i = 1; i < childControls.size(); i++) {
			childControls[i]->mouseDown(x,y);
		}
	}

	//Method designed to handle mouse up events
	void mouseUp(float x, float y) {
		if (checkBoundingBox(textures[minimizeBtn].position.xy()+position.xy(),textures[minimizeBtn].position.xy()+position.xy()+textures[minimizeBtn].size,x,y)) {
			if (maximized) {
				minimize();
			} else {
				maximize();
			}
		}
		if (!maximized) return;
		for (int i = 1; i < childControls.size(); i++) {
			childControls[i]->mouseUp(x,y);
		}
	}

	//Method designed to handle mouse drag events
	void mouseDrag(float x, float y) {
		if (!maximized) return;
		for (int i = 1; i < childControls.size(); i++) {
			childControls[i]->mouseDrag(x,y);
		}
	}

	//Method designed to handle mouse hold events
	void mouseHold(float x, float y) {
		if (!maximized) return;
		for (int i = 1; i < childControls.size(); i++) {
			childControls[i]->mouseHold(x,y);
		}
	}
};

/**
* UISlider
* Struct designed to provide control for a slider on the UI
*/
struct UISlider : public UIControl {

	float sliderValue;
	float sliderMinValue;
	float sliderMaxValue;
	float scrollAccel;
	bool sliding;

	int slideBtn;
	int scrollLeftBtn;
	int scrollRightBtn;

	bool changed;

	//Constructor
	UISlider() {
		width = 180;
		height = 18;
		scrollAccel = 0.0;
		background.color = vec4(0,0,0,0.25);
		background.size = vec2(width-2,height-2);
		background.position = vec3(1,1,0);
		background.texture = 0;
		background.visible = true;
		sliding = false;
		sliderValue = 0;
		sliderMinValue = 0;
		sliderMaxValue = 10.0;
		changed = false;
		createTextures();
	}

	//Method designed to determine if the poll has changed or not
	bool pollChanged() {	
		if (changed) {
			changed = false;
			return true;
		}
		return false;
	}

	//Method designed to set the value of the slider
	void setValue(float newValue) {
		sliderValue = newValue;
		updateSlider();
		changed = true;
	}
	
	//Method designed to create textures for the slider components
	void createTextures() {
		textures.clear();

		scrollLeftBtn = textures.size();
		textures.push_back(TextureRegion());
		textures.back().size = vec2(18,18);
		textures.back().textureSize = vec2(18.0/256.0,18.0/256.0);
		textures.back().textureBase = vec2(0,1.0 - 18.0/256.0);
		textures.back().texture = defaultTexture;
		textures.back().position = vec3(0,0,0);

		slideBtn = textures.size();
		textures.push_back(TextureRegion());
		textures.back().size = vec2(28,18);
		textures.back().textureSize = vec2(28.0/256.0,18.0/256.0);
		textures.back().textureBase = vec2(18.0/256.0,1.0 - 18.0/256.0);
		textures.back().texture = defaultTexture;
		textures.back().position = vec3(18,0,0);

		scrollRightBtn = textures.size();
		textures.push_back(TextureRegion());
		textures.back().size = vec2(18,18);
		textures.back().textureSize = vec2(18.0/256.0,18.0/256.0);
		textures.back().textureBase = vec2(46.0/256.0,1.0 - 18.0/256.0);
		textures.back().texture = defaultTexture;
		textures.back().position = vec3(width - 18,0,0);
	}

	//Method designed to handle mouse up events
	void onMouseUpEvent(float x, float y) {
		scrollAccel = 0.0;
		
		if (sliding) {
			changed = true;
			sliding = false;
		}
	}

	//Method designed to handle mouse hold events
	void onMouseHoldEvent(float x, float y) {
		if (checkBoundingBox(position.xy(), position.xy() + vec2(18,18), x, y) ) {
			sliderValue -= scrollAccel*(sliderMaxValue - sliderMinValue)/50.0;
			scrollAccel += 0.1;
			updateSlider();
		} else if (checkBoundingBox(position.xy() + vec2(width-18,0),position.xy() + vec2(width,18),x,y)) {
			sliderValue += scrollAccel*(sliderMaxValue - sliderMinValue)/50.0;
			scrollAccel += 0.1;
			updateSlider();
		} else {
			float relMouseX = x - position.x - 18 - 14;
			relMouseX /= (width-64);
			float newVal = relMouseX*(sliderMaxValue - sliderMinValue);
			
			if (newVal != sliderValue) {
				
				sliding = true;
				sliderValue = newVal;
				updateSlider();
				changed = true;
			}
			//clickDown = false;
		}
	}

	//Method designed to update the slider
	void updateSlider() {
		sliderValue = fclamp(sliderValue, sliderMinValue, sliderMaxValue);
		textures[slideBtn].position.x = int(18.0 + (width-64.0)*(sliderValue - sliderMinValue)/(sliderMaxValue - sliderMinValue));
	}

};


struct UICaption : public UIControl {
	string caption;
	xfont font;

	UICaption() {
		width = 180;
		height = 18;
		setCaption("caption");
		background.visible = true;
	}
	void setColor(vec4 newColor) {
		background.color = newColor;
	}
	void setCaption(const string &text) {
		ximage canvas(width,height,1,XIMAGE_FORMAT_RGBA32);
		canvas.fill(vec4(0,0,0,0));
		canvas.drawText(text,0,0,vec4(1,1,1,1),1,font);

		unsigned int texIndex = 0;
		if (background.texture) {
			glDeleteTextures(1,&background.texture);
		}
		glGenTextures(1,&texIndex);
		glBindTexture(GL_TEXTURE_2D, texIndex);
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,canvas.width(),canvas.height(),0,GL_RGBA,GL_UNSIGNED_BYTE,canvas(0,0));
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		background.size = vec2(width,height);
		background.texture = texIndex;
		background.textureBase = vec2(0,0);
		background.textureSize = vec2(1,1);
	}

};

struct UICheckBox : public UIControl {
	bool checked;
	int checkedTexture;
	int uncheckedTexture;

	UICheckBox() {
		checked = false;
		createTextures();
		width = 18;
		height = 18;
	}
	void setChecked(bool newValue) {
		checked = newValue;
		textures[uncheckedTexture].visible = !checked;
		textures[checkedTexture].visible = checked;
	}
	void createTextures() {
		uncheckedTexture = textures.size();
		textures.push_back(TextureRegion());
		textures.back().size = vec2(18,18);
		textures.back().textureSize = vec2(18.0/256.0,18.0/256.0);
		textures.back().textureBase = vec2(64.0/256.0,1.0 - 18.0/256.0);
		textures.back().texture = defaultTexture;

		checkedTexture = textures.size();
		textures.push_back(TextureRegion());
		textures.back().visible = false;
		textures.back().size = vec2(18,18);
		textures.back().textureSize = vec2(18.0/256.0,18.0/256.0);
		textures.back().textureBase = vec2(100.0/256.0,1.0 - 18.0/256.0);
		textures.back().texture = defaultTexture;

	}
	void onMouseUpEvent(float x, float y) {
		setChecked(!checked);
	}
};

struct UIButton : public UIControl {
	int topLeftTex[3];
	int topMidTex[3];
	int topRightTex[3];
	int midLeftTex[3];
	int midMidTex[3];
	int midRightTex[3];
	int botLeftTex[3];
	int botMidTex[3];
	int botRightTex[3];


	bool mouseOver;
	bool mouseDown;
	bool btnPushed;

	UIButton() {
		width = 80;
		height = 18;
		createTextures();
		resetState(); 
	}
	void createTextures() {
		textures.clear();
		for (int i = 0; i < 3; i++) {
			topLeftTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(0,height-6,0);
			textures.back().size = vec2(6,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(202.0/256.0 + i*18/256.0,1.0 - 6.0/256.0);
			textures.back().texture = defaultTexture;

			topMidTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(6,height-6,0);
			textures.back().size = vec2(width-12,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(208.0/256.0 + i*18/256.0,1.0 - 6.0/256.0);
			textures.back().texture = defaultTexture;

			topRightTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(width-6,height-6,0);
			textures.back().size = vec2(6,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(214.0/256.0 + i*18/256.0,1.0 - 6.0/256.0);
			textures.back().texture = defaultTexture;


			midLeftTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(0,6,0);
			textures.back().size = vec2(6,height-12);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(202.0/256.0 + i*18/256.0,1.0 - 12.0/256.0);
			textures.back().texture = defaultTexture;

			midMidTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(6,6,0);
			textures.back().size = vec2(width-12,height-12);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(208.0/256.0 + i*18/256.0,1.0 - 12.0/256.0);
			textures.back().texture = defaultTexture;

			midRightTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(width-6,6,0);
			textures.back().size = vec2(6,height-12);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(214.0/256.0 + i*18/256.0,1.0 - 12.0/256.0);
			textures.back().texture = defaultTexture;


			botLeftTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(0,0,0);
			textures.back().size = vec2(6,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(202.0/256.0 + i*18/256.0,1.0 - 18.0/256.0);
			textures.back().texture = defaultTexture;

			botMidTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(6,0,0);
			textures.back().size = vec2(width-12,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(208.0/256.0 + i*18/256.0,1.0 - 18.0/256.0);
			textures.back().texture = defaultTexture;

			botRightTex[i] = textures.size();
			textures.push_back(TextureRegion());
			textures.back().position = vec3(width-6,0,0);
			textures.back().size = vec2(6,6);
			textures.back().textureSize = vec2(6.0/256.0,6.0/256.0);
			textures.back().textureBase = vec2(214.0/256.0 + i*18/256.0,1.0 - 18.0/256.0);
			textures.back().texture = defaultTexture;
		}

	}
	void setMouseOverState() {
		if (mouseOver) return;
		mouseOver = true;
		mouseDown = false;
		textures[topLeftTex	[0]].visible = false;
		textures[topMidTex	[0]].visible = false;
		textures[topRightTex[0]].visible = false;
		textures[midLeftTex	[0]].visible = false;
		textures[midMidTex	[0]].visible = false;
		textures[midRightTex[0]].visible = false;
		textures[botLeftTex	[0]].visible = false;
		textures[botMidTex	[0]].visible = false;
		textures[botRightTex[0]].visible = false;

		textures[topLeftTex	[1]].visible = true;
		textures[topMidTex	[1]].visible = true;
		textures[topRightTex[1]].visible = true;
		textures[midLeftTex	[1]].visible = true;
		textures[midMidTex	[1]].visible = true;
		textures[midRightTex[1]].visible = true;
		textures[botLeftTex	[1]].visible = true;
		textures[botMidTex	[1]].visible = true;
		textures[botRightTex[1]].visible = true;

		textures[topLeftTex	[2]].visible = false;
		textures[topMidTex	[2]].visible = false;
		textures[topRightTex[2]].visible = false;
		textures[midLeftTex	[2]].visible = false;
		textures[midMidTex	[2]].visible = false;
		textures[midRightTex[2]].visible = false;
		textures[botLeftTex	[2]].visible = false;
		textures[botMidTex	[2]].visible = false;
		textures[botRightTex[2]].visible = false;
	}
	void setMouseDownState() {
		if (mouseDown) return;
		mouseOver = false;
		mouseDown = true;


		textures[topLeftTex	[0]].visible = false;
		textures[topMidTex	[0]].visible = false;
		textures[topRightTex[0]].visible = false;
		textures[midLeftTex	[0]].visible = false;
		textures[midMidTex	[0]].visible = false;
		textures[midRightTex[0]].visible = false;
		textures[botLeftTex	[0]].visible = false;
		textures[botMidTex	[0]].visible = false;
		textures[botRightTex[0]].visible = false;

		textures[topLeftTex	[1]].visible = false;
		textures[topMidTex	[1]].visible = false;
		textures[topRightTex[1]].visible = false;
		textures[midLeftTex	[1]].visible = false;
		textures[midMidTex	[1]].visible = false;
		textures[midRightTex[1]].visible = false;
		textures[botLeftTex	[1]].visible = false;
		textures[botMidTex	[1]].visible = false;
		textures[botRightTex[1]].visible = false;

		textures[topLeftTex	[2]].visible = true;
		textures[topMidTex	[2]].visible = true;
		textures[topRightTex[2]].visible = true;
		textures[midLeftTex	[2]].visible = true;
		textures[midMidTex	[2]].visible = true;
		textures[midRightTex[2]].visible = true;
		textures[botLeftTex	[2]].visible = true;
		textures[botMidTex	[2]].visible = true;
		textures[botRightTex[2]].visible = true;


	}
	void resetState() {

		textures[topLeftTex	[0]].visible = true;
		textures[topMidTex	[0]].visible = true;
		textures[topRightTex[0]].visible = true;
		textures[midLeftTex	[0]].visible = true;
		textures[midMidTex	[0]].visible = true;
		textures[midRightTex[0]].visible = true;
		textures[botLeftTex	[0]].visible = true;
		textures[botMidTex	[0]].visible = true;
		textures[botRightTex[0]].visible = true;

		textures[topLeftTex	[1]].visible = false;
		textures[topMidTex	[1]].visible = false;
		textures[topRightTex[1]].visible = false;
		textures[midLeftTex	[1]].visible = false;
		textures[midMidTex	[1]].visible = false;
		textures[midRightTex[1]].visible = false;
		textures[botLeftTex	[1]].visible = false;
		textures[botMidTex	[1]].visible = false;
		textures[botRightTex[1]].visible = false;

		textures[topLeftTex	[2]].visible = false;
		textures[topMidTex	[2]].visible = false;
		textures[topRightTex[2]].visible = false;
		textures[midLeftTex	[2]].visible = false;
		textures[midMidTex	[2]].visible = false;
		textures[midRightTex[2]].visible = false;
		textures[botLeftTex	[2]].visible = false;
		textures[botMidTex	[2]].visible = false;
		textures[botRightTex[2]].visible = false;
		mouseOver = false;
		mouseDown = false;
		btnPushed = false;
	}
	bool pollPushed() {
		if (btnPushed) {
			btnPushed = false;
			return true;
		}
		return false;
	}
	void onMouseUpEvent(float x, float y) {
		if (!btnPushed) {
			btnPushed = true;
		}
		if (mouseCurrentlyOver) {
			setMouseOverState();
		} else {
			resetState();
		}
	}
	void onMouseExitEvent(float x, float y) {
		if (!clickDown) resetState();
	}
	void onMouseDownEvent(float x, float y) {
		setMouseDownState();
	}

	void onMouseEnterEvent(float x, float y) {
		setMouseOverState();
	}
};

void renderTextureRegion(TextureRegion* tregion) {
	if (!tregion->visible) return;
	glColor4fv(tregion->color.ptr);
	glBindTexture(GL_TEXTURE_2D, tregion->texture);
	glBegin(GL_QUADS);
	glTexCoord2fv(tregion->textureBase.ptr);
	glVertex3fv(tregion->position.ptr);

	glTexCoord2fv((tregion->textureBase + vec2(tregion->textureSize.x,0)).ptr);
	glVertex3fv((tregion->position + vec3(tregion->size.x,0,0)).ptr);

	glTexCoord2fv((tregion->textureBase + vec2(tregion->textureSize.x,tregion->textureSize.y)).ptr);
	glVertex3fv((tregion->position + vec3(tregion->size.x,tregion->size.y,0)).ptr);

	glTexCoord2fv((tregion->textureBase + vec2(0,tregion->textureSize.y)).ptr);
	glVertex3fv((tregion->position + vec3(0,tregion->size.y,0)).ptr);

	glEnd();
}


void renderUIPanelOpenGL(UIPanel* panel, float viewWidth, float viewHeight) {
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glMatrixMode(GL_PROJECTION);
	glPushMatrix();	
	glLoadIdentity();
	glOrtho(0,viewWidth,0,viewHeight,-1000,1000);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	
	//draw panel background
	glColor4f(1,1,1,1);	
	renderTextureRegion(&panel->background);

	
	
	//draw uicontrols
	for (int i = 0; i < panel->childControls.size(); i++) {
		glPushMatrix();
		glTranslatef(panel->childControls[i]->position.x,panel->childControls[i]->position.y,panel->childControls[i]->position.z);
		renderTextureRegion(&(panel->childControls[i]->background));
		for (int j = 0; j < panel->childControls[i]->textures.size(); j++) { 
			renderTextureRegion(&(panel->childControls[i]->textures[j]));
		}
		glPopMatrix();
		if (!panel->maximized) break;
	}

	renderTextureRegion(&(panel->cursorTexture));

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

#endif
