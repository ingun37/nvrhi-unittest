static const float2 g_positions[] = {
	float2(-0.5, -0.5),
	float2(0, 0.5),
	float2(0.5, -0.5)
};

struct VertexOutput
{
    float4 position : SV_POSITION;
}

struct PixelOutput
{
    float4 c0: SV_Target0;
    float4 c1: SV_Target1;
    float4 c2: SV_Target2;
    float4 c3: SV_Target3;
};


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
PixelOutput ps(VertexOutput input)
{
    PixelOutput output;
    output.c0 = float4(1.0,0.0,0.0,1.0);
    output.c1 = float4(1.0,0.0,0.0,1.0);
    output.c2 = float4(1.0,0.0,0.0,1.0);
    output.c3 = float4(1.0,0.0,0.0,1.0);
	return output;
}
