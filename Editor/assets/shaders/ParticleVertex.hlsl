struct VSout {
	float4 pos : SV_Position;
	float3 velocity : VELOCITY;
	float lifeTime : LIFETIME;
};

cbuffer CameraTransform : register(b0) {
	matrix viewproj;
	matrix view;
	matrix proj;
};

cbuffer Modeltransform : register(b1) {
	matrix model;
}

VSout main(float3 p : POSITION, float3 v : VELOCITY, float t : LIFETIME) {
	VSout vso;
	vso.pos = mul(mul(float4(p, 1.0), model), view);
	vso.velocity = v;
	vso.lifeTime = t;
	return vso;
}