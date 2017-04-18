#ifndef MATRIXX_H
#define MATRIXX_H

#include "vectorx.h"

/****************************************/
/*   xmath Matrix Type Definitions      */
/****************************************/

//Rotation Order Bit Notations:
//00 = x
//01 = y
//10 = z
//xyz = 000110;
//yzx = 011000;
//zxy = 100001;
//zyx = 100100;
//yxz = 010010;
//xzy = 001001;
const int xyz = 0x6;
const int yzx = 0x18;
const int zxy = 0x21;
const int zyx = 0x25;
const int yxz = 0x12;
const int xzy = 0x9;

struct mat2;
struct mat3;
struct mat4;
typedef mat3		Matrix3;
typedef mat4		Matrix4;

struct mat2 {
	union {
		struct { float ptr[4]; };
		struct { float m[2][2]; };
		struct {
			float m00, m10;
			float m01, m11;
		};
	};

	mat2();
	mat2(float s);
	mat2(float _m00, float _m01,
		float _m10, float _m11);
	mat2(const float* mat2Array);
	mat2(const mat2 &m2);
	mat2(const mat3 &m3);
	mat2(const mat4 &m4);
	mat2(const vec2 &col0, const vec2 &col1);
	mat2(const vec2 *colArray);
	mat2(const vec4 &v);

	vec2    getRow(int row) const;

	//arithmatic operators
	mat2 operator +(const mat2 &other) const;	//matrix addition
	mat2 operator -(const mat2 &other) const;	//matrix subtraction
	mat2 operator -() const;
	mat2 operator *(const mat2 &other) const;	//matrix multiplication
	mat2 operator &(const mat2 &other) const;	//matrix component-wise multiplication
	mat2 operator |(const mat2 &other) const;	//matrix component-wise division
	mat2 operator *(float s) const;				//matrix scalar multiplication
	mat2 operator *(const vec2 &vec) const;		//matrix vector multiplication
	mat2 operator /(const mat2 &other) const;	//return matrix multiplied by other's inverse
	mat2 operator !() const;					//return inverse matrix
	mat2 operator ~() const;					//return matrix transposed


	//assignment operators
	mat2 &operator = (const mat2 &other);
	mat2 &operator += (const mat2 &other);
	mat2 &operator -= (const mat2 &other);
	mat2 &operator *= (const mat2 &other);
	mat2 &operator /= (const mat2 &other);
	mat2 &operator /= (float s);
	mat2 &operator *= (float s);
	mat2 &operator &= (const mat2 &other);
	mat2 &operator |= (const mat2 &other);

	//comparison operators
	bool operator == (const mat3 &other);
	bool operator != (const mat3 &other);


	//accessor operators
	float* operator [](int col);
	const float* operator [](int col) const;
	float& operator ()(int row, int col);
	const float& operator ()(int row, int col) const;

};

struct mat3 {
	union {
		struct { float ptr[9]; };
		struct { float m[3][3]; };
		struct {
			float m00, m10, m20;
			float m01, m11, m21;
			float m02, m12, m22;
		};
	};

	mat3();
	mat3(float s);
	mat3(float _m00, float _m01, float _m02,
		float _m10, float _m11, float _m12,
		float _m20, float _m21, float _m22);
	mat3(const float* mat3Array);
	mat3(const mat3 &m3);
	mat3(const mat4 &m4);
	mat3(const vec3 &col0, const vec3 &col1, const vec3& col2);
	mat3(const vec3 *colArray);


	float	det() const;							//return matrix determinant
	mat3	inverse() const;						//return inverse matrix
	mat3	inverseAffine2d() const;					//return affine inverse matrix
	void	invert();								//invert this matrix
	void	invertAffine2d();							//invert this matrix as an affine matrix
	void	transform2d(vec2 &v) const;				//apply affine transform to vector
	void	transform3d(vec3 &v) const;				//apply affine transform to vector

	//matrix construction
	void	makeIdentity();							//construct identity matrix
	void	makeTranslation2d(const vec2 &translation);                     //construct translation matrix
	void	makeRotation2d(float angleRadians, const vec2 &axis);           //construct 2d rotation matrix from angle and a given axis
	void	makeRotation2d(const vec2 &axisAngles, int rotationOrder);      //construct 2d rotation matrix from given angles around each axis
	void    makeTransform2d(const vec2& translation, const vec2 &axisAngles, int rotationOrder);
	void	makeScale2d(const vec2 &scale);                                 //construct scaling matrix
	void	makeRotation3d(float angleRadians, const vec3 &axis);         //construct rotation matrix from angle and a given axis
	void	makeRotation3d(const vec3 &axisAngles, int rotationOrder);	//construct rotation matrix from given angles around each axis



	//matrix decomposition
	vec2	getTranslation2d() const;
	vec2	getAxisAngleRotation2d() const;
	vec3	getAxisAngleRotation3d() const;
	vec3	getScale() const;
	vec3    getRow(int row) const;


