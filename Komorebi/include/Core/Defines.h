#pragma once

#include <math.h>

namespace POD {
	struct Vector2 {
		float x, y;
	};

	struct Vector3 {
		float x, y, z;

		inline float mangitude() const { return sqrt(sqrMangitude()); }
		inline float sqrMangitude() const { return x * x + y * y + z * z; }		

		inline void operator+=(const Vector3& v) { x+=v.x; y+=v.y; z+=v.z; }
		inline void operator-=(const Vector3& v) { x-=v.x; y-=v.y; z-=v.z; }
		inline void operator*=(const Vector3& v) { x*=v.x; y*=v.y; z*=v.z; }
		inline void operator*=(float f) { x*=f; y*=f; z*=f; }
	};

	inline Vector3 operator-(const Vector3& v0, const Vector3& v1) { return { v0.x - v1.x, v0.y - v1.y, v0.z - v1.z }; }
	inline Vector3 operator+(const Vector3& v0, const Vector3& v1) { return { v0.x + v1.x, v0.y + v1.y, v0.z + v1.z }; }	
	inline Vector3 operator*(const Vector3& v0, const Vector3& v1) { return { v0.x * v1.x, v0.y * v1.y, v0.z * v1.z }; }
	inline Vector3 operator*(const Vector3& v, float f) { return { v.x * f, v.y * f, v.z * f }; }

	struct Vector4 {
		float x, y, z, w;
	};

	struct Vertex {
		Vector3 pos;
		Vector3 normal;
		Vector3 tangent;
		Vector2 uv;
	};

	struct Particle {
		Vector3 pos;
		Vector3 velocity;
		float lifeTime;
	};

#define MAX_BONES_PER_VERTEX 4

	struct SkinnedVertex {
		Vector3 pos;
		Vector3 normal;
		Vector3 tangent;
		Vector2 uv;	
		int boneIdx[MAX_BONES_PER_VERTEX];
		float weight[MAX_BONES_PER_VERTEX];
	};
}