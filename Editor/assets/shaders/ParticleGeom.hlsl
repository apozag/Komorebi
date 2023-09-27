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

[maxvertexcount(4)]
void main(
	point VSout input[1],
	inout TriangleStream< GSout > output
)
{	

	const float size = 5.f;

	float4 worldPos = mul(input[0].pos, viewproj);

	GSout p0;
	p0.pos = worldPos + float4(size, -size, 0, 0);
	p0.velocity = input[0].velocity;
	p0.lifeTime = input[0].lifeTime;
	output.Append(p0);

	GSout p1;
	p1.pos = worldPos + float4(size, size, 0, 0);
	p1.velocity = input[0].velocity;
	p1.lifeTime = input[0].lifeTime;
	output.Append(p1);

	GSout p2;
	p2.pos = worldPos + float4(-size, -size, 0, 0);
	p2.velocity = input[0].velocity;
	p2.lifeTime = input[0].lifeTime;
	output.Append(p2);

	GSout p3;
	p3.pos = worldPos + float4(-size, size, 0, 0);
	p3.velocity = input[0].velocity;
	p3.lifeTime = input[0].lifeTime;
	output.Append(p3);
}