	//arithmatic operators
	mat3 operator +(const mat3 &other) const;	//matrix addition
	mat3 operator -(const mat3 &other) const;	//matrix subtraction
	mat3 operator -() const;
	mat3 operator *(const mat3 &other) const;	//matrix multiplication
	mat3 operator &(const mat3 &other) const;	//matrix component-wise multiplication
	mat3 operator |(const mat3 &other) const;	//matrix component-wise division
	mat3 operator *(float s) const;				//matrix scalar multiplication
	mat3 operator *(const vec2 &vec) const;		//matrix vector multiplication
	mat3 operator *(const vec3 &vec) const;		//matrix vector multiplication
	mat3 operator /(const mat3 &other) const;	//return matrix multiplied by other's inverse
	mat3 operator !() const;					//return inverse matrix
	mat3 operator ~() const;					//return matrix transposed


	//assignment operators
	mat3 &operator = (const mat3 &other);
	mat3 &operator += (const mat3 &other);
	mat3 &operator -= (const mat3 &other);
	mat3 &operator *= (const mat3 &other);
	mat3 &operator /= (const mat3 &other);
	mat3 &operator /= (float s);
	mat3 &operator *= (float s);
	mat3 &operator &= (const mat3 &other);
	mat3 &operator |= (const mat3 &other);

	//comparison operators
	bool operator == (const mat3 &other);
	bool operator != (const mat3 &other);


	//accessor operators
	float* operator [](int col);
	const float* operator [](int col) const;
	float& operator ()(int row, int col);
	const float& operator ()(int row, int col) const;

};

struct mat4 {
	union {
		struct { float ptr[16]; };
		struct { float m[4][4]; };
		struct {
			float m00, m10, m20, m30;
			float m01, m11, m21, m31;
			float m02, m12, m22, m32;
			float m03, m13, m23, m33;
		};
	};

	mat4();
	mat4(float s);
	mat4(float _m00, float _m01, float _m02, float _m03,
		float _m10, float _m11, float _m12, float _m13,
		float _m20, float _m21, float _m22, float _m23,
		float _m30, float _m31, float _m32, float _m33);
	mat4(const float* mat4Array);
	mat4(const mat4 &m4);
	mat4(const vec4 &col0, const vec4 &col1, const vec4& col2, const vec4& col3);
	mat4(const vec4 *colArray);

	float	det() const;							//return matrix determinant
	mat4	inverse() const;						//return inverse matrix
	mat4	inverseAffine() const;					//return affine inverse matrix
	void	invert();								//invert this matrix
	void	invertAffine();							//invert this matrix as an affine matrix
	void	transform(vec3 &v) const;				//apply affine transform to vector
	void	transform(vec4 &v) const;				//apply affine transform to vector

	//matrix construction
	void	makeIdentity();							//construct identity matrix
	void	makeTranslation(const vec3 &translation);                  //construct translation matrix
	void	makeRotation(float angleRadians, const vec3 &axis);         //construct rotation matrix from angle and a given axis
	void	makeRotation(const vec3 &axisAngles, int rotationOrder);	//construct rotation matrix from given angles around each axis
	void    makeTransform(const vec3& translation, const vec3 &axisAngles, int rotationOrder);
	void    makeTransform(const vec3& translation, const vec3 &axisAngles, int rotationOrder, const vec3 &scale);
	void	makeScale(const vec3 &scale);                               //construct scaling matrix
	void	setCol(int col, const vec4 &v4);
	void	setCol(int col, const vec3 &v3);
	//matrix decomposition
	vec3	getTranslation() const;
	vec3	getAxisAngleRotation(int rotationOrder) const;
	vec4	getScale() const;
	vec4    getRow(int row) const;



	//arithmatic operators
	mat4 operator +(const mat4 &other) const;	//matrix addition
	mat4 operator -(const mat4 &other) const;	//matrix subtraction
	mat4 operator -() const;
	mat4 operator *(const mat4 &other) const;	//matrix multiplication
	mat4 operator &(const mat4 &other) const;	//matrix component-wise multiplication
	mat4 operator |(const mat4 &other) const;	//matrix component-wise division
	mat4 operator *(float s) const;				//matrix scalar multiplication
	vec3 operator *(const vec3 &v) const;		//matrix vector multiplication
	vec4 operator *(const vec4 &v) const;		//matrix vector multiplication
	mat4 operator /(const mat4 &other) const;	//return matrix multiplied by other's inverse
	mat4 operator !() const;					//return inverse matrix
	mat4 operator ~() const;					//return matrix transposed


	//assignment operators
	mat4 &operator = (const mat4 &other);
	mat4 &operator += (const mat4 &other);
	mat4 &operator -= (const mat4 &other);
	mat4 &operator *= (const mat4 &other);
	mat4 &operator /= (const mat4 &other);
	mat4 &operator /= (float s);
	mat4 &operator *= (float s);
	mat4 &operator &= (const mat4 &other);
	mat4 &operator |= (const mat4 &other);

