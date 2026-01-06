// shaders.slang

//
// This file provides a simple vertex and fragment shader that can be compiled
// using Slang. This code should also be valid as HLSL, and thus it does not
// use any of the new language features supported by Slang.
//

// Uniform data to be passed from application -> shader.
cbuffer Uniforms
{
    float4x4 modelViewProjection;
}

// Per-vertex attributes to be assembled from bound vertex buffers.
struct AssembledVertex
{
    float3	position : POSITION;
    float2	uv    : UV;
};

// Output of the vertex shader, and input to the fragment shader.
struct CoarseVertex
{
    float2 uv;
};

// Output of the fragment shader
struct Fragment
{
    float4 color;
};

// Vertex  Shader

struct VertexStageOutput
{
    CoarseVertex    coarseVertex    : CoarseVertex;
    nointerpolation uint2 info : AAAA;
    float4          sv_position     : SV_Position;
};

struct FragmentStageInput
{
    CoarseVertex coarseVertex : CoarseVertex;
    nointerpolation uint2 info : AAAA;
};

[shader("vertex")]
VertexStageOutput vertexMain(
    AssembledVertex assembledVertex)
{
    VertexStageOutput output;

    float3 position = assembledVertex.position;
    float2 uv    = assembledVertex.uv;

    output.coarseVertex.uv = uv;
    output.info = uint2(1,2);
    output.sv_position = mul(modelViewProjection, float4(position, 1.0));

    return output;
}

// Fragment Shader

[shader("fragment")]
Fragment fragmentMain(
    FragmentStageInput input) : SV_Target
{
    float2 uv = input.coarseVertex.uv;

    Fragment output;
    output.color = float4(uv, 0.0, 1.0);
    return output;
}