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
		
	float3 world_position = positions[vertex_id] * 50.0 + float3(camera_position.x, camera_position.y, 0.0f);
	
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
#if 0
	if (abs(x) < half_line_width)
	{
		return float4(0.f, 1.f, 0.f, 1.f); // Red for Y-axis
	}
	else if (abs(y) < half_line_width)
	{
		return float4(1.f, 0.f, 0.f, 1.f); // Green for X-axis
	}

	float x_mod = abs(fmod(x, cell_size));
	float y_mod = abs(fmod(y, cell_size));
	
	if (min(x_mod, cell_size - x_mod) > half_line_width && min(y_mod, cell_size - y_mod) > half_line_width)
	{
		discard;
	}
	
	return float4(0.4f, 0.4f, 0.4f, 1.f);
#else	
	float2 grid_xy = float2(x, y) / cell_size;
	float2 dist_to_nearest_grid = abs(frac(grid_xy + 0.5) - float2(0.5, 0.5));
	float2 pixel_width = max(fwidth(grid_xy), float2(0.00001f, 0.00001f));
	float2 line_width = (dist_to_nearest_grid - half_line_width) / pixel_width;
	float alpha = 1.f - smoothstep(0.f, 1.f, min(line_width.x, line_width.y));
	
	float3 color = float3(0.4f, 0.4f, 0.4f);
	if (abs(grid_xy.x) < 0.5f && line_width.x <= line_width.y)
	{
		color = float3(0.f, 1.f, 0.f); // Y축 (x = 0)
	}
	else if (abs(grid_xy.y) < 0.5f && line_width.y <= line_width.x)
	{
		color = float3(1.f, 0.f, 0.f); // X축 (y = 0)
	}

	return float4(color, alpha);
#endif
}