	//comparison operators
	bool operator == (const mat4 &other);
	bool operator != (const mat4 &other);


	//accessor operators
	float* operator [](int col);
	const float* operator [](int col) const;
	float& operator ()(int row, int col);
	const float& operator ()(int row, int col) const;

};

/****************************************/
/*      mat3		                    */
/****************************************/

mat3::mat3() {

}
mat3::mat3(float s) {
	m00 = s;
	m01 = s;
	m02 = s;
	m10 = s;
	m11 = s;
	m12 = s;
	m20 = s;
	m21 = s;
	m22 = s;
}
mat3::mat3(float _m00, float _m01, float _m02,
	float _m10, float _m11, float _m12,
	float _m20, float _m21, float _m22) {
	m00 = _m00;
	m01 = _m01;
	m02 = _m02;
	m10 = _m10;
	m11 = _m11;
	m12 = _m12;
	m20 = _m20;
	m21 = _m21;
	m22 = _m22;

}
mat3::mat3(const float* mat3Array) {
	m00 = mat3Array[0];
	m10 = mat3Array[1];
	m20 = mat3Array[2];
	m01 = mat3Array[3];
	m11 = mat3Array[4];
	m21 = mat3Array[5];
	m02 = mat3Array[6];
	m12 = mat3Array[7];
	m22 = mat3Array[8];
}
mat3::mat3(const mat3 &m3) {
	m00 = m3.m00;
	m01 = m3.m01;
	m02 = m3.m02;
	m10 = m3.m10;
	m11 = m3.m11;
	m12 = m3.m12;
	m20 = m3.m20;
	m21 = m3.m21;
	m22 = m3.m22;
}
mat3::mat3(const mat4 &m4) {
	m00 = m4.m00;
	m01 = m4.m01;
	m02 = m4.m02;
	m10 = m4.m10;
	m11 = m4.m11;
	m12 = m4.m12;
	m20 = m4.m20;
	m21 = m4.m21;
	m22 = m4.m22;
}
mat3::mat3(const vec3 &col0, const vec3 &col1, const vec3& col2) {
	m00 = col0.x;
	m01 = col0.y;
	m02 = col0.z;
	m10 = col1.x;
	m11 = col1.y;
	m12 = col1.z;
	m20 = col2.x;
	m21 = col2.y;
	m22 = col2.z;
}
mat3::mat3(const vec3 *colArray) {
	m00 = colArray[0].x;
	m01 = colArray[0].y;
	m02 = colArray[0].z;
	m10 = colArray[1].x;
	m11 = colArray[1].y;
	m12 = colArray[1].z;
	m20 = colArray[2].x;
	m21 = colArray[2].y;
	m22 = colArray[2].z;
}

float* mat3::operator [](int col) {
	return m[col];
}
const float* mat3::operator [](int col) const {
	return m[col];
}
float& mat3::operator ()(int row, int col) {
	return m[col][row];
}
const float& mat3::operator ()(int row, int col) const {
	return m[col][row];
}


/****************************************/
/*      mat4		                    */
/****************************************/

mat4::mat4() {}
mat4::mat4(float s) {
	m00 = m10 = m20 = m30 =
		m01 = m11 = m21 = m31 =
		m02 = m12 = m22 = m32 =
		m03 = m13 = m23 = m33 = s;
}
mat4::mat4(float _m00, float _m01, float _m02, float _m03,
	float _m10, float _m11, float _m12, float _m13,
	float _m20, float _m21, float _m22, float _m23,
	float _m30, float _m31, float _m32, float _m33) {
	m00 = _m00; m10 = _m10; m20 = _m20; m30 = _m30;
	m01 = _m01;	m11 = _m11;	m21 = _m21;	m31 = _m31;
	m02 = _m02;	m12 = _m12;	m22 = _m22;	m32 = _m32;
	m03 = _m03;	m13 = _m13;	m23 = _m23;	m33 = _m33;
}
mat4::mat4(const float* mat4Array) {
	m00 = mat4Array[0]; m10 = mat4Array[4]; m20 = mat4Array[8];		m30 = mat4Array[12];
	m01 = mat4Array[1];	m11 = mat4Array[5];	m21 = mat4Array[9];		m31 = mat4Array[13];
	m02 = mat4Array[2];	m12 = mat4Array[6];	m22 = mat4Array[10];	m32 = mat4Array[14];
	m03 = mat4Array[3];	m13 = mat4Array[7];	m23 = mat4Array[11];	m33 = mat4Array[15];
}
mat4::mat4(const mat4 &m4) {
	for (int i = 0; i < 16; i++) ptr[i] = m4.ptr[i];
}
mat4::mat4(const vec4 &col0, const vec4 &col1, const vec4& col2, const vec4& col3) {
	m00 = col0[0]; m10 = col1[0]; m20 = col2[0];	m30 = col3[0];
	m01 = col0[1]; m11 = col1[1]; m21 = col2[1];	m31 = col3[1];
	m02 = col0[2]; m12 = col1[2]; m22 = col2[2];	m32 = col3[2];
	m03 = col0[3]; m13 = col1[3]; m23 = col2[3];	m33 = col3[3];
}
mat4::mat4(const vec4 *colArray) {
	m00 = colArray[0][0]; m10 = colArray[1][0]; m20 = colArray[2][0];	m30 = colArray[3][0];
	m01 = colArray[0][1]; m11 = colArray[1][1]; m21 = colArray[2][1];	m31 = colArray[3][1];
	m02 = colArray[0][2]; m12 = colArray[1][2]; m22 = colArray[2][2];	m32 = colArray[3][2];
	m03 = colArray[0][3]; m13 = colArray[1][3]; m23 = colArray[2][3];	m33 = colArray[3][3];
}

