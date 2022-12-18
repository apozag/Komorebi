cbuffer Color : register(b7) {
	float4 m_color;
	float4 bla;
	float4 ble;
	float4 bli;
};

float4 main() : SV_TARGET
{
	return m_color;//float4(1.0f, 0.0f, 0.0f, 1.0f);
}