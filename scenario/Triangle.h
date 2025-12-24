//
// Created by Ingun Jon on 12/23/25.
//

#ifndef NVRHI_UNIT_TEST_TRIANGLE_H
#define NVRHI_UNIT_TEST_TRIANGLE_H
#include "App.h"
#include <nvrhi/nvrhi.h>
#include <ios>
#include "my_math.h"
namespace math = donut::math;

struct ConstantBufferEntry {
    dm::float4x4 viewProjMatrix;
    float padding[16 * 3];
};

struct Vertex {
    math::float3 position;
    math::float2 uv;
};


struct Triangle : public App {
    Triangle() = delete;

    Triangle(Context&& webGpu);

    nvrhi::BufferDesc CreateStaticConstantBufferDesc(
        uint32_t byteSize,
        const char* debugName);

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_TRIANGLE_H