float	mat4::det() const {
	return ptr[0] * ptr[7] * ptr[9] * ptr[14] - ptr[0] * ptr[6] * ptr[9] * ptr[15]
		+ ptr[1] * ptr[6] * ptr[8] * ptr[15] - ptr[1] * ptr[7] * ptr[8] * ptr[14]
		+ ptr[2] * ptr[4] * ptr[9] * ptr[15] - ptr[2] * ptr[5] * ptr[8] * ptr[15]
		+ ptr[2] * ptr[7] * ptr[8] * ptr[13] - ptr[2] * ptr[7] * ptr[9] * ptr[12]
		- ptr[3] * ptr[4] * ptr[9] * ptr[14] + ptr[3] * ptr[5] * ptr[8] * ptr[14]
		- ptr[3] * ptr[6] * ptr[8] * ptr[13] + ptr[3] * ptr[6] * ptr[9] * ptr[12]
		+ ptr[0] * ptr[5] * ptr[10] * ptr[15] - ptr[0] * ptr[5] * ptr[11] * ptr[14]
		+ ptr[0] * ptr[6] * ptr[11] * ptr[13] - ptr[0] * ptr[7] * ptr[10] * ptr[13]
		- ptr[1] * ptr[4] * ptr[10] * ptr[15] + ptr[1] * ptr[4] * ptr[11] * ptr[14]
		- ptr[1] * ptr[6] * ptr[11] * ptr[12] + ptr[1] * ptr[7] * ptr[10] * ptr[12]
		- ptr[2] * ptr[4] * ptr[11] * ptr[13] + ptr[2] * ptr[5] * ptr[11] * ptr[12]
		+ ptr[3] * ptr[4] * ptr[10] * ptr[13] - ptr[3] * ptr[5] * ptr[10] * ptr[12];
}
mat4	mat4::inverse() const {
	float detinv = 1.0f / det();
	mat4 ip;
	ip.ptr[0] = (ptr[5] * ptr[10] * ptr[15] - ptr[5] * ptr[14] * ptr[11] - ptr[9] * ptr[6] * ptr[15] + ptr[9] * ptr[7] * ptr[14] + ptr[6] * ptr[13] * ptr[11] - ptr[13] * ptr[10] * ptr[7])*detinv;
	ip.ptr[4] = (ptr[4] * ptr[14] * ptr[11] - ptr[4] * ptr[10] * ptr[15] + ptr[8] * ptr[6] * ptr[15] - ptr[8] * ptr[7] * ptr[14] - ptr[12] * ptr[6] * ptr[11] + ptr[12] * ptr[10] * ptr[7])*detinv;
	ip.ptr[8] = (ptr[4] * ptr[9] * ptr[15] - ptr[4] * ptr[13] * ptr[11] - ptr[8] * ptr[5] * ptr[15] + ptr[8] * ptr[13] * ptr[7] + ptr[5] * ptr[12] * ptr[11] - ptr[12] * ptr[9] * ptr[7])*detinv;
	ip.ptr[12] = (ptr[4] * ptr[13] * ptr[10] - ptr[4] * ptr[9] * ptr[14] + ptr[8] * ptr[5] * ptr[14] - ptr[8] * ptr[6] * ptr[13] - ptr[5] * ptr[12] * ptr[10] + ptr[12] * ptr[9] * ptr[6])*detinv;

	ip.ptr[1] = (ptr[1] * ptr[14] * ptr[11] - ptr[1] * ptr[10] * ptr[15] + ptr[2] * ptr[9] * ptr[15] - ptr[2] * ptr[13] * ptr[11] - ptr[9] * ptr[3] * ptr[14] + ptr[3] * ptr[13] * ptr[10])*detinv;
	ip.ptr[5] = (ptr[0] * ptr[10] * ptr[15] - ptr[0] * ptr[14] * ptr[11] - ptr[8] * ptr[2] * ptr[15] + ptr[8] * ptr[3] * ptr[14] + ptr[2] * ptr[12] * ptr[11] - ptr[12] * ptr[3] * ptr[10])*detinv;
	ip.ptr[9] = (ptr[0] * ptr[13] * ptr[11] - ptr[0] * ptr[9] * ptr[15] + ptr[1] * ptr[8] * ptr[15] - ptr[1] * ptr[12] * ptr[11] - ptr[8] * ptr[3] * ptr[13] + ptr[12] * ptr[9] * ptr[3])*detinv;
	ip.ptr[13] = (ptr[0] * ptr[9] * ptr[14] - ptr[0] * ptr[13] * ptr[10] - ptr[1] * ptr[8] * ptr[14] + ptr[1] * ptr[12] * ptr[10] + ptr[8] * ptr[2] * ptr[13] - ptr[2] * ptr[12] * ptr[9])*detinv;

	ip.ptr[2] = (ptr[1] * ptr[6] * ptr[15] - ptr[1] * ptr[7] * ptr[14] - ptr[5] * ptr[2] * ptr[15] + ptr[5] * ptr[3] * ptr[14] + ptr[2] * ptr[13] * ptr[7] - ptr[6] * ptr[3] * ptr[13])*detinv;
	ip.ptr[6] = (ptr[0] * ptr[7] * ptr[14] - ptr[0] * ptr[6] * ptr[15] + ptr[4] * ptr[2] * ptr[15] - ptr[4] * ptr[3] * ptr[14] - ptr[2] * ptr[12] * ptr[7] + ptr[12] * ptr[6] * ptr[3])*detinv;
	ip.ptr[10] = (ptr[0] * ptr[5] * ptr[15] - ptr[0] * ptr[13] * ptr[7] - ptr[4] * ptr[1] * ptr[15] + ptr[4] * ptr[3] * ptr[13] + ptr[1] * ptr[12] * ptr[7] - ptr[5] * ptr[12] * ptr[3])*detinv;
	ip.ptr[14] = (ptr[0] * ptr[6] * ptr[13] - ptr[0] * ptr[5] * ptr[14] + ptr[4] * ptr[1] * ptr[14] - ptr[4] * ptr[2] * ptr[13] - ptr[1] * ptr[12] * ptr[6] + ptr[5] * ptr[2] * ptr[12])*detinv;

	ip.ptr[3] = (ptr[1] * ptr[10] * ptr[7] - ptr[1] * ptr[6] * ptr[11] + ptr[5] * ptr[2] * ptr[11] - ptr[5] * ptr[3] * ptr[10] - ptr[2] * ptr[9] * ptr[7] + ptr[9] * ptr[6] * ptr[3])*detinv;
	ip.ptr[7] = (ptr[0] * ptr[6] * ptr[11] - ptr[0] * ptr[10] * ptr[7] - ptr[4] * ptr[2] * ptr[11] + ptr[4] * ptr[3] * ptr[10] + ptr[8] * ptr[2] * ptr[7] - ptr[8] * ptr[6] * ptr[3])*detinv;
	ip.ptr[11] = (ptr[0] * ptr[9] * ptr[7] - ptr[0] * ptr[5] * ptr[11] + ptr[4] * ptr[1] * ptr[11] - ptr[4] * ptr[9] * ptr[3] - ptr[1] * ptr[8] * ptr[7] + ptr[8] * ptr[5] * ptr[3])*detinv;
	ip.ptr[15] = (ptr[0] * ptr[5] * ptr[10] - ptr[0] * ptr[9] * ptr[6] - ptr[4] * ptr[1] * ptr[10] + ptr[4] * ptr[2] * ptr[9] + ptr[1] * ptr[8] * ptr[6] - ptr[8] * ptr[5] * ptr[2])*detinv;
	return ip;
}
mat4	mat4::inverseAffine() const {
	return inverse();

}
void	mat4::invert() {
	*this = this->inverse();
}
void	mat4::invertAffine() {
	*this = this->inverseAffine();
}
void	mat4::transform(vec3 &v) const {

}
void	mat4::transform(vec4 &v) const {

}

