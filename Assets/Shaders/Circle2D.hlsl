cbuffer Constants : register(b0) // FConstants
{
	row_major matrix projection;
	float4 color;
	float2 center;
	float radius;
	float padding[2];
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 screen_pos : TEXCOORD0;
};

// Vertex Shader
PS_INPUT mainVS(uint vertex_id : SV_VertexID)
{
	PS_INPUT output;
	
	float2 positions[6] =
	{
		float2(-radius, radius),
		float2(radius, radius),
		float2(radius, -radius),
		float2(-radius, radius),
		float2(radius, -radius),
		float2(-radius, -radius),
	};
	
	float2 screen_pos = positions[vertex_id] + center;

	output.screen_pos = screen_pos;
	output.position = mul(float4(screen_pos, 0.f, 1.f), projection);
	output.color = color;
	
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	if (distance(input.screen_pos, center) > radius)
	{
		discard;
	}
	return input.color;
}
