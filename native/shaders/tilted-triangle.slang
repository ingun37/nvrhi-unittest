cbuffer Uniforms
{
    float4 color;
    float4x4 transform;
}

static const float3 g_positions[] = {
	float3(-0.5, -0.5, 0.0),
	float3(0, 0.5, 1.0),
	float3(0.5, -0.5, 0.0)
};

struct VertexOutput
{
    float4 position : SV_POSITION;
}

[shader("vertex")]
VertexOutput vs(
	uint i_vertexId : SV_VertexID,
)
{
	VertexOutput o;
	o.position = mul(transform, float4(g_positions[i_vertexId], 1.0));
	return o;
}
[shader("fragment")]
float4 ps(VertexOutput input): SV_Target0
{
	return color;
}
