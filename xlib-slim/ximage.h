#ifndef XIMAGE_H
#define XIMAGE_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <vector>
#include <utility>
#include <cstdlib>

#include "xvector.h"

using namespace std;

namespace xlib {
	//support pixel storage format
#define XIMAGE_FORMAT_RGBA32			0			//32 bit pixel with 4 8 bit channels in RGBA order
#define XIMAGE_FORMAT_RGB24				1			//24 bit pixel with 3 8 bit channels in RGB order
#define XIMAGE_FORMAT_RGBAFLOAT32		3			//128 bit pixel with 4 32 bit float channels
#define XIMAGE_FORMAT_RGBA5551			4			//16 bit pixel with 5 bit color channels and 1 bit alpha
#define XIMAGE_FORMAT_RGB655			5			//16 bit pixel with 6 bit R channel and 5 bit G and B channels
#define XIMAGE_FORMAT_GRAYSCALE8		6			//8 bit pixel with single channel color
#define XIMAGE_FORMAT_GRAYSCALE16		7			//16 bit pixel with single channel color
#define XIMAGE_FORMAT_GRAYSCALE32		8			//32 bit pixel with single channel color
#define XIMAGE_FORMAT_GRAYSCALE_FLOAT32	9			//32 bit pixel with single channel color as float

	//supported file storage format
#define XIMAGE_FILE_FORMAT_XMG			0			//native ximage file format
#define XIMAGE_FILE_FORMAT_BMP			1			//format represented as a windows bitmap file
#define XIMAGE_FILE_FORMAT_TGA			2			//format represented as a TARGA file format
#define XIMAGE_FILE_FORMAT_PNG			3			//format represented as the PNG image file format
#define XIMAGE_FILE_FORMAT_TIFF			4			//format represented as the TIFF file format
#define XIMAGE_FILE_FORMAT_RAW			5			//format stored as raw native pixel data


#define MAX(X,Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))

#define JPG_SOI 0xFFD8
#define JPG_SOF 0xFFC0
#define JPG_SOS 0xFFDA
#define JPG_EOS 0xFFD9
#define JPG_DHT 0xFFC4
#define JPG_DQT 0xFFDB

	const float SQRT2 = 1.4142135623730950488f;
	const float PI = 3.14159265358979324f;

	class xfont;
	class ximage  {

		struct PixelDecompositionCallbacks {
			float(*getChannelNormalizedFunc)(char*, int);		//returns the normalized color channel value at the given channel number
			int(*getPixelSizeBytesFunc)(void);					//returns the size of a pixel in bytes
			int(*getNumChannelsFunc)(void);					//returns the number of channels per pixel
			int(*getChannelBitSizeFunc)(int);					//returns the size in bits for the given channel number
			void(*setChannelValueNormalizedFunc)(char*, int, float);
		};

		unsigned short _width;
		unsigned short _height;
		unsigned short _depth;
		unsigned short	_pixelFormat;
		int		_imageDataSize;
		bool	_imageDataNotOwned;
		char*	_imageData;


		PixelDecompositionCallbacks _pixelDecompFuncs;

		static float	_getChannelNormalized_RGBA32(char* pixel, int channel);
		static int		_getPixelSizeBytes_RGBA32();
		static int		_getNumChannels_RGBA32();
		static int		_getChannelBitSize_RGBA32(int channel);
		static void		_setChannelValueNormalized_RGBA32(char* pixel, int channel, float value);

		static float	_getChannelNormalized_RGB24(char* pixel, int channel);
		static int		_getPixelSizeBytes_RGB24();
		static int		_getNumChannels_RGB24();
		static int		_getChannelBitSize_RGB24(int channel);
		static void		_setChannelValueNormalized_RGB24(char* pixel, int channel, float value);


		static float	_getChannelNormalized_GRAYSCALE8(char* pixel, int channel);
		static int		_getPixelSizeBytes_GRAYSCALE8();
		static int		_getNumChannels_GRAYSCALE8();
		static int		_getChannelBitSize_GRAYSCALE8(int channel);
		static void		_setChannelValueNormalized_GRAYSCALE8(char* pixel, int channel, float value);

		static float	_getChannelNormalized_GRAYSCALE_FLOAT32(char* pixel, int channel);
		static int		_getPixelSizeBytes_GRAYSCALE_FLOAT32();
		static int		_getNumChannels_GRAYSCALE_FLOAT32();
		static int		_getChannelBitSize_GRAYSCALE_FLOAT32(int channel);
		static void		_setChannelValueNormalized_GRAYSCALE_FLOAT32(char* pixel, int channel, float value);

		static float	_getChannelNormalized_RGBAFLOAT32(char* pixel, int channel);
		static int		_getPixelSizeBytes_RGBAFLOAT32();
		static int		_getNumChannels_RGBAFLOAT32();
		static int		_getChannelBitSize_RGBAFLOAT32(int channel);
		static void		_setChannelValueNormalized_RGBAFLOAT32(char* pixel, int channel, float value);

		void			_init();
		void			_setCurrentPixelDecompCallBacks(int format);
		int				_getPixelSizeBytes() const;
		float			_getPixelChannelNormalized(int offset, int channel)  const;
		int				_getChannelValue(int offset, int channel)  const;
		int				_getChannelSizeBits(int channel) const;

		void			_setPixelChannelNormalized(int offset, int channel, float value);


	public:
		int				_getPixelOffset(int row, int col, int dep) const;
		//ximage constructors
		ximage();
		ximage(const ximage &other);
		ximage(int imageWidth, int imageHeight, int imageDepth = 1, int storageFormat = XIMAGE_FORMAT_RGBA32);
		ximage(void* pixelData, int pixelStorageFormat, int width, int height, int depth = 1);
		ximage(string filename, int fileformat);

		void overrideImagePtr(char* newData);

		int width() const;
		int height() const;
		int depth() const;
		int size() const;
		int pixelFormat() const;
		int pixelSize();

