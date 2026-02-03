struct MaterialData
{
    float3 baseColor;
    float metalness;    // padding handled automatically by alignment
    float roughness;
    float _pad0;        // explicit padding for alignment
    float _pad1;
    float _pad2;
};

struct MaterialParamsStruct
{
    ConstantBuffer<MaterialData> data;
};

ParameterBlock<MaterialParamsStruct> g_material;

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
	o.position = float4(g_positions[i_vertexId], 0.0, 1.0);
	return o;
}
[shader("fragment")]
float4 ps(VertexOutput input): SV_Target0
{
	return float4(1.0, 0.5, 0.2, 1.0);
}
