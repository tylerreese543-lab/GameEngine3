#pragma once
#include <string>
#include <iostream>
#include <map>

#define PI (float)3.1415926535f
struct mat3;

struct vec2 {
	float X, Y;

	vec2();
	vec2(float, float);

		inline vec2 operator* (const float& r) const {
		return vec2(X * r, Y * r);
	}
	inline vec2 operator+ (const vec2& r) const {
		return vec2(X + r.X, Y + r.Y);
	}
	inline vec2 operator - (const vec2& r) const {
		return vec2(X - r.X, Y - r.Y);
	}
	inline vec2 operator - () const {
		return vec2(-X, -Y);
	}
	inline vec2 operator* (const vec2& r) const {
		return vec2(X * r.X, Y * r.Y);
	}
	inline vec2 operator/ (const vec2& r) const {
		return vec2(X / r.X, Y * r.Y);
	}
};
struct vec4 {
	float X;
	float Y;
	float Z;
	float W;

	vec4();
	vec4(float, float, float, float);
};
struct col3 {
	float R;
	float G;
	float B;

	col3();
	col3(float, float, float);
	col3(float);
};

struct ivec3 {
	int X;
	int Y;
	int Z;
	ivec3();
	ivec3(int, int, int);
	ivec3(int);
};
struct vec3 {
	float X;
	float Y;
	float Z;

	vec3();
	vec3(float, float, float);
	vec3(float);
	float gmag();
	vec3 smag(float);
	vec3(const char*);
	inline vec3 operator* (const float& r) const {
		return vec3(X * r, Y * r, Z * r);
	}
	inline vec3 operator+ (const vec3& r) const {
		return vec3(X + r.X, Y + r.Y, Z + r.Z);
	}
	inline vec3 operator - (const vec3& r) const {
		return vec3(X - r.X, Y - r.Y, Z - r.Z);
	}
	inline vec3 operator - () const {
		return vec3(-X, -Y, -Z);
	}
	inline vec3 operator* (const vec3& r) const {
		return vec3(X * r.X, Y * r.Y, Z * r.Z);
	}
	inline vec3 operator/ (const vec3& r) const {
		return vec3(X / r.X, Y * r.Y, Z * r.Z);
	}
	inline vec3 operator* (mat3& mat) const;
	static vec3 randomdir();
	static vec3 random();
	inline void operator += (const vec3& r) {
		X += r.X;
		Y += r.Y;
		Z += r.Z;
	}
	inline void operator -= (const vec3& r) {
		X -= r.X;
		Y -= r.Y;
		Z -= r.Z;
	}
	inline void operator *= (const vec3& r) {
		X *= r.X;
		Y *= r.Y;
		Z *= r.Z;
	}
	vec3 cross(const vec3& other) const {
		return vec3(
			Y * other.Z - Z * other.Y,
			Z * other.X - X * other.Z,
			X * other.Y - Y * other.X
		);
	}
	float dot(const vec3& other) const {
		return X * other.X + Y * other.Y + Z * other.Z;
	}
	bool operator==(const vec3& other) const;
};
struct mat4 {
	float m[4][4];

	static mat4 identity();
	static mat4 yrot(float);
	static mat4 xrot(float);
	static mat4 zrot(float);
	static mat4 yxz(float, float, float);
	static mat4 zxy(float, float, float);
	static mat4 transform(vec3 trans, vec3 scale, vec3 rot);
	mat4();
	mat4(
		float, float, float, float,
		float, float, float, float,
		float, float, float, float,
		float, float, float, float
	);
	inline mat4 operator* (const mat4& right) const {
		mat4 return_matrix;
		for (unsigned int i = 0; i < 4; i++) {
			for (unsigned int j = 0; j < 4; j++) {
				return_matrix.m[i][j] = m[i][0] * right.m[0][j] + m[i][1] * right.m[1][j] + m[i][2] * right.m[2][j] + m[i][3] * right.m[3][j];
			}
		}
		return return_matrix;
	}
	inline vec4 operator* (vec4& vec) const {
		vec4 returnv4;
		//TODO: replace this with column major multiplication
		returnv4.X = vec.X * m[0][0] + vec.Y * m[1][0] + vec.Z * m[2][0] + vec.W * m[3][0];
		returnv4.Y = vec.X * m[0][1] + vec.Y * m[1][1] + vec.Z * m[2][1] + vec.W * m[3][1];
		returnv4.Z = vec.X * m[0][2] + vec.Y * m[1][2] + vec.Z * m[2][2] + vec.W * m[3][2];
		returnv4.W = vec.X * m[0][3] + vec.Y * m[1][3] + vec.Z * m[2][3] + vec.W * m[3][3];
	}
	inline vec3 operator* (vec3 vec) const {
		vec3 returnv3;
		returnv3.X = vec.X * m[0][0] + vec.Y * m[1][0] + vec.Z * m[2][0] + 1 * m[3][0];
		returnv3.Y = vec.X * m[0][1] + vec.Y * m[1][1] + vec.Z * m[2][1] + 1 * m[3][1];
		returnv3.Z = vec.X * m[0][2] + vec.Y * m[1][2] + vec.Z * m[2][2] + 1 * m[3][2];
		return returnv3;
	}

