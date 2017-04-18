#ifndef VECTORX_H
#define VECTORX_H

#include "matrixx.h"

/****************************************/
/*   xmath Vector Type Definitions      */
/****************************************/

#define dot *
#define cross ^
//C-Style operators
struct vec2;
struct vec3;
struct vec4;

typedef vec2		Vector2;
typedef vec3		Vector3;
typedef vec4		Vector4;



void vec2_dot(const vec2& a, const vec2& b, float &result);     //result = a dot b 
void vec3_dot(const vec3& a, const vec3& b, float &result);
void vec4_dot(const vec4& a, const vec4& b, float &result);

void vec3_cross(const vec3& a, const vec3& b, vec3 &result);    //result = a cross b

void vec2_add(const vec2& a, const vec2& b, vec2 &result);      //result = a + b
void vec3_add(const vec3& a, const vec3& b, vec3 &result);
void vec4_add(const vec4& a, const vec4& b, vec4 &result);

void vec2_sub(const vec2& a, const vec2& b, vec2 &result);      //result = a - b
void vec3_sub(const vec3& a, const vec3& b, vec3 &result);
void vec4_sub(const vec4& a, const vec4& b, vec4 &result);

void vec2_mul(const vec2& a, const vec2& b, vec2 &result);      //result = a & b
void vec3_mul(const vec3& a, const vec3& b, vec3 &result);
void vec4_mul(const vec4& a, const vec4& b, vec4 &result);

void vec2_smul(const vec2& a, float s, vec2 &result);           //result = a * s
void vec3_smul(const vec3& a, float s, vec3 &result);
void vec4_smul(const vec4& a, float s, vec4 &result);

void vec2_div(const vec2& a, const vec2& b, vec2 &result);      //result = a / b
void vec3_div(const vec3& a, const vec3& b, vec3 &result);
void vec4_div(const vec4& a, const vec4& b, vec4 &result);

void vec2_sdiv(const vec2& a, float s, vec2 &result);           //result = a / s
void vec3_sdiv(const vec3& a, float s, vec3 &result);
void vec4_sdiv(const vec4& a, float s, vec4 &result);

float vec2_distance(const vec2 &a, const vec2 &b);
float vec3_distance(const vec3 &a, const vec3 &b);
float vec4_distance(const vec4 &a, const vec4 &b);


struct vec2 {
	/**********************/
	/*   2d Vector Type   */
	/**********************/
	union {
		struct { float x, y; };		//named notation
		struct { float ptr[2]; };		//vector notation
	};

	vec2() {}
	vec2(float s);
	vec2(float _x, float _y);
	vec2(const float* vec2Array);
	vec2(const vec2& v2);
	vec2(const vec3& v3);
	vec2(const vec4& v4);


	void	clamp(const vec2 &min, const vec2 &max);	//clamp component values between given min and max component values
	void	clamp(float min, float max);				//clamp component values between given min and max values
	void	clampRadial(float maxRadius);				//clamp to given radius
	void	normalize();								//normalize the vector to length 1

	float	length() const;								//returns the length (magnitude) of the vector
	float	lengthSqr() const;							//returns the squared length (magnitude^2) of the vector
	vec2	normalized() const;							//returns a normalized copy of the vector
	vec2	yx() const;									//returns a copy with components swapped

	vec2&	operator +=(const vec2 &other);
	vec2&	operator -=(const vec2 &other);
	vec2&	operator +=(float other);
	vec2&	operator -=(float other);
	vec2&	operator *=(const vec2 &other);
	vec2&	operator *=(float other);
	vec2&	operator /=(const vec2 &other);
	vec2&	operator /=(float other);

	bool	operator ==(const vec2 &other) const;		//component-wise compasison
	bool	operator !=(const vec2 &other) const;		//component-wise compasison
	bool	operator < (const vec2 &other) const;		//length compasison
	bool	operator <=(const vec2 &other) const;		//length compasison
	bool	operator > (const vec2 &other) const;		//length compasison
	bool	operator >=(const vec2 &other) const;		//length compasison

	vec2	operator +(const vec2 &other) const;
	vec2	operator -(const vec2 &other) const;
	vec2	operator -() const;
	float	operator *(const vec2 &other) const;		//dot product
	vec2	operator &(const vec2 &other) const;		//vector component product
	vec2	operator /(const vec2 &other) const;		//vector component division
	vec2	operator /(float &s) const;					//vector scalar division
	vec2	operator *(float &s) const;					//vector scalar multiplication
	float	operator ~();								//returns the length (magnitude) of the vector