//matrix construction
void	mat4::makeIdentity() {
	m00 = 1.0; m10 = 0.0; m20 = 0.0; m30 = 0.0;
	m01 = 0.0; m11 = 1.0; m21 = 0.0; m31 = 0.0;
	m02 = 0.0; m12 = 0.0; m22 = 1.0; m32 = 0.0;
	m03 = 0.0; m13 = 0.0; m23 = 0.0; m33 = 1.0;
}
void	mat4::makeTranslation(const vec3 &translation) {
	m00 = 1.0; m10 = 0.0; m20 = 0.0; m30 = translation.x;
	m01 = 0.0; m11 = 1.0; m21 = 0.0; m31 = translation.y;
	m02 = 0.0; m12 = 0.0; m22 = 1.0; m32 = translation.z;
	m03 = 0.0; m13 = 0.0; m23 = 0.0; m33 = 1.0;
}
void	mat4::makeRotation(float angleRadians, const vec3 &axis) {
	float c = cos(angleRadians);
	float s = sin(angleRadians);
	float t = 1.0 - c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(a1.x*a1.x + a1.y*a1.y + a1.z*a1.z);
	// if (magnitude==0) throw error;
	// a1.x /= magnitude;
	// a1.y /= magnitude;
	// a1.z /= magnitude;
	makeIdentity();
	m00 = c + axis.x*axis.x*t;
	m11 = c + axis.y*axis.y*t;
	m22 = c + axis.z*axis.z*t;
	float tmp1 = axis.x*axis.y*t;
	float tmp2 = axis.z*s;
	m10 = tmp1 + tmp2;
	m01 = tmp1 - tmp2;
	tmp1 = axis.x*axis.z*t;
	tmp2 = axis.y*s;
	m20 = tmp1 - tmp2;
	m02 = tmp1 + tmp2;    tmp1 = axis.y*axis.z*t;
	tmp2 = axis.x*s;
	m21 = tmp1 + tmp2;
	m12 = tmp1 - tmp2;

}
void	mat4::makeRotation(const vec3 &axisAngles, int rotationOrder) {
	mat4 rotAxes[3];
	rotAxes[0].makeRotation(axisAngles.x, vec3(1, 0, 0));
	rotAxes[1].makeRotation(axisAngles.y, vec3(0, 1, 0));
	rotAxes[2].makeRotation(axisAngles.x, vec3(0, 0, 1));
	makeIdentity();
	for (int i = 0; i < 3; i++) {
		int rotIndex = rotationOrder & 0x3;
		*this *= rotAxes[rotIndex];
		rotationOrder >>= 2;
	}
}
void    mat4::makeTransform(const vec3& translation, const vec3 &axisAngles, int rotationOrder) {

}
void    mat4::makeTransform(const vec3& translation, const vec3 &axisAngles, int rotationOrder, const vec3 &scale) {

}
void	mat4::makeScale(const vec3 &scale) {

}

