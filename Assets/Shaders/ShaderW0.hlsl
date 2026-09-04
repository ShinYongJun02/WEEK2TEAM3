cbuffer constants : register(b0) // FConstants
{
	row_major matrix ModelMatrix;
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
    
	output.position = mul(input.position, ModelMatrix);
	output.color = input.color;
    
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET 
{
	return input.color;
}