cbuffer modelConstants : register(b0)
{
	row_major matrix model;
	row_major matrix view_projection;
	float3 outline_color;
	float thickness;
}

struct VS_INPUT
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// Vertex Shader
PS_INPUT mainVS(VS_INPUT input)
{
	PS_INPUT output;
	
	float4x4 outline_scale = float4x4(
		1.0f + thickness, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f + thickness, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f + thickness, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	
	float4x4 mvp = mul(mul(outline_scale, model), view_projection);
	
	output.position = mul(input.position, mvp);
	output.color = float4(outline_color, 1.0f);
    
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	return input.color;
}