	friend vec2 operator *(float s, const vec2 &v);		//scalar vector product
	friend vec2 operator /(float s, const vec2 &v);		//scalar vector division

};

struct vec3 {

	/**********************/
	/*   3d Vector Type   */
	/**********************/
	union {
		struct { float x, y, z; };		//named notation
		struct { float ptr[3]; };		//vector notation
	};

	vec3() {}
	vec3(float s);
	vec3(float _x, float _y, float _z);
	vec3(const float* vec3Array);
	vec3(const vec2& v2);
	vec3(const vec3& v3);
	vec3(const vec4& v4);


	void	clamp(const vec3 &min, const vec3 &max);	//clamp component values between given min and max component values
	void	clamp(float min, float max);				//clamp component values between given min and max values
	void	clampRadial(float maxRadius);				//clamp to given radius
	void	normalize();								//normalize the vector to length 1

	float	length() const;								//returns the length (magnitude) of the vector
	float	lengthSqr() const;							//returns the squared length (magnitude^2) of the vector
	vec3	normalized() const;							//returns a normalized copy of the vector
	vec3	reflect(const vec3 &normal) const;			//reflects the vector about the given normal
	vec2	xz() const;									//returns a vec2 of the x and z components
	vec2	yz() const;									//returns a vec2 of the y and z components
	vec2	xy() const;									//returns a vec2 of the x and y components
	vec3    yzx() const;								//returns a rearranged copy of the vector
	vec3    zxy() const;								//returns a rearranged copy of the vector
	vec3    zyx() const;								//returns a rearranged copy of the vector

	vec3&	operator +=(const vec3 &other);
	vec3&	operator -=(const vec3 &other);
	vec3&	operator +=(float other);
	vec3&	operator -=(float other);
	vec3&	operator *=(float other);
	vec3&	operator /=(const vec3 &other);
	vec3&	operator /=(float other);
	vec3&	operator &=(const vec3 &other);

	bool	operator ==(const vec3 &other) const;		//component-wise comparison
	bool	operator !=(const vec3 &other) const;		//component-wise comparison
	bool	operator < (const vec3 &other) const;		//length comparison
	bool	operator <=(const vec3 &other) const;		//length comparison		
	bool	operator > (const vec3 &other) const;		//length comparison
	bool	operator >=(const vec3 &other) const;		//length comparison

	vec3	operator +(const vec3 &other) const;		//vector addition
	vec3	operator -(const vec3 &other) const;		//vector subtraction
	vec3	operator -() const;
	float	operator *(const vec3 &other) const;		//dot product
	vec3	operator &(const vec3 &other) const;		//component-wise product 
	vec3	operator /(const vec3 &other) const;		//component-wise division
	vec3	operator /(float s) const;					//scalar division
	vec3	operator *(float s) const;					//scalar multiplication
	vec3	operator ^(const vec3 &other) const;		//cross product
	float	operator ~();								//length

	friend vec3 operator *(float s, const vec3 &v);
	friend vec3 operator /(float s, const vec3 &v);

	float& operator [](int i);
	const float& operator [](int i) const;
};

struct vec4 {
	/**********************/
	/*   4d Vector Type   */
	/**********************/
	union {
		struct { float x, y, z, w; };	//named notation
		struct { float ptr[4]; };		//vector notation
	};

	vec4();
	vec4(float s);
	vec4(float _x, float _y, float _z, float _w);
	vec4(const float* vec4Array);
	vec4(const vec2& v2);
	vec4(const vec3& v3);
	vec4(const vec4& v4);


	void	clamp(const vec4 &min, const vec4 &max);	//clamp component values between given min and max component values
	void	clamp(float min, float max);				//clamp component values between given min and max values
	void	clampRadial(float maxRadius);				//clamp to given radius
	void	normalize();								//normalize the vector to length 1

	float	length() const;								//returns the length (magnitude) of the vector
	float	lengthSqr() const;							//returns the squared length (magnitude^2) of the vector
	vec4	normalized() const;							//returns a normalized copy of the vector
	vec3	xyz() const;								//returns the xyz components as a vec3
	void    getAxes(vec3 &x, vec3 &y, vec3 &z);
	vec4&	operator +=(const vec4 &other);
	vec4&	operator -=(const vec4 &other);
	vec4&	operator +=(float other);
	vec4&	operator -=(float other);
	vec4&	operator &=(const vec4 &other);
	vec4&	operator *=(float other);
	vec4&	operator /=(const vec4 &other);
	vec4&	operator /=(float other);

