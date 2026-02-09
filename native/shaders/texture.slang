struct MaterialData
{
    Texture2D texture0;
    Texture2D texture1;
    Texture2D texture2;
    Texture2D texture3;
    Texture2D texture4;
    Texture2D texture5;
    Texture2D texture6;
    Texture2D texture7;
    Texture2D texture8;
    Texture2D texture9;
    Texture2D textureA;
    Texture2D textureB;
    Texture2D textureC;
    Texture2D textureD;
    Texture2D textureE;
    Texture2D textureF;
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