		//image transformations
		ximage resizedTo(int newWidth, int newHeight, int newDepth = 1, bool filtered = true) const;
		ximage rotatedClockwise() const;
		ximage rotatedCounterClockwise() const;
		ximage rotated180() const;
		ximage flippedHorizontal() const;
		ximage flippedVertical() const;
		ximage convertedTo(int pixelformat) const;
		ximage inverted() const;
		ximage cropped(int leftX, int leftY, int rightX, int rightY) const;
		ximage channel(int channelNum) const;
		ximage blur(float radius) const;
		ximage gradientMap(const vector<vec4> &gradient) const;
		ximage displaced(const ximage &displaceMap, float magnitude) const;
		ximage DCT(int size) const;
		ximage inverseDCT(int size) const;
		ximage RGBToYUV(float wr = 0.299, float wg = 0.587, float wb = 0.114, float umax = 0.436, float vmax = 0.615) const;
		ximage YUVToRGB(float wr = 0.299, float wg = 0.587, float wb = 0.114, float umax = 0.436, float vmax = 0.615) const;
		//get/set pixels
		vec4	getPixelLerp(float u, float v) const;
		vec4	getVoxelLerp(float u, float v, float w) const;

		vec4	getPixel(int col, int row) const;
		void	setPixel(int col, int row, const vec4 &pixelVal);

		vec4	getVoxel(int col, int row, int dep) const;
		void	setVoxel(int col, int row, int dep, const vec4 &pixelVal);

		char*	getDataSource();
		void	setDataSource(char* source);
		void	fill(const vec4 &color);

		void	drawText(string text, int xloc, int yloc, vec4 color, float scale, const xfont &font);
		void	drawSolidRect(int xloc, int yloc, int w, int h, vec4 color);
		void	drawRect(int xloc, int yloc, int w, int h, int thickness, vec4 color);
		void	drawLine(int xstart, int ystart, int xend, int yend, int thickness, vec4 color);


		//image operators
		ximage  operator - (const ximage &other) const;
		ximage  operator + (const ximage &other) const;
		ximage  operator * (const ximage &other) const;
		ximage  operator / (const ximage &other) const;

		ximage  operator - (float s) const;
		ximage  operator + (float s) const;
		ximage  operator * (float s) const;
		ximage  operator / (float s) const;
		ximage  operator - (const vec4 &v) const;
		ximage  operator + (const vec4 &v) const;
		ximage  operator * (const vec4 &v) const;
		ximage  operator / (const vec4 &v) const;

		ximage & operator =  (const ximage &other);
		ximage & operator -= (const ximage &other);
		ximage & operator += (const ximage &other);
		ximage & operator *= (const ximage &other);
		ximage & operator /= (const ximage &other);

		ximage & operator -= (float s);
		ximage & operator += (float s);
		ximage & operator *= (float s);
		ximage & operator /= (float s);

		char* operator()(int col, int row, int dep = 0);
		const char* operator()(int col, int row, int dep = 0) const;

		void clear();

		//image IO
		void writeToFile(const string &fileName, int fileFormat) const;
		void readFromFile(const string &fileName, int fileFormat);

		void			exportAs_XMG(string filename) const;
		void			exportAs_BMP(string filename) const;
		void			exportAs_TIF(string filename) const;
		void			exportAs_PNG(string filename) const;
		void			exportAs_TGA(string filename) const;
		void			exportAs_JPG(string filename) const;


		void			importFrom_XMG(string filename);
		void			importFrom_BMP(string filename);
		void			importFrom_TIF(string filename);
		void			importFrom_PNG(string filename);
		void			importFrom_TGA(string filename);



		~ximage();
	};

	class xfont {
	public:
		struct FontData {
			int width;
			int height;
			int celWidth;
			int celHeight;
			char pad;
			unsigned char charWidth[256];
		};
		FontData fontData;
		ximage fontImg;
		xfont() {
			loadFont("xlib/monospaced_font.bmp", "xlib/monospaced_fontdata.dat");
		}
		xfont(string fontBmpFile, string fontDataFile) {
			loadFont(fontBmpFile, fontDataFile);
		}
		void loadFont(string fontBitmapFileName, string fontDataFileName) {
			ifstream fin;
			fin.open(fontDataFileName.c_str(), ios::binary);

			if (fin.fail()) {
				//cout << "Failed to load font data file: " << fontDataFileName << endl;
				return;
				//
			}

			fin.read((char*)(void*)&fontData, 273);
			fin.close();
			fin.clear();
			fontImg.importFrom_BMP(fontBitmapFileName);
		}
	};


	void	ximage::drawText(string text, int xloc, int yloc, vec4 color, float scale, const xfont &font) {
		int x = xloc;
		for (int c = 0; c < (int)text.length(); c++) {
			unsigned char chr = text[c];
			if (chr == '\n') {
				x = xloc;
				yloc -= (int)(font.fontData.celHeight*scale);
				continue;
			}
			if (chr == '\t') {
				x += (int)(font.fontData.charWidth[' '] * 4 * scale);
				continue;
			}
			int chrWidth = font.fontData.charWidth[chr];
			int chrHeight = font.fontData.celHeight;
			ximage resized;
			if (scale < 1.0f) resized = font.fontImg.resizedTo(int(font.fontImg.width()*scale), int(font.fontImg.height()*scale));
			for (int i = 0; i < chrWidth*scale; i++) {
				for (int j = 0; j < chrHeight*scale; j++) {
					float fontVal;
					if (scale > 1.0) fontVal = font.fontImg.getPixelLerp(float(i / scale + font.fontData.celWidth*chr) / font.fontImg.width(), float(j / scale) / font.fontImg.height()).x*color.w;
					else if (scale < 1.0) {
						fontVal = resized.getPixel(i + font.fontData.celWidth*chr*scale, j).x*color.w;
					}
					else fontVal = font.fontImg.getPixel(i + font.fontData.celWidth*chr, j).x*color.w;
					vec4 pixelVal = vec4(color.x, color.y, color.z, 1.0)*fontVal + getPixel(i + x, j + yloc)*(1.0f - fontVal);
					setPixel(i + x, j + yloc, pixelVal);
				}
			}
			x += chrWidth*scale;
		}
	}



	void	ximage::drawSolidRect(int xloc, int yloc, int w, int h, vec4 color) {
		for (int i = xloc; i < xloc + w; i++) {
			for (int j = yloc; j < yloc + h; j++) {
				setPixel(i, j, color);
			}
		}
	}

	void	ximage::drawRect(int xloc, int yloc, int w, int h, int thickness, vec4 color) {
		/*drawLine(xloc,yloc,xloc+w,yloc);
		drawLine(xloc+w,yloc,xloc+w,yloc+h);
		drawLine(xloc+w,yloc+h,xloc,yloc+h);
		drawLine(xloc,yloc+h,xloc,yloc);*/
	}
	void	ximage::drawLine(int xstart, int ystart, int xend, int yend, int thickness, vec4 color) {
		/*int dx = xend - xstart;
		int dy = yend - ystart;
		float errorx = 0;
		float errory = 0;
		int x = xstart;
		int u = ystart;
		while (x != xend && y != yend) {

		}*/
	}