	operator const float* () const {
		return &m[0][0];
	};
};

struct mat3 {
	float m[3][3];
	mat3();
	mat3(
		float, float, float,
		float, float, float,
		float, float, float
	);

	static mat3 identity();
	static mat3 yrot(float rad) {
		return mat3(
			cos(rad), 0.0f, sin(rad),
			0.0f, 1.0f, 0.0f,
			-sin(rad), 0.0f, cos(rad)
		);
	};
	static mat3 xrot(float rad) {
		return mat3(
			1.0f, 0.0f, 0.0f,
			0.0f, cos(rad), -sin(rad),
			0.0f, sin(rad), cos(rad)
		);
	};
	static mat3 zrot(float rad) {
		return mat3(
			cos(rad), -sin(rad), 0.0f,
			sin(rad), cos(rad), 0.0f,
			0.0f, 0.0f, 1.0f
		);
	};

	inline operator const float* () const {
		return &m[0][0];
	};

	static mat3 yxz(float x, float y, float z) {
		return zrot(z) * xrot(x) * yrot(y);
	}
	static mat3 xyz(float x, float y, float z) {
		return zrot(z) * xrot(y) * yrot(x);
	}
	inline mat3 operator* (const mat3& right) const {
		mat3 return_matrix;
		for (unsigned int i = 0; i < 3; i++) {
			for (unsigned int j = 0; j < 3; j++) {
				return_matrix.m[i][j] = m[i][0] * right.m[0][j] + m[i][1] * right.m[1][j] + m[i][2] * right.m[2][j];
			}
		}
		return return_matrix;
	}
	inline vec3 operator* (const vec3& vec) const {
		vec3 returnv3;
		//std::cout << "Multiplying vec3 by mat3\n";
		returnv3.X = vec.X * m[0][0] + vec.Y * m[1][0] + vec.Z * m[2][0];
		returnv3.Y = vec.X * m[0][1] + vec.Y * m[1][1] + vec.Z * m[2][1];
		returnv3.Z = vec.X * m[0][2] + vec.Y * m[1][2] + vec.Z * m[2][2];
		return returnv3;
	}
	
	static mat3 scale(vec3 scale) {
		return mat3(
			scale.X, 0.0f, 0.0f,
			0.0f, scale.Y, 0.0f,
			0.0f, 0.0f, scale.Z
		);
	}
	static mat3 transform(vec3 scale, vec3 rot) {
		return mat3::scale(scale) * mat3::yxz(rot.X, rot.Y, rot.Z);
	};
};
float randf();
float randnf();
float randf_range(float min, float max);

/*
A map of vec3 classes and time stamps. Used to interpolate between vec3 values over time.
*/
struct vec3seq {
	std::map<float, vec3> sequence;


	/*
	Adds a vec3 to the sequence at the specified time stamp.
	@param time - The time stamp to add the vec3 at.
	@param value - The vec3 value to add.
	*/
	void add(float time, vec3 value) {
		sequence[time] = value;
	}
	/*
	Gets the interpolated vec3 value at the specified time.
	@param time - The time to get the interpolated vec3 value at.
	@return The interpolated vec3 value.
	*/
	vec3 get(float time) {
		if (sequence.empty()) {
			return vec3(0.0f);
		}
		auto it = sequence.lower_bound(time);
		if (it == sequence.begin()) {
			return it->second;
		}
		if (it == sequence.end()) {
			return std::prev(it)->second;
		}
		auto it_prev = std::prev(it);
		float t1 = it_prev->first;
		float t2 = it->first;
		vec3 v1 = it_prev->second;
		vec3 v2 = it->second;
		float factor = (time - t1) / (t2 - t1);
		return v1 * (1.0f - factor) + v2 * factor;
	}
};

