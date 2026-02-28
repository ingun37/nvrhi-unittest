cbuffer Uniforms
{
    float4 color;
    float4x4 transform;
}

StructuredBuffer<float4> vertices;

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
	o.position = mul(transform, float4(vertices[i_vertexId].xyz, 1.0));
	return o;
}
[shader("fragment")]
float4 ps(VertexOutput input): SV_Target0
{
	return color;
}
