cbuffer Constants : register(b0) // FConstants
{
	row_major matrix projection;
	float4 color;
	float2 center;
	float size;
	float rotation;
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// Vertex Shader
PS_INPUT mainVS(uint vertex_id : SV_VertexID)
{
	PS_INPUT output;
	
	float2 positions[3] =
	{
		float2(-0.5, -0.5),
		float2(0.0, 0.3660254),
		float2(0.5, -0.5),
	};
	
	float2x2 rotation_matrix = float2x2(
	   cos(rotation), sin(rotation),
	   -sin(rotation), cos(rotation)
	);
	
	float2 position = positions[vertex_id];
	position *= size;
	position = mul(position, rotation_matrix);
	position += center;
	
	output.position = mul(float4(position, 0.f, 1.f), projection);
	output.color = color;
	
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	return input.color;
}