//matrix decomposition
vec3	mat4::getTranslation() const {
	return vec3(m[3]);
}
vec3	mat4::getAxisAngleRotation(int rotationOrder) const {
	return vec3();
}
vec4	mat4::getScale() const {
	return vec3();
}
vec4    mat4::getRow(int row) const {
	return vec4(m[0][row], m[1][row], m[2][row], m[3][row]);
}



//arithmatic operators
mat4 mat4::operator +(const mat4 &other) const {
	mat4 result;
	result.m00 = m00 + other.m00; result.m01 = m01 + other.m01; result.m02 = m02 + other.m02; result.m03 = m03 + other.m03;
	result.m10 = m10 + other.m10; result.m11 = m11 + other.m11; result.m12 = m12 + other.m12; result.m13 = m13 + other.m13;
	result.m20 = m20 + other.m20; result.m21 = m21 + other.m21; result.m22 = m22 + other.m22; result.m23 = m23 + other.m23;
	result.m30 = m30 + other.m30; result.m31 = m31 + other.m31; result.m32 = m32 + other.m32; result.m33 = m33 + other.m33;
	return result;
}
mat4 mat4::operator -(const mat4 &other) const {
	mat4 result;
	result.m00 = m00 - other.m00; result.m01 = m01 - other.m01; result.m02 = m02 - other.m02; result.m03 = m03 - other.m03;
	result.m10 = m10 - other.m10; result.m11 = m11 - other.m11; result.m12 = m12 - other.m12; result.m13 = m13 - other.m13;
	result.m20 = m20 - other.m20; result.m21 = m21 - other.m21; result.m22 = m22 - other.m22; result.m23 = m23 - other.m23;
	result.m30 = m30 - other.m30; result.m31 = m31 - other.m31; result.m32 = m32 - other.m32; result.m33 = m33 - other.m33;
	return result;
}
mat4 mat4::operator -() const {
	mat4 result;
	result.m00 = -m00; result.m01 = -m01; result.m02 = -m02; result.m03 = -m03;
	result.m10 = -m10; result.m11 = -m11; result.m12 = -m12; result.m13 = -m13;
	result.m20 = -m20; result.m21 = -m21; result.m22 = -m22; result.m23 = -m23;
	result.m30 = -m30; result.m31 = -m31; result.m32 = -m32; result.m33 = -m33;
	return result;
}
mat4 mat4::operator *(const mat4 &B) const {
	mat4 result;
	result.m00 = m00 * B.m00 + m01 * B.m10 + m02 * B.m20 + m03 * B.m30;
	result.m01 = m00 * B.m01 + m01 * B.m11 + m02 * B.m21 + m03 * B.m31;
	result.m02 = m00 * B.m02 + m01 * B.m12 + m02 * B.m22 + m03 * B.m32;
	result.m03 = m00 * B.m03 + m01 * B.m13 + m02 * B.m23 + m03 * B.m33;
	result.m10 = m10 * B.m00 + m11 * B.m10 + m12 * B.m20 + m13 * B.m30;
	result.m11 = m10 * B.m01 + m11 * B.m11 + m12 * B.m21 + m13 * B.m31;
	result.m12 = m10 * B.m02 + m11 * B.m12 + m12 * B.m22 + m13 * B.m32;
	result.m13 = m10 * B.m03 + m11 * B.m13 + m12 * B.m23 + m13 * B.m33;
	result.m20 = m20 * B.m00 + m21 * B.m10 + m22 * B.m20 + m23 * B.m30;
	result.m21 = m20 * B.m01 + m21 * B.m11 + m22 * B.m21 + m23 * B.m31;
	result.m22 = m20 * B.m02 + m21 * B.m12 + m22 * B.m22 + m23 * B.m32;
	result.m23 = m20 * B.m03 + m21 * B.m13 + m22 * B.m23 + m23 * B.m33;
	result.m30 = m30 * B.m00 + m31 * B.m10 + m32 * B.m20 + m33 * B.m30;
	result.m31 = m30 * B.m01 + m31 * B.m11 + m32 * B.m21 + m33 * B.m31;
	result.m32 = m30 * B.m02 + m31 * B.m12 + m32 * B.m22 + m33 * B.m32;
	result.m33 = m30 * B.m03 + m31 * B.m13 + m32 * B.m23 + m33 * B.m33;
	return result;
}
mat4 mat4::operator &(const mat4 &other) const {
	mat4 result;
	result.m00 = m00 * other.m00; result.m01 = m01 * other.m01; result.m02 = m02 * other.m02; result.m03 = m03 * other.m03;
	result.m10 = m10 * other.m10; result.m11 = m11 * other.m11;	result.m12 = m12 * other.m12; result.m13 = m13 * other.m13;
	result.m20 = m20 * other.m20; result.m21 = m21 * other.m21;	result.m22 = m22 * other.m22; result.m23 = m23 * other.m23;
	result.m30 = m30 * other.m30; result.m31 = m31 * other.m31;	result.m32 = m32 * other.m32; result.m33 = m33 * other.m33;
	return result;
}
mat4 mat4::operator |(const mat4 &other) const {
	mat4 result;
	result.m00 = m00 / other.m00; result.m01 = m01 / other.m01; result.m02 = m02 / other.m02; result.m03 = m03 / other.m03;
	result.m10 = m10 / other.m10; result.m11 = m11 / other.m11; result.m12 = m12 / other.m12; result.m13 = m13 / other.m13;
	result.m20 = m20 / other.m20; result.m21 = m21 / other.m21; result.m22 = m22 / other.m22; result.m23 = m23 / other.m23;
	result.m30 = m30 / other.m30; result.m31 = m31 / other.m31; result.m32 = m32 / other.m32; result.m33 = m33 / other.m33;
	return result;
}
mat4 mat4::operator *(float s) const {
	mat4 result;
	result.m00 = s*m00; result.m01 = s*m01; result.m02 = s*m02; result.m03 = s*m03;
	result.m10 = s*m10; result.m11 = s*m11; result.m12 = s*m12; result.m13 = s*m13;
	result.m20 = s*m20; result.m21 = s*m21; result.m22 = s*m22; result.m23 = s*m23;
	result.m30 = s*m30; result.m31 = s*m31; result.m32 = s*m32; result.m33 = s*m33;
	return result;
}
vec4 mat4::operator *(const vec4 &v) const {
	vec4 r;
	r.x = ptr[0] * v.x + ptr[4] * v.y + ptr[8] * v.z + ptr[12] * v.w;
	r.y = ptr[1] * v.x + ptr[5] * v.y + ptr[9] * v.z + ptr[13] * v.w;
	r.z = ptr[2] * v.x + ptr[6] * v.y + ptr[10] * v.z + ptr[14] * v.w;
	r.w = ptr[3] * v.x + ptr[7] * v.y + ptr[11] * v.z + ptr[15] * v.w;
	return r;
}
vec3 mat4::operator *(const vec3 &v) const {
	vec3 r;
	r.x = ptr[0] * v.x + ptr[4] * v.y + ptr[8] * v.z + ptr[12];
	r.y = ptr[1] * v.x + ptr[5] * v.y + ptr[9] * v.z + ptr[13];
	r.z = ptr[2] * v.x + ptr[6] * v.y + ptr[10] * v.z + ptr[14];
	return r;
}
mat4 mat4::operator /(const mat4 &other) const {
	return (*this)*other.inverse();
}
mat4 mat4::operator !() const {
	return inverse();
}
/*mat4 operator ~() const {
return transposed();
}*/


