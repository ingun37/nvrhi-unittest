RWStructuredBuffer<float4> result;

[shader("compute")]
[numthreads(1,2,64)]
void cs(uint3 threadId : SV_DispatchThreadID)
{
    uint xi = threadId.y;
    uint yi = threadId.z;
    uint vi = xi+2*yi;

    float x = float(xi) - 0.5;
    float y = float(yi)/64.0 - 0.5;
    float z = float(vi)/128.0;
    result[vi] = float4(x,
                        y,
                        z,
                        1.0
                    );
}