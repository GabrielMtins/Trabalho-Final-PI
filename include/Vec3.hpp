#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include <cstdio>

/**
 * Estrutura simples de Vec3 que utilizo nos meus projetos.
 * Os nomes dos métodos são autoexplicativos.
 */
struct Vec3 {
	float x, y, z;

	Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
	Vec3(float x, float y, float z): x(x), y(y), z(z) {}

	inline Vec3 operator+(const Vec3& other) const {
		return Vec3(x + other.x, y + other.y, z + other.z);
	}

	inline Vec3 operator-(const Vec3& other) const {
		return Vec3(x - other.x, y - other.y, z - other.z);
	}

	inline Vec3 operator*(const Vec3& other) const {
		return Vec3(x * other.x, y * other.y, z * other.z);
	}

	inline Vec3 operator*(float val) const {
		return Vec3(x * val, y * val, z * val);
	}

	inline Vec3 operator/(float val) const {
		return Vec3(x / val, y / val, z / val);
	}

	inline Vec3& operator+=(const Vec3& other) {
		x += other.x;
		y += other.y;
		z += other.z;

		return *this;
	}

	inline Vec3& operator-=(const Vec3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;

		return *this;
	}

	inline Vec3& operator*=(const Vec3& other) {
		x *= other.x;
		y *= other.y;
		z *= other.z;

		return *this;
	}

	inline Vec3& operator*=(float val) {
		x *= val;
		y *= val;
		z *= val;

		return *this;
	}

	inline Vec3& operator/=(float val) {
		x /= val;
		y /= val;
		z /= val;

		return *this;
	}

	inline bool operator==(const Vec3& other) {
		return other.x == x && other.y == y && z == other.z;
	}

	inline float lengthSqr(void) const {
		return x * x + y * y + z * z;
	}

	inline float length(void) const {
		return sqrtf(x * x + y * y + z * z);
	}

	inline Vec3 normalized(void) const {
		return *this / length();
	}

	inline float dot(const Vec3 &other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	inline Vec3 cross(const Vec3& other) const {
		return Vec3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	inline Vec3 rotateX(float cos_angle, float sin_angle) const {
		return Vec3(
				x,
				y * cos_angle - z * sin_angle,
				y * sin_angle + z * cos_angle
				);
	}

	inline Vec3 rotateY(float cos_angle, float sin_angle) const {
		return Vec3(
				x * cos_angle + z * sin_angle,
				y,
				-x * sin_angle + z * cos_angle
				);
	}

	inline void print(void) const {
		printf("%f %f %f\n", x, y, z);
	}

	static const Vec3 zero;
};

#endif
