RWStructuredBuffer<float4> result;

[shader("compute")]
[numthreads(1,2,64)]
void cs(uint3 threadId : SV_DispatchThreadID)
{
    uint xi = threadId.y;
    uint yi = threadId.z;
    uint vi = xi+2*yi;

    float x = float(xi) - 0.5;
    float y = float(yi)/63.0 - 0.5;

    float ln127 = 4.84418708646;

    float z = log(float(vi)) / ln127;

    result[vi] = float4(x,
                        y,
                        z,
                        1.0
                    );
}