#pragma once
#include "matrix.h"
#include <cmath>


vec2::vec2() {
	X = 0;
	Y = 0;
}
vec2::vec2(float _x, float _y) {
	X = _x;
	Y = _y;
}
vec4::vec4() {
	X = 0;
	Y = 0;
	Z = 0;
	W = 0;
}
vec4::vec4(float a, float b, float c, float d) {
	X = a;
	Y = b;
	Z = c;
	W = d;
}
vec3::vec3() {
	X = 0;
	Y = 0;
	Z = 0;
}
vec3::vec3(const char* a) {
	int val = std::stoi(a, nullptr, 16);
	unsigned char r = 0, g = 0, b = 0;
	b = val & 0xFF;
	val = val >> 8;
	g = val & 0xFF;
	val = val >> 8;
	r = val & 0xFF;
	val = val >> 8;
	X = float(r) / 256.0f;
	Y = float(g) / 256.0f;
	Z = float(b) / 256.0f;
}
vec3::vec3(float a) {
	X = a;
	Y = a;
	Z = a;
}
vec3::vec3(float a, float b, float c) {
	X = a;
	Y = b;
	Z = c;
}
float vec3::gmag() {
	return sqrtf((X*X) + (Y*Y) + (Z*Z));
}
vec3 vec3::smag(float mag) {
	float f = mag / gmag();
	return vec3(X * f, Y * f, Z * f);
}

mat4::mat4() {
	this->m[0][0] = 1.0f;
	this->m[0][1] = 0.0f;
	this->m[0][2] = 0.0f;
	this->m[0][3] = 0.0f;
	this->m[1][0] = 0.0f;
	this->m[1][1] = 1.0f;
	this->m[1][2] = 0.0f;
	this->m[1][3] = 0.0f;
	this->m[2][0] = 0.0f;
	this->m[2][1] = 0.0f;
	this->m[2][2] = 1.0f;
	this->m[2][3] = 0.0f;
	this->m[3][0] = 0.0f;
	this->m[3][1] = 0.0f;
	this->m[3][2] = 0.0f;
	this->m[3][3] = 1.0f;
}
mat4::mat4(
	float x0, float x1, float x2, float x3,
	float y0, float y1, float y2, float y3,
	float z0, float z1, float z2, float z3,
	float w0, float w1, float w2, float w3
) {
	this->m[0][0] = x0;
	this->m[0][1] = x1;
	this->m[0][2] = x2;
	this->m[0][3] = x3;
	this->m[1][0] = y0;
	this->m[1][1] = y1;
	this->m[1][2] = y2;
	this->m[1][3] = y3;
	this->m[2][0] = z0;
	this->m[2][1] = z1;
	this->m[2][2] = z2;
	this->m[2][3] = z3;
	this->m[3][0] = w0;
	this->m[3][1] = w1;
	this->m[3][2] = w2;
	this->m[3][3] = w3;
}
mat4 mat4::identity() {
	return mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
};
mat4 mat4::yrot(float rad) {
	return mat4(
		cos(rad), 0.0f, sin(rad), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(rad), 0.0f, cos(rad), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
};
mat4 mat4::xrot(float rad) {
	return mat4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(rad), -sin(rad), 0.0f,
		0.0f, sin(rad), cos(rad), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
};
mat4 mat4::zrot(float rad) {
	return mat4(
		cos(rad), -sin(rad), 0.0f, 0.0f,
		sin(rad), cos(rad), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
};
mat4 mat4::yxz(float rad_x, float rad_y, float rad_z) {
	return  zrot(rad_z) * xrot(rad_x) * yrot(rad_y);
}
mat4 mat4::zxy(float rad_x, float rad_y, float rad_z) {
	return  yrot(rad_y) * xrot(rad_x) * zrot(rad_z);
}
mat4 mat4::transform(vec3 trans, vec3 scale, vec3 rot) {
	return  mat4::yxz(rot.X, rot.Y, rot.Z) * mat4(
		scale.X, 0.0f, 0.0f, trans.X,
		0.0f, scale.Y, 0.0f, trans.Y,
		0.0f, 0.0f, scale.Z, trans.Z,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

col3::col3()
{
	R = 0;
	G = 0;
	B = 0;
}

col3::col3(float r, float g, float b)
{
	R = r;
	G = g;
	B = b;
}

col3::col3(float c)
{
	R = c;
	G = c;
	B = c;
}

bool vec3::operator==(const vec3& other) const
{
	return X == other.X && Y == other.Y && Z == other.Z;
}
float randf() {
	return float(rand()) / float(RAND_MAX);
}
float randnf() {
	return (2 * randf()) - 1;
}

float randf_range(float min, float max)
{
	return (randf() * (max - min)) + min;
}

float ease_curve(const std::string& Curve, float t) {
	if (Curve == "linear") {
		return t;
	}
	else if (Curve == "easein") {
		return t * t;
	}
	else if (Curve == "easeout") {
		return t * (2 - t);
	}
	else if (Curve == "easeinout") {
		if (t < 0.5f) {
			return 2 * t * t;
		}
		else {
			return -1 + (4 - 2 * t) * t;
		}
	}
	else {
		// Default to linear if unknown curve
		return t;
	}
}

vec3 ease_vec3(const std::string& Curve, float t, const vec3& start, const vec3& end) {
	float eased_t = ease_curve(Curve, t);
	return start * (1.0f - eased_t) + end * eased_t;
}
vec3 vec3::random() {
	return vec3(randnf(), randnf(), randnf());
}
vec3 vec3::randomdir() {
	return vec3::random().smag(1.0f);
}

col3 ease_col3(const std::string& Curve, float t, const col3& start, const col3& end) {
	float eased_t = ease_curve(Curve, t);
	return col3(
		start.R * (1.0f - eased_t) + end.R * eased_t,
		start.G * (1.0f - eased_t) + end.G * eased_t,
		start.B * (1.0f - eased_t) + end.B * eased_t
	);
}

mat2::mat2() {
	this->m[0][0] = 1.0f;
	this->m[0][1] = 0.0f;
	this->m[1][0] = 0.0f;
	this->m[1][1] = 1.0f;
}
mat2::mat2(
	float x0, float x1,
	float y0, float y1) {
	this->m[0][0] = x0;
	this->m[0][1] = x1;
	this->m[1][0] = y0;
	this->m[1][1] = y1;
}
mat3::mat3() {
	this->m[0][0] = 1.0f;
	this->m[0][1] = 0.0f;
	this->m[0][2] = 0.0f;
	this->m[1][0] = 0.0f;
	this->m[1][1] = 1.0f;
	this->m[1][2] = 0.0f;
	this->m[2][0] = 0.0f;
	this->m[2][1] = 0.0f;
	this->m[2][2] = 1.0f;
}

mat3::mat3(
	float x0, float x1, float x2,
	float y0, float y1, float y2,
	float z0, float z1, float z2
) {
	m[0][0] = x0;
	m[0][1] = x1;
	m[0][2] = x2;
	m[1][0] = y0;
	m[1][1] = y1;
	m[1][2] = y2;
	m[2][0] = z0;
	m[2][1] = z1;
	m[2][2] = z2;
}

ivec3::ivec3() {
	X = 0;
	Y = 0;
	Z = 0;
}

ivec3::ivec3(int a) {
	X = a;
	Y = a;
	Z = a;
}
ivec3::ivec3(int a, int b, int c) {
	X = a;
	Y = b;
	Z = c;
}