//assignment operators
mat4 & mat4::operator = (const mat4 &other) {
	m00 = other.m00; m01 = other.m01; m02 = other.m02; m03 = other.m03;
	m10 = other.m10; m11 = other.m11; m12 = other.m12; m13 = other.m13;
	m20 = other.m20; m21 = other.m21; m22 = other.m22; m23 = other.m23;
	m30 = other.m30; m31 = other.m31; m32 = other.m32; m33 = other.m33;
	return *this;
}
mat4 & mat4::operator += (const mat4 &other) {
	m00 += other.m00; m01 += other.m01; m02 += other.m02; m03 += other.m03;
	m10 += other.m10; m11 += other.m11; m12 += other.m12; m13 += other.m13;
	m20 += other.m20; m21 += other.m21; m22 += other.m22; m23 += other.m23;
	m30 += other.m30; m31 += other.m31; m32 += other.m32; m33 += other.m33;
	return *this;
}
mat4 & mat4::operator -= (const mat4 &other) {
	m00 -= other.m00; m01 -= other.m01; m02 -= other.m02; m03 -= other.m03;
	m10 -= other.m10; m11 -= other.m11; m12 -= other.m12; m13 -= other.m13;
	m20 -= other.m20; m21 -= other.m21; m22 -= other.m22; m23 -= other.m23;
	m30 -= other.m30; m31 -= other.m31; m32 -= other.m32; m33 -= other.m33;
	return *this;
}
mat4 & mat4::operator *= (const mat4 &B) {
	return (*this) = (*this)*B;
}
mat4 & mat4::operator /= (const mat4 &other) {
	return (*this) = (*this)*other.inverse();
}
mat4 & mat4::operator /= (float s) {
	m00 /= s; m01 /= s; m02 /= s; m03 /= s;
	m10 /= s; m11 /= s; m12 /= s; m13 /= s;
	m20 /= s; m21 /= s; m22 /= s; m23 /= s;
	m30 /= s; m31 /= s; m32 /= s; m33 /= s;
	return *this;
}
mat4 & mat4::operator *= (float s) {
	m00 *= s; m01 *= s; m02 *= s; m03 *= s;
	m10 *= s; m11 *= s; m12 *= s; m13 *= s;
	m20 *= s; m21 *= s; m22 *= s; m23 *= s;
	m30 *= s; m31 *= s; m32 *= s; m33 *= s;
	return *this;
}
mat4 & mat4::operator &= (const mat4 &other) {
	m00 *= other.m00; m01 *= other.m01; m02 *= other.m02; m03 *= other.m03;
	m10 *= other.m10; m11 *= other.m11; m12 *= other.m12; m13 *= other.m13;
	m20 *= other.m20; m21 *= other.m21; m22 *= other.m22; m23 *= other.m23;
	m30 *= other.m30; m31 *= other.m31; m32 *= other.m32; m33 *= other.m33;
	return *this;
}
mat4 & mat4::operator |= (const mat4 &other) {
	m00 /= other.m00; m01 /= other.m01; m02 /= other.m02; m03 /= other.m03;
	m10 /= other.m10; m11 /= other.m11; m12 /= other.m12; m13 /= other.m13;
	m20 /= other.m20; m21 /= other.m21; m22 /= other.m22; m23 /= other.m23;
	m30 /= other.m30; m31 /= other.m31; m32 /= other.m32; m33 /= other.m33;
	return *this;
}

