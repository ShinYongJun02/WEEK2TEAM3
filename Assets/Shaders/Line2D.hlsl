cbuffer Constants : register(b0) // FConstants
{
	row_major matrix projection;
	float4 color;
	float2 start;
	float2 end;
	float thickness;
	float3 padding;
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float2 Perpendicular(float2 v)
{
	return float2(-v.y, v.x);
}

// Vertex Shader
PS_INPUT mainVS(uint vertex_id : SV_VertexID)
{
	PS_INPUT output;
	
	float2 normal = normalize(Perpendicular(end - start));
	float2 offset = normal * thickness * 0.5f;

	float2 positions[6] =
	{
		start + offset,
        start - offset,
        end - offset,
        start + offset,
        end - offset,
        end + offset
	};

	output.position = mul(float4(positions[vertex_id], 0.f, 1.f), projection);
	output.color = color;
	
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	return input.color;
}