	float	ximage::_getChannelNormalized_RGBA32(char* pixel, int channel) {
		if (channel > 3) return 255;
		return float(((unsigned char*)(void*)pixel)[channel]) / 255.0f;
	}
	int		ximage::_getPixelSizeBytes_RGBA32() {
		return 4;
	}
	int		ximage::_getNumChannels_RGBA32() {
		return 4;
	}
	int		ximage::_getChannelBitSize_RGBA32(int channel) {
		return 8;
	}
	void	ximage::_setChannelValueNormalized_RGBA32(char* pixel, int channel, float value) {
		value *= 255;
		if (value > 255) value = 255;
		if (value < 0) value = 0;
		if (channel > 3) return;
		pixel[channel] = (unsigned char)value;
	}


	float	ximage::_getChannelNormalized_RGB24(char* pixel, int channel) {
		if (channel > 2) return 1.0;
		return float(((unsigned char*)(void*)pixel)[channel]) / 255.0f;
	}
	int		ximage::_getPixelSizeBytes_RGB24() {
		return 3;
	}
	int		ximage::_getNumChannels_RGB24() {
		return 3;
	}
	int		ximage::_getChannelBitSize_RGB24(int channel) {
		return 8;
	}
	void	ximage::_setChannelValueNormalized_RGB24(char* pixel, int channel, float value) {
		value *= 255;
		if (value > 255) value = 255;
		if (value < 0) value = 0;
		if (channel > 2) return;
		pixel[channel] = (unsigned char)value;
	}

	float	ximage::_getChannelNormalized_GRAYSCALE8(char* pixel, int channel) {
		return float(((unsigned char*)(void*)pixel)[0]) / 255.0f;
	}
	int		ximage::_getPixelSizeBytes_GRAYSCALE8() {
		return 1;
	}
	int		ximage::_getNumChannels_GRAYSCALE8() {
		return 1;
	}
	int		ximage::_getChannelBitSize_GRAYSCALE8(int channel) {
		return 8;
	}
	void	ximage::_setChannelValueNormalized_GRAYSCALE8(char* pixel, int channel, float value) {
		float* fpixel = (float*)(void*)pixel;
		fpixel[0] = value;
	}


	float	ximage::_getChannelNormalized_GRAYSCALE_FLOAT32(char* pixel, int channel) {
		return ((float*)(void*)pixel)[0];
	}
	int		ximage::_getPixelSizeBytes_GRAYSCALE_FLOAT32() {
		return 4;
	}
	int		ximage::_getNumChannels_GRAYSCALE_FLOAT32() {
		return 1;
	}
	int		ximage::_getChannelBitSize_GRAYSCALE_FLOAT32(int channel) {
		return 32;
	}
	void	ximage::_setChannelValueNormalized_GRAYSCALE_FLOAT32(char* pixel, int channel, float value) {
		((float*)(void*)pixel)[0] = value;
	}


	float	ximage::_getChannelNormalized_RGBAFLOAT32(char* pixel, int channel) {
		return ((float*)(void*)pixel)[channel];
	}
	int		ximage::_getPixelSizeBytes_RGBAFLOAT32() {
		return 32;
	}
	int		ximage::_getNumChannels_RGBAFLOAT32() {
		return 4;
	}
	int		ximage::_getChannelBitSize_RGBAFLOAT32(int channel) {
		return 32;
	}
	void	ximage::_setChannelValueNormalized_RGBAFLOAT32(char* pixel, int channel, float value) {
		((float*)(void*)pixel)[channel] = value;
	}





	void ximage::_init() {
		_width = 0;
		_height = 0;
		_depth = 1;
		_pixelFormat = XIMAGE_FORMAT_RGBA32;
		_imageData = NULL;
		_imageDataSize = 0;
	}
	int ximage::width() const {
		return _width;
	}
	int ximage::height() const {
		return _height;
	}
	int ximage::depth() const {
		return _depth;
	}
	int ximage::pixelFormat() const {
		return _pixelFormat;
	}
	int ximage::pixelSize() {
		return _getPixelSizeBytes();
	}
	void ximage::_setCurrentPixelDecompCallBacks(int format) {
		switch (format) {
		case XIMAGE_FORMAT_RGBA32:
			_pixelDecompFuncs.getChannelNormalizedFunc = _getChannelNormalized_RGBA32;
			_pixelDecompFuncs.getPixelSizeBytesFunc = _getPixelSizeBytes_RGBA32;
			_pixelDecompFuncs.getNumChannelsFunc = _getNumChannels_RGBA32;
			_pixelDecompFuncs.getChannelBitSizeFunc = _getChannelBitSize_RGBA32;
			_pixelDecompFuncs.setChannelValueNormalizedFunc = _setChannelValueNormalized_RGBA32;
			break;
		case XIMAGE_FORMAT_RGB24:
			_pixelDecompFuncs.getChannelNormalizedFunc = _getChannelNormalized_RGB24;
			_pixelDecompFuncs.getPixelSizeBytesFunc = _getPixelSizeBytes_RGB24;
			_pixelDecompFuncs.getNumChannelsFunc = _getNumChannels_RGB24;
			_pixelDecompFuncs.getChannelBitSizeFunc = _getChannelBitSize_RGB24;
			_pixelDecompFuncs.setChannelValueNormalizedFunc = _setChannelValueNormalized_RGB24;
			break;
		case XIMAGE_FORMAT_GRAYSCALE_FLOAT32:
			_pixelDecompFuncs.getChannelNormalizedFunc = _getChannelNormalized_GRAYSCALE_FLOAT32;
			_pixelDecompFuncs.getPixelSizeBytesFunc = _getPixelSizeBytes_GRAYSCALE_FLOAT32;
			_pixelDecompFuncs.getNumChannelsFunc = _getNumChannels_GRAYSCALE_FLOAT32;
			_pixelDecompFuncs.getChannelBitSizeFunc = _getChannelBitSize_GRAYSCALE_FLOAT32;
			_pixelDecompFuncs.setChannelValueNormalizedFunc = _setChannelValueNormalized_GRAYSCALE_FLOAT32;
			break;
		case XIMAGE_FORMAT_RGBAFLOAT32:
			_pixelDecompFuncs.getChannelNormalizedFunc = _getChannelNormalized_RGBAFLOAT32;
			_pixelDecompFuncs.getPixelSizeBytesFunc = _getPixelSizeBytes_RGBAFLOAT32;
			_pixelDecompFuncs.getNumChannelsFunc = _getNumChannels_RGBAFLOAT32;
			_pixelDecompFuncs.getChannelBitSizeFunc = _getChannelBitSize_RGBAFLOAT32;
			_pixelDecompFuncs.setChannelValueNormalizedFunc = _setChannelValueNormalized_RGBAFLOAT32;
			break;
		default:
			_pixelDecompFuncs.getChannelNormalizedFunc = _getChannelNormalized_RGBA32;
			_pixelDecompFuncs.getPixelSizeBytesFunc = _getPixelSizeBytes_RGBA32;
			_pixelDecompFuncs.getNumChannelsFunc = _getNumChannels_RGBA32;
			_pixelDecompFuncs.getChannelBitSizeFunc = _getChannelBitSize_RGBA32;
			_pixelDecompFuncs.setChannelValueNormalizedFunc = _setChannelValueNormalized_RGBA32;
			break;
		}
	}

