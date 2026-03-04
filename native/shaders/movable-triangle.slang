struct Uniforms
{
    float4 color;
    float4x4 transform;
};

ConstantBuffer<Uniforms> uniforms;

static const float2 g_positions[] = {
	float2(-0.5, -0.5),
	float2(0, 0.5),
	float2(0.5, -0.5)
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
	o.position = mul(uniforms.transform, float4(g_positions[i_vertexId], 0.5, 1.0));
	return o;
}
[shader("fragment")]
float4 ps(VertexOutput input): SV_Target0
{
	return uniforms.color;
}
