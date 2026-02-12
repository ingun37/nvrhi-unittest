//
// Created by Ingun Jon on 2/3/26.
//

#ifndef NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#define NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#include "include/scenario/Step.h"


struct ConstantBufferDraw : public Step {
    nvrhi::TextureHandle colorTexture;
    nvrhi::ShaderHandle vertex;
    nvrhi::ShaderHandle pixel;
    nvrhi::FramebufferHandle framebuffer;
    nvrhi::BufferHandle constantBuffer;
    nvrhi::CommandListHandle commandList;

    ConstantBufferDraw() = delete;

    explicit ConstantBufferDraw(const Context& ctx,
                                nvrhi::TextureHandle&& colorTexture,
                                nvrhi::ShaderHandle&& vertex,
                                nvrhi::ShaderHandle&& pixel,
                                nvrhi::FramebufferHandle&& framebuffer,
                                nvrhi::BufferHandle&& constantBuffer,
                                nvrhi::CommandListHandle&& commandList
        )
        : Step(ctx, "RunDrawCommand", "", ""),
          colorTexture(colorTexture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          constantBuffer(constantBuffer),
          commandList(commandList) {
    }

    StepFuture run(std::string) override;
};

struct ConstantBuffer : public Step {
    ConstantBuffer() = delete;

    explicit ConstantBuffer(const Context& ctx)
        : Step(ctx, "ConstantBuffer", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_CONSTANTBUFFER_H