	int ximage::_getPixelSizeBytes() const {
		return _pixelDecompFuncs.getPixelSizeBytesFunc();
	}
	int	ximage::_getChannelSizeBits(int channel) const {
		return _pixelDecompFuncs.getChannelBitSizeFunc(channel);
	}

	int	ximage::_getPixelOffset(int row, int col, int dep) const {
		row = ((unsigned int)row) % _height;
		col = ((unsigned int)col) % _width;
		dep = ((unsigned int)dep) % _depth;
		return (dep*_width*_height + row*_width + col)*_getPixelSizeBytes();
	}

	float			ximage::_getPixelChannelNormalized(int offset, int channel) const {
		return _pixelDecompFuncs.getChannelNormalizedFunc(&_imageData[offset], channel);
	}
	void			ximage::_setPixelChannelNormalized(int offset, int channel, float value) {
		_pixelDecompFuncs.setChannelValueNormalizedFunc(&_imageData[offset], channel, value);
	}

	//ximage constructors
	ximage::ximage() {
		_init();
	}
	ximage::ximage(string filename, int fileformat) {
		_init();
		readFromFile(filename, fileformat);
	}
	ximage::ximage(int imageWidth, int imageHeight, int imageDepth, int storageFormat) {
		_init();
		_width = imageWidth;
		_height = imageHeight;
		_depth = imageDepth;
		_pixelFormat = storageFormat;
		_setCurrentPixelDecompCallBacks(storageFormat);
		_imageDataSize = _width*_height*_depth*_pixelDecompFuncs.getPixelSizeBytesFunc();
		_imageData = new char[_imageDataSize];
	}
	ximage::ximage(void* pixelData, int pixelStorageFormat, int width, int height, int depth) {
		_init();
		_width = width;
		_height = height;
		_depth = depth;
		_pixelFormat = pixelStorageFormat;
		_setCurrentPixelDecompCallBacks(pixelStorageFormat);
		_imageDataSize = _width*_height*_depth*_pixelDecompFuncs.getPixelSizeBytesFunc();
		_imageData = new char[_imageDataSize];
		memcpy(_imageData, pixelData, _imageDataSize);
	}


	void ximage::overrideImagePtr(char* newData) {
		_imageDataNotOwned = true;
		_imageData = newData;
	}

