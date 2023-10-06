struct VSout {
	float4 pos : SV_Position;
	float3 velocity : VELOCITY;
	float lifeTime : LIFETIME;
};

struct GSout {
	float4 pos : SV_Position;
	float3 velocity : VELOCITY;
	float lifeTime : LIFETIME;
};

cbuffer CameraTransform : register(b0) {
	matrix viewproj;
	matrix view;
	matrix proj;
};

cbuffer QuadSize : register(b1) {
	float quadSize = 1.f;
};

[maxvertexcount(4)]
void main(
	point VSout input[1],
	inout TriangleStream< GSout > output
)
{	
	if (input[0].lifeTime <= 0.f) {
		return;
	}

	//float4 worldPos = mul(input[0].pos, viewproj);
	float4 worldPos = input[0].pos;
	//float4 worldPos = mul(input[0].pos, proj);

	GSout p0;
	p0.pos = worldPos + float4(quadSize, -quadSize, 0, 0);
	p0.pos = mul(p0.pos, proj);
	p0.velocity = input[0].velocity;
	p0.lifeTime = input[0].lifeTime;
	output.Append(p0);

	GSout p1;
	p1.pos = worldPos + float4(quadSize, quadSize, 0, 0);
	p1.pos = mul(p1.pos, proj);
	p1.velocity = input[0].velocity;
	p1.lifeTime = input[0].lifeTime;
	output.Append(p1);

	GSout p2;
	p2.pos = worldPos + float4(-quadSize, -quadSize, 0, 0);
	p2.pos = mul(p2.pos, proj);
	p2.velocity = input[0].velocity;
	p2.lifeTime = input[0].lifeTime;
	output.Append(p2);

	GSout p3;
	p3.pos = worldPos + float4(-quadSize, quadSize, 0, 0);
	p3.pos = mul(p3.pos, proj);
	p3.velocity = input[0].velocity;
	p3.lifeTime = input[0].lifeTime;
	output.Append(p3);
}