	bool	operator ==(const vec4 &other) const;		//component-wise comparison
	bool	operator !=(const vec4 &other) const;		//component-wise comparison
	bool	operator < (const vec4 &other) const;		//length comparison
	bool	operator <=(const vec4 &other) const;		//length comparison		
	bool	operator > (const vec4 &other) const;		//length comparison
	bool	operator >=(const vec4 &other) const;		//length comparison

	vec4	operator +(const vec4 &other) const;		//vector addition
	vec4	operator -(const vec4 &other) const;		//vector subtraction
	vec4	operator -() const;
	float	operator *(const vec4 &other) const;		//dot product
	vec4	operator &(const vec4 &other) const;		//component-wise product 
	vec4	operator /(const vec4 &other) const;		//component-wise division
	vec4	operator /(float s) const;					//scalar division
	vec4	operator *(float s) const;					//scalar multiplication
	float	operator ~();								//length
	float&	operator [](int i);							//return ith component
	const float&	operator [](int i) const;			//return ith component
	friend vec4 operator *(float s, const vec4 &v);
	friend vec4 operator /(float s, const vec4 &v);
};

/****************************************/
/*      vec2		                    */
/****************************************/

vec2::vec2(float _x, float _y) {
	x = _x;
	y = _y;
}
vec2::vec2(const vec2 &other) {
	x = other.x;
	y = other.y;
}


float vec2::length() const {
	return fsqrt(x*x + y*y);
}
float vec2::lengthSqr() const {
	return x*x + y*y;
}
void vec2::normalize() {
	float s = finvSqrt(lengthSqr());
	x *= s;
	y *= s;
}
vec2 vec2::normalized() const {
	vec2 n = *this;
	n.normalize();
	return n;
}


vec2 vec2::operator -(const vec2 &other) const {
	return vec2(x - other.x, y - other.y);
}
vec2 vec2::operator +(const vec2 &other) const {
	return vec2(x + other.x, y + other.y);
}
float vec2::operator *(const vec2 &other) const {
	return x*other.x + y*other.y;
}
vec2& vec2::operator +=(const vec2 &other) {
	x += other.x;
	y += other.y;
	return *this;
}



/****************************************/
/*      vec3		                    */
/****************************************/

vec3::vec3(float s) {
	x = y = z = s;
}
vec3::vec3(float _x, float _y, float _z) {
	x = _x;
	y = _y;
	z = _z;
}
vec3::vec3(const float* vec3Array) {
	x = vec3Array[0];
	y = vec3Array[1];
	z = vec3Array[2];
}
vec3::vec3(const vec2& v2) {
	x = v2.x;
	y = v2.y;
	z = 0;
}
vec3::vec3(const vec3& v3) {
	x = v3.x;
	y = v3.y;
	z = v3.z;
}
vec3::vec3(const vec4& v4) {
	x = v4.x;
	y = v4.y;
	z = v4.z;
}

vec2 vec3::xz() const {
	return vec2(x, z);
}
vec2 vec3::yz() const {
	return vec2(y, z);
}
vec2 vec3::xy() const {
	return vec2(x, y);
}


void	vec3::clamp(const vec3 &min, const vec3 &max) {
	x = (x > min.x ? (x < max.x ? x : max.x) : min.x);
	y = (y > min.y ? (y < max.y ? y : max.y) : min.y);
	z = (z > min.z ? (z < max.z ? z : max.z) : min.z);
}
void	vec3::clamp(float min, float max) {
	x = (x > min ? (x < max ? x : max) : min);
	y = (y > min ? (y < max ? y : max) : min);
	z = (z > min ? (z < max ? z : max) : min);
}
void	vec3::clampRadial(float maxRadius) {
	float vsize = length();
	if (vsize > maxRadius) {
		normalize();
		*this *= maxRadius;
	}
}
void	vec3::normalize() {
	float s = finvSqrt(lengthSqr());
	x *= s;
	y *= s;
	z *= s;
}

float	vec3::length() const {
	return fsqrt(x*x + y*y + z*z);
}
float	vec3::lengthSqr() const {
	return x*x + y*y + z*z;
}
vec3	vec3::normalized() const {
	vec3 n = *this;
	n.normalize();
	return n;
}