	//image transformations
	ximage ximage::inverted() const {
		ximage result(*this);
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				for (int k = 0; k < _depth; k++) {
					result.setVoxel(i, j, k, -getVoxel(i, j, k) + 1.0);
				}
			}
		}
		return result;
	}
	ximage ximage::cropped(int leftX, int leftY, int rightX, int rightY) const {
		int width = rightX - leftX;
		int height = rightY - leftY;
		ximage result(width, height, 1, _pixelFormat);

		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				result.setPixel(i, j, getPixel(i + leftX, j + leftY));
			}
		}

		return result;
	}

	ximage ximage::blur(float radius) const {
		const float gaussian[7][7] = {
			{ 0.00000067f, 0.00002292f, 0.00019117f, 0.00038771f, 0.00019117f, 0.00002292f, 0.00000067f },
			{ 0.00002292f, 0.00078633f, 0.00655965f, 0.01330373f, 0.00655965f, 0.00078633f, 0.00002292f },
			{ 0.00019117f, 0.00655965f, 0.05472157f, 0.11098164f, 0.05472157f, 0.00655965f, 0.00019117f },
			{ 0.00038771f, 0.01330373f, 0.11098164f, 0.22508352f, 0.11098164f, 0.01330373f, 0.00038771f },
			{ 0.00019117f, 0.00655965f, 0.05472157f, 0.11098164f, 0.05472157f, 0.00655965f, 0.00019117f },
			{ 0.00002292f, 0.00078633f, 0.00655965f, 0.01330373f, 0.00655965f, 0.00078633f, 0.00002292f },
			{ 0.00000067f, 0.00002292f, 0.00019117f, 0.00038771f, 0.00019117f, 0.00002292f, 0.00000067f }
		};
		ximage result(*this);
		float invWidth = 1.0f / _width;
		float invHeight = 1.0f / _height;
		radius = radius / 3.0f;
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				vec4 color(0.0);
				for (int x = 0; x < 7; x++) {
					for (int y = 0; y < 7; y++) {
						float u = radius*(x - 3);
						float v = radius*(y - 3);
						u += i;
						v += j;
						u *= invWidth;
						v *= invHeight;
						if (u < 0) u = 0;
						if (v < 0) v = 0;
						if (u > 1) u = 1;
						if (v > 1) v = 1;
						color += gaussian[y][x] * getPixelLerp(u, v);
					}
				}
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage ximage::RGBToYUV(float wr, float wg, float wb, float umax, float vmax) const {
		ximage result(*this);
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				for (int k = 0; k < _depth; k++) {
					vec4 rgb = getVoxel(i, j, k);
					vec4 yuv;
					yuv.x = wr*rgb.x + wg*rgb.y + wb*rgb.z;
					yuv.y = umax*(rgb.z - yuv.x) / (1.0 - wb);
					yuv.z = vmax*(rgb.x - yuv.x) / (1.0 - wr);
					yuv.w = rgb.w;
					result.setVoxel(i, j, k, yuv);
				}
			}
		}
		return result;
	}
	ximage ximage::YUVToRGB(float wr, float wg, float wb, float umax, float vmax) const {
		ximage result(*this);
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				for (int k = 0; k < _depth; k++) {
					vec4 yuv = getVoxel(i, j, k);
					vec4 rgb;
					rgb.x = yuv.x + yuv.z*(1.0 - wr) / vmax;
					rgb.y = yuv.x - yuv.y*(wb*(1.0 - wb) / (umax*wg)) - yuv.z*(wr*(1.0 - wr) / (vmax*wg));
					rgb.z = yuv.x + yuv.y*(1.0 - wb) / umax;
					rgb.w = yuv.w;
					result.setVoxel(i, j, k, rgb);
				}
			}
		}
		return result;
	}

	ximage ximage::resizedTo(int newWidth, int newHeight, int newDepth, bool filtered) const {
		ximage result(newWidth, newHeight, 1, _pixelFormat);
		bool downsampleHeight = newHeight < _height;
		bool downsampleWidth = newWidth < _width;
		float ratioWidth = float(newWidth) / _width;
		float ratioHeight = float(newHeight) / _height;
		for (int i = 0; i < newWidth; i++) {
			for (int j = 0; j < newHeight; j++) {
				if (!filtered) {
					result.setPixel(i, j, getPixel(i, j));
				}
				else {
					if (downsampleWidth && downsampleHeight) {
						vec4 sum;
						float count = 1;
						float i0 = 0;
						float j0 = 0;
						for (i0 = -0.5; i0 < 0.5; i0 += ratioWidth) {
							for (j0 = -0.5; j0 < 0.5; j0 += ratioHeight) {
								sum += getPixel(int((i + i0) / ratioWidth), int((j + j0) / ratioHeight));
								count++;
							}
						}
						sum /= count;
						result.setPixel(i, j, sum);
					}
					else if (downsampleWidth && !downsampleHeight) {
						vec4 sum;
						float count = 1;
						float i0 = 0;
						for (i0 = -0.5; i0 < 0.5; i0 += ratioWidth) {
							sum += getPixelLerp((i + i0) / newWidth, float(j) / newHeight);
							count++;
						}
						sum /= count;
						result.setPixel(i, j, sum);
					}
					else if (!downsampleWidth && downsampleHeight) {
						vec4 sum;
						float count = 1;
						float j0 = 0;
						for (j0 = -0.5; j0 < 0.5; j0 += ratioHeight) {
							sum += getPixelLerp(float(i) / newWidth, (j + j0) / newHeight);
							count++;
						}
						sum /= count;
						result.setPixel(i, j, sum);
					}
					else {
						result.setPixel(i, j, getPixelLerp(float(i) / newWidth, float(j) / newHeight));
					}
				}
			}
		}
		return result;
	}

	//get/set pixels
	vec4	ximage::getPixelLerp(float u, float v) const {
		int x, y;
		u *= _width;
		v *= _height;
		x = int(u);
		y = int(v);

		u -= x;
		v -= y;

		x = x%_width;
		y = y%_height;


		//if (x >= _width - 1) x = 0;
		//if (y >= _height - 1) y = 0;
		float uinv = 1.0f - u;
		float vinv = 1.0f - v;

		vec4 color;
		vec4 c00 = getPixel(x, y);
		vec4 c10 = getPixel(x + 1, y);
		vec4 c11 = getPixel(x + 1, y + 1);
		vec4 c01 = getPixel(x, y + 1);
		//color.x = coslerp(coslerp(c00.x, c10.x, u),  coslerp(c01.x, c11.x, u), v);
		//color.y = coslerp(coslerp(c00.y, c10.y, u),  coslerp(c01.y, c11.y, u), v);
		//color.z = coslerp(coslerp(c00.z, c10.z, u),  coslerp(c01.z, c11.z, u), v);
		//color.w = coslerp(coslerp(c00.w, c10.w, u),  coslerp(c01.w, c11.w, u), v);
		color.x = (c00.x*uinv + c10.x*u)*vinv + (c01.x*uinv + c11.x*u)*v;
		color.y = (c00.y*uinv + c10.y*u)*vinv + (c01.y*uinv + c11.y*u)*v;
		color.z = (c00.z*uinv + c10.z*u)*vinv + (c01.z*uinv + c11.z*u)*v;
		color.w = (c00.w*uinv + c10.w*u)*vinv + (c01.w*uinv + c11.w*u)*v;

		return color;
	}
	vec4	ximage::getVoxelLerp(float u, float v, float w) const {
		if (_depth == 1) return getPixelLerp(u, v);
		int x, y, z;
		u *= _width;
		v *= _height;
		w *= _depth;
		x = int(u);
		y = int(v);
		z = int(w);
		u -= x;
		v -= y;
		w -= z;


		if (x >= _width - 1) x = 0;
		if (y >= _height - 1) y = 0;
		if (z >= _depth - 1) y = 0;
		float uinv = 1.0f - u;
		float vinv = 1.0f - v;
		float winv = 1.0f - w;

		vec4 color;
		vec4 c000 = getVoxel(x, y, z);
		vec4 c100 = getVoxel(x + 1, y, z);
		vec4 c110 = getVoxel(x + 1, y + 1, z);
		vec4 c010 = getVoxel(x, y + 1, z);
		vec4 c001 = getVoxel(x, y, z + 1);
		vec4 c101 = getVoxel(x + 1, y, z + 1);
		vec4 c111 = getVoxel(x + 1, y + 1, z + 1);
		vec4 c011 = getVoxel(x, y + 1, z + 1);

		//color.x = coslerp(coslerp(coslerp(c000.x, c100.x, u), coslerp(c010.x, c110.x, u), v), coslerp(coslerp(c001.x, c101.x, u),  coslerp(c011.x, c111.x, u), v), w);
		//color.y = coslerp(coslerp(coslerp(c000.y, c100.y, u), coslerp(c010.y, c110.y, u), v), coslerp(coslerp(c001.y, c101.y, u),  coslerp(c011.y, c111.y, u), v), w);
		//color.z = coslerp(coslerp(coslerp(c000.z, c100.z, u), coslerp(c010.z, c110.z, u), v), coslerp(coslerp(c001.z, c101.z, u),  coslerp(c011.z, c111.z, u), v), w);
		//color.w = coslerp(coslerp(coslerp(c000.w, c100.w, u), coslerp(c010.w, c110.w, u), v), coslerp(coslerp(c001.w, c101.w, u),  coslerp(c011.w, c111.w, u), v), w);

		color.x = ((c000.x*uinv + c100.x*u)*vinv + (c010.x*uinv + c110.x*u)*v)*winv + ((c001.x*uinv + c101.x*u)*vinv + (c011.x*uinv + c111.x*u)*v)*w;
		color.y = ((c000.y*uinv + c100.y*u)*vinv + (c010.y*uinv + c110.y*u)*v)*winv + ((c001.y*uinv + c101.y*u)*vinv + (c011.y*uinv + c111.y*u)*v)*w;
		color.z = ((c000.z*uinv + c100.z*u)*vinv + (c010.z*uinv + c110.z*u)*v)*winv + ((c001.z*uinv + c101.z*u)*vinv + (c011.z*uinv + c111.z*u)*v)*w;
		color.w = ((c000.w*uinv + c100.w*u)*vinv + (c010.w*uinv + c110.w*u)*v)*winv + ((c001.w*uinv + c101.w*u)*vinv + (c011.w*uinv + c111.w*u)*v)*w;

		return color;
	}

	vec4	ximage::getPixel(int col, int row) const {
		//col = col % _width;
		//row = row % _height;
		int offset = _getPixelOffset(row, col, 0);
		vec4 color;
		color.x = _getPixelChannelNormalized(offset, 0);
		color.y = _getPixelChannelNormalized(offset, 1);
		color.z = _getPixelChannelNormalized(offset, 2);
		color.w = _getPixelChannelNormalized(offset, 3);
		return color;
	}
	void	ximage::setPixel(int col, int row, const vec4 &pixelVal) {
		//col = col % _width;
		//row = row % _height;
		int offset = _getPixelOffset(row, col, 0);
		_setPixelChannelNormalized(offset, 3, pixelVal.w);
		_setPixelChannelNormalized(offset, 2, pixelVal.z);
		_setPixelChannelNormalized(offset, 1, pixelVal.y);
		_setPixelChannelNormalized(offset, 0, pixelVal.x);
	}

	vec4	ximage::getVoxel(int col, int row, int dep) const {
		//col = col % _width;
		//row = row % _height;
		//dep = dep % _depth;
		int offset = _getPixelOffset(row, col, dep);
		vec4 color;
		color.x = _getPixelChannelNormalized(offset, 0);
		color.y = _getPixelChannelNormalized(offset, 1);
		color.z = _getPixelChannelNormalized(offset, 2);
		color.w = _getPixelChannelNormalized(offset, 3);
		return color;
	}
	void	ximage::setVoxel(int col, int row, int dep, const vec4 &pixelVal) {
		int offset = _getPixelOffset(row, col, dep);
		_setPixelChannelNormalized(offset, 3, pixelVal.w);
		_setPixelChannelNormalized(offset, 2, pixelVal.z);
		_setPixelChannelNormalized(offset, 1, pixelVal.y);
		_setPixelChannelNormalized(offset, 0, pixelVal.x);
	}
	char*	ximage::getDataSource() {
		return _imageData;
	}
	void	ximage::setDataSource(char* source) {
		if (_imageData) delete[] _imageData;
		_imageData = source;
	}

	void	ximage::fill(const vec4 &color) {
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				for (int k = 0; k < _depth; k++) {
					setVoxel(i, j, k, color);
				}
			}
		}
	}


	ximage ximage::channel(int channelNum) const {
		ximage result(_width, _height, _depth, _pixelFormat);
		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				for (int k = 0; k < _depth; k++) {
					float channelVal = getVoxel(i, j, k)[channelNum];
					result.setVoxel(i, j, k, vec4(channelVal));
				}
			}
		}
		return result;
	}
	ximage ximage::displaced(const ximage &displaceMap, float magnitude) const {
		ximage result(*this);
		float invWidth = 1.0f / _width;
		float invHeight = 1.0f / _height;

		for (int i = 0; i < _width; i++) {
			for (int j = 0; j < _height; j++) {
				float u = float(i);
				float v = float(j);
				u *= invWidth;
				v *= invHeight;
				vec4 dis = displaceMap.getPixelLerp(u, v);
				u += magnitude*(dis.x - 0.5f)*2.0f;
				v += magnitude*(dis.y - 0.5f)*2.0f;
				result.setPixel(i, j, getPixelLerp(u, v));
			}
		}
		return result;
	}
	ximage ximage::DCT(int size) const {
		ximage result(*this);
		for (int u = 0; u < _width; u += size) {
			for (int v = 0; v < _height; v += size) {
				for (int i = 0; i < size; i++) {
					for (int j = 0; j < size; j++) {
						vec4 sum(0, 0, 0, 0);
						float cx = (i == 0 ? 1.0 / SQRT2 : 1);
						float cy = (j == 0 ? 1.0 / SQRT2 : 1);
						for (int x = 0; x < size; x++) {
							for (int y = 0; y < size; y++) {
								vec4 pixel = getPixel(u + x, v + y);
								float h = cos(x*PI*(2 * i + 1.0) / (2.0*size))*cos(y*PI*(2 * j + 1.0) / (2.0*size));
								sum.x += pixel.x*h;
								sum.y += pixel.y*h;
								sum.z += pixel.z*h;
								sum.w += pixel.w*h;
							}
						}
						sum = cx*cy*sum*0.25;
						sum += 127 / size;
						sum /= 255 / size;
						result.setPixel(u + i, v + j, sum);
					}
				}

			}
		}
		return result;
	}
	ximage ximage::inverseDCT(int size) const {
		ximage result(*this);
		for (int u = 0; u < _width; u += size) {
			for (int v = 0; v < _height; v += size) {

				for (int x = 0; x < size; x++) {
					for (int y = 0; y < size; y++) {
						vec4 sum(0, 0, 0, 0);
						for (int i = 0; i < size; i++) {
							for (int j = 0; j < size; j++) {
								float cx = (i == 0 ? 1.0 / SQRT2 : 1);
								float cy = (j == 0 ? 1.0 / SQRT2 : 1);
								vec4 pixel = getPixel(u + i, v + j);
								pixel -= 0.5;
								pixel *= 255 / size;

								float h = cos(x*PI*(2 * i + 1.0) / (2 * size))*cos(y*PI*(2 * j + 1.0) / (2 * size));
								sum.x += pixel.x*h;
								sum.y += pixel.y*h;
								sum.z += pixel.z*h;
								sum.w += pixel.w*h;
							}
						}
						sum *= 0.25;
						result.setPixel(u + x, v + y, sum);
					}
				}


			}
		}
		return result;
	}
	//image operators
	ximage::ximage(const ximage &other) {
		_width = other._width;
		_height = other._height;
		_depth = other._depth;
		_pixelFormat = other._pixelFormat;
		_imageDataSize = other._imageDataSize;
		_imageData = new char[_imageDataSize];
		memcpy(_imageData, other._imageData, _imageDataSize);
		_pixelDecompFuncs = other._pixelDecompFuncs;
	}
	ximage & ximage::operator = (const ximage &other) {
		if (&other == this) return *this;
		clear();
		_width = other._width;
		_height = other._height;
		_depth = other._depth;
		_pixelFormat = other._pixelFormat;
		_imageDataSize = other._imageDataSize;
		_imageData = new char[_imageDataSize];
		memcpy(_imageData, other._imageData, _imageDataSize);
		_pixelDecompFuncs = other._pixelDecompFuncs;
		return *this;
	}
	ximage  ximage::operator - (const ximage &other) const {
		int maxWidth = MAX(other._width, _width);
		int maxHeight = MAX(other._height, _height);
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				float u = float(i) / maxWidth;
				float v = float(j) / maxHeight;
				vec4 color = getPixelLerp(u, v) - other.getPixelLerp(u, v);
				result.setPixel(i, j, color);
			}
		}
		return result;
	}

	ximage  ximage::operator + (const ximage &other) const {
		int maxWidth = MAX(other._width, _width);
		int maxHeight = MAX(other._height, _height);
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				float u = float(i) / maxWidth;
				float v = float(j) / maxHeight;
				vec4 color = getPixelLerp(u, v) + other.getPixelLerp(u, v);
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator * (const ximage &other) const {
		int maxWidth = MAX(other._width, _width);
		int maxHeight = MAX(other._height, _height);
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				float u = float(i) / maxWidth;
				float v = float(j) / maxHeight;
				vec4 color = getPixelLerp(u, v) & other.getPixelLerp(u, v);
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator / (const ximage &other) const {
		int maxWidth = MAX(other._width, _width);
		int maxHeight = MAX(other._height, _height);
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				float u = float(i) / maxWidth;
				float v = float(j) / maxHeight;
				vec4 color = getPixelLerp(u, v) / other.getPixelLerp(u, v);
				result.setPixel(i, j, color);
			}
		}
		return result;
	}


	ximage  ximage::operator - (float s) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) - s;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator + (float s) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) + s;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator * (float s) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) * s;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator / (float s) const {
		int maxWidth = _width;
		int maxHeight = _height;
		s = 1.0f / s;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) * s;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}



	ximage  ximage::operator - (const vec4 &v) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) - v;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator + (const vec4 &v) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) + v;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator * (const vec4 &v) const {
		int maxWidth = _width;
		int maxHeight = _height;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) & v;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}
	ximage  ximage::operator / (const vec4 &v) const {
		int maxWidth = _width;
		int maxHeight = _height;
		vec4 vinv = 1.0 / v;
		ximage result(maxWidth, maxHeight, 1, _pixelFormat);
		for (int i = 0; i < maxWidth; i++) {
			for (int j = 0; j < maxHeight; j++) {
				vec4 color = getPixel(i, j) & vinv;
				result.setPixel(i, j, color);
			}
		}
		return result;
	}

	/*ximage & ximage::operator -= (const ximage &other);
	ximage & ximage::operator += (const ximage &other);
	ximage & ximage::operator *= (const ximage &other);
	ximage & ximage::operator /= (const ximage &other);

	ximage & ximage::operator -= (float s);
	ximage & ximage::operator += (float s);
	ximage & ximage::operator *= (float s);
	ximage & ximage::operator /= (float s);
	*/
	char* ximage::operator()(int col, int row, int dep) {
		return &_imageData[_getPixelOffset(row, col, dep)];
	}
	const char* ximage::operator()(int col, int row, int dep) const {
		return &_imageData[_getPixelOffset(row, col, dep)];
	}

	void ximage::clear() {
		if (_imageData) delete[] _imageData;
		_imageData = NULL;
		_width = 0;
		_height = 0;
		_depth = 1;
		_imageDataSize = 0;
	}
	//image IO
	void ximage::writeToFile(const string &fileName, int fileFormat) const {

	}

	void ximage::exportAs_XMG(string filename) const {
		ofstream fout(filename.c_str(), ios::binary);
		if (fout.fail()) {
			//cout << "ERROR: Failed to open: " << filename << endl;
			return;
		}
		fout.write("XMG0", 4);
		fout.write((const char*)&_width, 2);
		fout.write((const char*)&_height, 2);
		fout.write((const char*)&_depth, 2);
		fout.write((const char*)&_pixelFormat, 2);
		fout.write((const char*)&_imageDataSize, 4);
		fout.write(_imageData, _imageDataSize);
		fout.close();
	}

	void ximage::importFrom_XMG(string filename) {
		ifstream fin(filename.c_str(), ios::binary);
		if (fin.fail()) {
			//cout << "ERROR: Failed to open: " << filename << endl;
			return;
		}
		char tag[5] = "XMG0";
		fin.read(tag, 4);
		if (tag != "XMG0") {
			cout << "ERROR: Invalid XMG file format" << endl;
			return;
		}
		clear();
		fin.read((char*)(void*)&_width, 2);
		fin.read((char*)(void*)&_height, 2);
		fin.read((char*)(void*)&_depth, 2);
		fin.read((char*)(void*)&_pixelFormat, 2);
		_setCurrentPixelDecompCallBacks(_pixelFormat);
		fin.read((char*)(void*)&_imageDataSize, 4);
		_imageData = new char[_imageDataSize];
		fin.read(_imageData, _imageDataSize);
		fin.close();
	}
	void ximage::exportAs_BMP(string filename) const {
		const char* fname = filename.c_str();
		ofstream fout;
		fout.open(fname, ios::binary);
		if (fout.fail()) {
			//cout << "ERROR: Failed to open: " << filename << endl;
			return;
		}
		char bmpHeader[54];
		memset(bmpHeader, 0, 54);
		unsigned int& fileSize = *(unsigned int*)(void*)&bmpHeader[0x2];
		unsigned int& dataOffset = *(unsigned int*)(void*)&bmpHeader[0xA];
		unsigned int& headerSize = *(unsigned int*)(void*)&bmpHeader[0xE];
		unsigned int& width = *(unsigned int*)(void*)&bmpHeader[0x12];
		unsigned int& height = *(unsigned int*)(void*)&bmpHeader[0x16];
		unsigned short& numPlanes = *(unsigned short*)(void*)&bmpHeader[0x1A];
		unsigned short& bbp = *(unsigned short*)(void*)&bmpHeader[0x1C];
		unsigned int& bitmapSize = *(unsigned int*)(void*)&bmpHeader[0x22];
		int pixelSize = _getPixelSizeBytes();
		if (pixelSize > 4 || _pixelFormat == XIMAGE_FORMAT_GRAYSCALE_FLOAT32) {
			ximage reformatted = ximage(_width, _height);
			for (int i = 0; i < _width; i++) {
				for (int j = 0; j < _height; j++) {
					reformatted.setPixel(i, j, getPixel(i, j));
				}
			}
			reformatted.exportAs_BMP(filename);
			return;
		}
		bbp = pixelSize * 8;

		int rowSize = int(4 * (ceil(bbp*_width / 32.0f)));
		bmpHeader[0] = 'B';
		bmpHeader[1] = 'M';
		fileSize = 54 + rowSize*_height;
		dataOffset = 54;
		headerSize = 40;
		width = _width;
		height = _height;
		numPlanes = 1;

		bitmapSize = rowSize*_height;
		int numChannels = _pixelDecompFuncs.getNumChannelsFunc();

		fout.write(bmpHeader, 54);
		char* tmpRow = new char[rowSize];
		memset(tmpRow, 0, rowSize);
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				int pixelOffset = _getPixelOffset(i, j, 0);
				if (numChannels == 4) {
					tmpRow[pixelSize*j + 0] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 2)*255.0);
					tmpRow[pixelSize*j + 1] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 1)*255.0);
					tmpRow[pixelSize*j + 2] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 0)*255.0);
					tmpRow[pixelSize*j + 3] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 3)*255.0);
				}
				else if (numChannels == 3) {
					tmpRow[pixelSize*j + 0] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 2)*255.0);
					tmpRow[pixelSize*j + 1] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 1)*255.0);
					tmpRow[pixelSize*j + 2] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 0)*255.0);
				}
				else {
					tmpRow[pixelSize*j + 0] = (unsigned char)(_getPixelChannelNormalized(pixelOffset, 0)*255.0);
				}

			}
			fout.write(tmpRow, rowSize);
		}
		delete[] tmpRow;
		fout.close();
	}


	void ximage::importFrom_BMP(string filename) {
		ifstream fin(filename.c_str(), ios::binary);
		if (fin.fail()) {
			//cout << "ERROR: Failed to open: " << filename << endl;
			return;
		}
		char bmpHeader[54];
		fin.read(bmpHeader, 54);
		unsigned int& fileSize = *(unsigned int*)(void*)&bmpHeader[0x2];
		unsigned int& dataOffset = *(unsigned int*)(void*)&bmpHeader[0xA];
		unsigned int& headerSize = *(unsigned int*)(void*)&bmpHeader[0xE];
		unsigned int& width = *(unsigned int*)(void*)&bmpHeader[0x12];
		unsigned int& height = *(unsigned int*)(void*)&bmpHeader[0x16];
		unsigned short& numPlanes = *(unsigned short*)(void*)&bmpHeader[0x1A];
		unsigned short& bbp = *(unsigned short*)(void*)&bmpHeader[0x1C];
		unsigned int& bitmapSize = *(unsigned int*)(void*)&bmpHeader[0x22];

		switch (bbp) {
		case 32:
			_setCurrentPixelDecompCallBacks(XIMAGE_FORMAT_RGBA32);
			break;
		case 24:
			_setCurrentPixelDecompCallBacks(XIMAGE_FORMAT_RGB24);
			break;
		case 8:
			_setCurrentPixelDecompCallBacks(XIMAGE_FORMAT_GRAYSCALE8);
			break;
		default:
			cout << "Error: Unsupported BITMAP format" << endl;
			return;
			break;
		}

		clear();
		_width = width;
		_height = height;
		_depth = 1;
		int pixelSize = _getPixelSizeBytes();
		_imageData = new char[_height*_width*pixelSize];
		int rowSize = int(4 * (ceil(bbp*_width / 32.0f)));
		char* tmpRow = new char[rowSize];
		_imageDataSize = _width*_height*_depth*pixelSize;

		for (int i = 0; i < _height; i++) {
			fin.read(tmpRow, rowSize);
			for (int j = 0; j < _width; j++) {
				int pixelOffset = _getPixelOffset(i, j, 0);
				if (bbp == 32) {
					_setPixelChannelNormalized(pixelOffset, 2, ((unsigned char)tmpRow[j*pixelSize + 0]) / 255.0f);
					_setPixelChannelNormalized(pixelOffset, 1, ((unsigned char)tmpRow[j*pixelSize + 1]) / 255.0f);
					_setPixelChannelNormalized(pixelOffset, 0, ((unsigned char)tmpRow[j*pixelSize + 2]) / 255.0f);
					_setPixelChannelNormalized(pixelOffset, 3, ((unsigned char)tmpRow[j*pixelSize + 3]) / 255.0f);

				}
				else if (bbp == 24) {
					_setPixelChannelNormalized(pixelOffset, 2, ((unsigned char)tmpRow[j*pixelSize + 0]) / 255.0f);
					_setPixelChannelNormalized(pixelOffset, 1, ((unsigned char)tmpRow[j*pixelSize + 1]) / 255.0f);
					_setPixelChannelNormalized(pixelOffset, 0, ((unsigned char)tmpRow[j*pixelSize + 2]) / 255.0f);
				}
				else {
					_setPixelChannelNormalized(pixelOffset, 0, ((unsigned char)tmpRow[j*pixelSize + 0]) / 255.0f);
				}

			}
		}
		delete[] tmpRow;
		fin.close();
	}

	void ximage::readFromFile(const string &fileName, int fileFormat) {
		switch (fileFormat) {
		case XIMAGE_FILE_FORMAT_BMP:
			importFrom_BMP(fileName);
			break;
		case XIMAGE_FILE_FORMAT_XMG:
			importFrom_XMG(fileName);
			break;
		}
	}

	ximage::~ximage() {
		if (!_imageDataNotOwned) {
			if (_imageData) delete[] _imageData;
			_imageData = NULL;
		}
	}
}

#endif