/*
A map of float values and time stamps. Used to interpolate between float values over time.
*/

struct floatseq {
	std::map<float, float> sequence;
	/*
	Adds a float to the sequence at the specified time stamp.
	@param time - The time stamp to add the float at.
	@param value - The float value to add.
	*/
	void add(float time, float value) {
		sequence[time] = value;
	}
	/*
	Gets the interpolated float value at the specified time.
	@param time - The time to get the interpolated float value at.
	@return The interpolated float value.
	*/
	float get(float time) {
		if (sequence.empty()) {
			return 0.0f;
		}
		auto it = sequence.lower_bound(time);
		if (it == sequence.begin()) {
			return it->second;
		}
		if (it == sequence.end()) {
			return std::prev(it)->second;
		}
		auto it_prev = std::prev(it);
		float t1 = it_prev->first;
		float t2 = it->first;
		float v1 = it_prev->second;
		float v2 = it->second;
		float factor = (time - t1) / (t2 - t1);
		return v1 * (1.0f - factor) + v2 * factor;
	}
};

/*
A map of col3 classes and time stamps. Used to interpolate between col3 values over time.
*/

struct col3seq {
	std::map<float, col3> sequence;


	/*
	Adds a col3 to the sequence at the specified time stamp.
	@param time - The time stamp to add the col3 at.
	@param value - The col3 value to add.
	*/
	void add(float time, col3 value) {
		sequence[time] = value;
	}
	/*
	Gets the interpolated col3 value at the specified time.
	@param time - The time to get the interpolated col3 value at.
	@return The interpolated col3 value.
	*/
	col3 get(float time) {
		if (sequence.empty()) {
			return col3(0.0f);
		}
		auto it = sequence.lower_bound(time);
		if (it == sequence.begin()) {
			return it->second;
		}
		if (it == sequence.end()) {
			return std::prev(it)->second;
		}
		auto it_prev = std::prev(it);
		float t1 = it_prev->first;
		float t2 = it->first;
		col3 v1 = it_prev->second;
		col3 v2 = it->second;
		float factor = (time - t1) / (t2 - t1);
		return col3(
			v1.R * (1.0f - factor) + v2.R * factor,
			v1.G * (1.0f - factor) + v2.G * factor,
			v1.B * (1.0f - factor) + v2.B * factor
		);
	}
};

/*
A function that has built-in easing functions for smooth interpolations between values.

@param Curve - The type of easing curve to use. Options are:
"linear" - Linear interpolation.
"easein" - Ease-in interpolation.
"easeout" - Ease-out interpolation.
"easeinout" - Ease-in-out interpolation.

@param t - The normalized time value (0.0 to 1.0).
@return The eased value.
*/

float ease_curve(const std::string& Curve, float t);
vec3 ease_vec3(const std::string& Curve, float t, const vec3& start, const vec3& end);
col3 ease_col3(const std::string& Curve, float t, const col3& start, const col3& end);

struct mat2 {
	float m[2][2];
	mat2();
	mat2(
		float, float,
		float, float
	);

	operator const float* () const {
		return &m[0][0];
	};

	operator float* () {
		return &m[0][0];
	};

	vec2 operator* (const vec2& vec) const {
		vec2 returnv2;
		returnv2.X = vec.X * m[0][0] + vec.Y * m[1][0];
		returnv2.Y = vec.X * m[0][1] + vec.Y * m[1][1];
		return returnv2;
	}

	mat2 fromAngle(float angle) {
		float c = cosf(angle);
		float s = sinf(angle);
		return mat2(
			c, -s,
			s, c
		);
	}
};

inline vec3 vec3::operator* (mat3& mat) const {
	vec3 returnv3;
	returnv3.X = X * mat.m[0][0] + Y * mat.m[0][1] + Z * mat.m[0][2];
	returnv3.Y = X * mat.m[1][0] + Y * mat.m[1][1] + Z * mat.m[1][2];
	returnv3.Z = X * mat.m[2][0] + Y * mat.m[2][1] + Z * mat.m[2][2];
	return returnv3;
}