vec3&	vec3::operator +=(const vec3 &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}
vec3&	vec3::operator -=(const vec3 &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	return *this;
}
vec3&	vec3::operator +=(float other) {
	x += other;
	y += other;
	z += other;
	return *this;
}
vec3&	vec3::operator -=(float other) {
	x -= other;
	y -= other;
	z -= other;
	return *this;
}
vec3&	vec3::operator &=(const vec3 &other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
	return *this;
}
vec3&	vec3::operator *=(float other) {
	x *= other;
	y *= other;
	z *= other;
	return *this;
}
vec3&	vec3::operator /=(const vec3 &other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;
	return *this;
}
vec3&	vec3::operator /=(float other) {
	other = 1.0f / other;
	x *= other;
	y *= other;
	z *= other;
	return *this;
}

bool	vec3::operator ==(const vec3 &other) const {
	return x == other.x && y == other.y && z == other.z;
}
bool	vec3::operator !=(const vec3 &other) const {
	return x != other.x && y != other.y && z != other.z;
}
bool	vec3::operator < (const vec3 &other) const {
	return lengthSqr() < other.lengthSqr();
}
bool	vec3::operator <=(const vec3 &other) const {
	return lengthSqr() <= other.lengthSqr();
}
bool	vec3::operator > (const vec3 &other) const {
	return lengthSqr() > other.lengthSqr();
}
bool	vec3::operator >=(const vec3 &other) const {
	return lengthSqr() >= other.lengthSqr();
}

vec3	vec3::operator +(const vec3 &other) const {
	vec3 result;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	return result;
}
vec3	vec3::operator -(const vec3 &other) const {
	vec3 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	return result;
}
vec3	vec3::operator -() const {
	return vec3(-x, -y, -z);
}
float	vec3::operator *(const vec3 &other) const {
	float d;
	d = x*other.x + y*other.y + z*other.z;
	return d;
}
vec3	vec3::operator &(const vec3 &other) const {
	vec4 result;
	result.x = x * other.x;
	result.y = y * other.y;
	result.z = z * other.z;
	return result;

}
vec3	vec3::operator /(const vec3 &other) const {
	vec3 result;
	result.x = x / other.x;
	result.y = y / other.y;
	result.z = z / other.z;
	return result;
}
vec3	vec3::operator /(float s) const {
	vec3 result;
	s = 1.0f / s;
	result.x = x*s;
	result.y = y*s;
	result.z = z*s;
	return result;
}
vec3	vec3::operator *(float s) const {
	vec4 result;
	result.x = x*s;
	result.y = y*s;
	result.z = z*s;
	return result;
}
float	vec3::operator ~() {
	return length();
}
float&	vec3::operator [](int i) {
	return ptr[i];
}
const float&	vec3::operator [](int i) const {
	return ptr[i];
}
vec3 operator *(float s, const vec3 &v) {
	return v*s;
}
vec3 operator /(float s, const vec3 &v) {
	vec4 result;
	result.x = s / v.x;
	result.y = s / v.y;
	result.z = s / v.z;
	return result;
}

vec3	vec3::operator ^(const vec3 &other) const {
	vec3 v;
	v.x = y*other.z - z*other.y;
	v.y = z*other.x - x*other.z;
	v.z = x*other.y - y*other.x;
	return v;
}





/****************************************/
/*      vec4		                    */
/****************************************/

vec4::vec4() {}
vec4::vec4(float s) {
	x = s;
	y = s;
	z = s;
	w = s;
}
vec4::vec4(float _x, float _y, float _z, float _w) {
	x = _x;
	y = _y;
	z = _z;
	w = _w;
}
vec4::vec4(const float* vec4Array) {
	x = vec4Array[0];
	y = vec4Array[1];
	z = vec4Array[2];
	w = vec4Array[3];
}
vec4::vec4(const vec2& v2) {
	x = v2.x;
	y = v2.y;
	z = 0.0;
	w = 0.0;
}
vec4::vec4(const vec3& v3) {
	x = v3.x;
	y = v3.y;
	z = v3.z;
	w = 0.0;
}
vec4::vec4(const vec4& v4) {
	x = v4.x;
	y = v4.y;
	z = v4.z;
	w = v4.w;
}


void	vec4::clamp(const vec4 &min, const vec4 &max) {
	x = (x > min.x ? (x < max.x ? x : max.x) : min.x);
	y = (y > min.y ? (y < max.y ? y : max.y) : min.y);
	z = (z > min.z ? (z < max.z ? z : max.z) : min.z);
	w = (w > min.w ? (w < max.w ? w : max.w) : min.w);
}
void	vec4::clamp(float min, float max) {
	x = (x > min ? (x < max ? x : max) : min);
	y = (y > min ? (y < max ? y : max) : min);
	z = (z > min ? (z < max ? z : max) : min);
	w = (w > min ? (w < max ? w : max) : min);
}
void	vec4::clampRadial(float maxRadius) {
	float vsize = length();
	if (vsize > maxRadius) {
		normalize();
		*this *= maxRadius;
	}
}
void	vec4::normalize() {
	float s = finvSqrt(length());
	x *= s;
	y *= s;
	z *= s;
	w *= s;
}

