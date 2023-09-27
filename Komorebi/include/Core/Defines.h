#pragma once

namespace POD {
	struct Vector2 {
		float x, y;
	};

	struct Vector3 {
		float x, y, z;
	};

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