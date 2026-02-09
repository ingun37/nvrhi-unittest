struct MaterialData
{
    Texture2D texture00;
    Texture2D texture01;
    Texture2D texture02;
    Texture2D texture03;
    Texture2D texture04;
    Texture2D texture05;
    Texture2D texture06;
    Texture2D texture07;
    Texture2D texture08;
    Texture2D texture09;
    Texture2D texture0A;
    Texture2D texture0B;
    Texture2D texture0C;
    Texture2D texture0D;
    Texture2D texture0E;
    Texture2D texture0F;
    Texture2D texture10;
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
	return float4(0.0,0.0,0.0, 1.0);
}
