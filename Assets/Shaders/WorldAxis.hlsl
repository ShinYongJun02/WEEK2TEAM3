cbuffer Constants : register(b0) // FConstants
{
	row_major matrix view;
	row_major matrix projection;
	float4 color;
	float3 axis;
	float thickness;
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
	
	const float halfLength = 10000.0f;
	const float view_thickness = thickness * 0.01f;

	float3 min_pos = axis * -halfLength;
	float4 view_min_pos = mul(float4(min_pos, 1.f), view);
	
	float3 max_pos = axis * halfLength;
	float4 view_max_pos = mul(float4(max_pos, 1.f), view);
	
	float2 delta = normalize(view_max_pos.xy - view_min_pos.xy);
	float len = length(delta);
	float2 perpendicular = float2(-delta.y, delta.x) / len;
	
	float2 offset = perpendicular * view_thickness * 0.5f;
	
	float4 view_positions[6] =
	{
		view_min_pos + float4(offset, 0.f, 0.f),
		view_max_pos + float4(offset, 0.f, 0.f),
		view_max_pos - float4(offset, 0.f, 0.f),
		view_min_pos + float4(offset, 0.f, 0.f),
		view_max_pos - float4(offset, 0.f, 0.f),
		view_min_pos - float4(offset, 0.f, 0.f)
	};
	
	float4 projected_position = mul(view_positions[vertex_id], projection);

	output.position = projected_position;
	output.color = color;
	
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	return input.color;
}