//comparison operators
bool mat4::operator == (const mat4 &other) {
	return	m00 == other.m00 && m01 == other.m01 && m02 == other.m02 && m03 == other.m03 &&
		m10 == other.m10 && m11 == other.m11 && m12 == other.m12 && m13 == other.m13 &&
		m20 == other.m20 && m21 == other.m21 && m22 == other.m22 && m23 == other.m23 &&
		m30 == other.m30 && m31 == other.m31 && m32 == other.m32 && m33 == other.m33;

}
bool mat4::operator != (const mat4 &other) {
	return	m00 != other.m00 && m01 != other.m01 && m02 != other.m02 && m03 != other.m03 &&
		m10 != other.m10 && m11 != other.m11 && m12 != other.m12 && m13 != other.m13 &&
		m20 != other.m20 && m21 != other.m21 && m22 != other.m22 && m23 != other.m23 &&
		m30 != other.m30 && m31 != other.m31 && m32 != other.m32 && m33 != other.m33;
}


//accessor operators
float* mat4::operator [](int col) {
	return m[col];
}
const float* mat4::operator [](int col) const {
	return m[col];
}
float& mat4::operator ()(int row, int col) {
	return m[col][row];
}
const float& mat4::operator ()(int row, int col) const {
	return m[col][row];
}


void	mat4::setCol(int col, const vec4 &v4) {
	m[col][0] = v4.x;
	m[col][1] = v4.y;
	m[col][2] = v4.z;
	m[col][3] = v4.w;
}
void	mat4::setCol(int col, const vec3 &v3) {
	m[col][0] = v3.x;
	m[col][1] = v3.y;
	m[col][2] = v3.z;
}

#endif