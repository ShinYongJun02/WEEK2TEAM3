cbuffer Constants : register(b0) // FConstants
{
	row_major matrix view_projection;
	float3 camera_position;
	float padding;
}

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 world_position : TEXCOORD0;
};

// Vertex Shader
PS_INPUT mainVS(uint vertex_id : SV_VertexID)
{
	PS_INPUT output;
	
	float3 positions[6] =
	{
		float3(-1, 1, 0),
		float3(1, 1, 0),
		float3(1, -1, 0),
		float3(-1, 1, 0),
		float3(1, -1, 0),
		float3(-1, -1, 0)
	};
		
	float3 world_position = positions[vertex_id] * 10.0 + float3(camera_position.x, camera_position.y, 0.0f);
	
	output.world_position = world_position;
	output.position = mul(float4(world_position, 1.f), view_projection);
	
	return output;
}

// Pixel Shader
float4 mainPS(PS_INPUT input) : SV_TARGET
{
	const float cell_size = 1.0f;
	const float half_line_width = 0.01f;
	
	float x = input.world_position.x;
	float y = input.world_position.y;
#if false
	if (abs(x) < half_line_width)
	{
		return float4(0.f, 1.f, 0.f, 1.f); // Red for Y-axis
	}
	else if (abs(y) < half_line_width)
	{
		return float4(1.f, 0.f, 0.f, 1.f); // Green for X-axis
	}

	float x_mod = fmod(x, cell_size);
	float y_mod = fmod(y, cell_size);
	
	if (abs(x_mod) > half_line_width && abs(y_mod) > half_line_width)
	{
		discard;
	}
	
	return float4(0.4f, 0.4f, 0.4f, 1.f);
#else
	if (abs(x) < half_line_width)
	{
		return float4(0.f, 1.f, 0.f, 1.f); // Red for Y-axis
	}
	else if (abs(y) < half_line_width)
	{
		return float4(1.f, 0.f, 0.f, 1.f); // Green for X-axis
	}
	
	float2 grid_xy = float2(x, y) / cell_size;
	float2 dist_to_nearest_grid = abs(frac(grid_xy + 0.5) - float2(0.5, 0.5));
	float2 pixel_width = max(fwidth(grid_xy), float2(0.00001f, 0.00001f));
	float2 line_width = (dist_to_nearest_grid - half_line_width) / pixel_width;
	float alpha = 1.f - smoothstep(0.f, 1.f, min(line_width.x, line_width.y));
	                                       
	return float4(0.4f, 0.4f, 0.4f, alpha);
#endif
}