float	vec4::length() const {
	return fsqrt(x*x + y*y + z*z + w*w);
}
float	vec4::lengthSqr() const {
	return x*x + y*y + z*z + w*w;
}
vec4	vec4::normalized() const {
	vec4 n = *this;
	n.normalize();
	return n;
}
vec3	vec4::xyz() const {
	return vec3(x, y, z);
}
//void    vec4::getAxes(vec3 &x, vec3 &y, vec3 &z);
vec4&	vec4::operator +=(const vec4 &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}
vec4&	vec4::operator -=(const vec4 &other) {
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}
vec4&	vec4::operator +=(float other) {
	x += other;
	y += other;
	z += other;
	w += other;
	return *this;
}
vec4&	vec4::operator -=(float other) {
	x -= other;
	y -= other;
	z -= other;
	w -= other;
	return *this;
}
vec4&	vec4::operator &=(const vec4 &other) {
	x *= other.x;
	y *= other.y;
	z *= other.z;
	w *= other.w;
	return *this;
}
vec4&	vec4::operator *=(float other) {
	x *= other;
	y *= other;
	z *= other;
	w *= other;
	return *this;
}
vec4&	vec4::operator /=(const vec4 &other) {
	x /= other.x;
	y /= other.y;
	z /= other.z;
	w /= other.w;
	return *this;
}
vec4&	vec4::operator /=(float other) {
	other = 1.0f / other;
	x *= other;
	y *= other;
	z *= other;
	w *= other;
	return *this;
}

bool	vec4::operator ==(const vec4 &other) const {
	return x == other.x && y == other.y && z == other.z && w == other.w;
}
bool	vec4::operator !=(const vec4 &other) const {
	return x != other.x && y != other.y && z != other.z && w != other.w;
}
bool	vec4::operator < (const vec4 &other) const {
	return lengthSqr() < other.lengthSqr();
}
bool	vec4::operator <=(const vec4 &other) const {
	return lengthSqr() <= other.lengthSqr();
}
bool	vec4::operator > (const vec4 &other) const {
	return lengthSqr() > other.lengthSqr();
}
bool	vec4::operator >=(const vec4 &other) const {
	return lengthSqr() >= other.lengthSqr();
}

vec4	vec4::operator +(const vec4 &other) const {
	vec4 result;
	result.x = x + other.x;
	result.y = y + other.y;
	result.z = z + other.z;
	result.w = w + other.w;
	return result;
}
vec4	vec4::operator -(const vec4 &other) const {
	vec4 result;
	result.x = x - other.x;
	result.y = y - other.y;
	result.z = z - other.z;
	result.w = w - other.w;
	return result;
}
vec4	vec4::operator -() const {
	return vec4(-x, -y, -z, -w);
}
float	vec4::operator *(const vec4 &other) const {
	float d;
	d = x*other.x + y*other.y + z*other.z + w*other.w;
	return d;
}
vec4	vec4::operator &(const vec4 &other) const {
	vec4 result;
	result.x = x * other.x;
	result.y = y * other.y;
	result.z = z * other.z;
	result.w = w * other.w;
	return result;

}
vec4	vec4::operator /(const vec4 &other) const {
	vec4 result;
	result.x = x / other.x;
	result.y = y / other.y;
	result.z = z / other.z;
	result.w = w / other.w;
	return result;
}
vec4	vec4::operator /(float s) const {
	vec4 result;
	s = 1.0f / s;
	result.x = x*s;
	result.y = y*s;
	result.z = z*s;
	result.w = w*s;
	return result;
}
vec4	vec4::operator *(float s) const {
	vec4 result;
	result.x = x*s;
	result.y = y*s;
	result.z = z*s;
	result.w = w*s;
	return result;
}
float	vec4::operator ~() {
	return length();
}
float&	vec4::operator [](int i) {
	return ptr[i];
}
const float&	vec4::operator [](int i) const {
	return ptr[i];
}
vec4 operator *(float s, const vec4 &v) {
	return v*s;
}
vec4 operator /(float s, const vec4 &v) {
	vec4 result;
	result.x = s / v.x;
	result.y = s / v.y;
	result.z = s / v.z;
	result.w = s / v.w;
	return result;
}

#endif