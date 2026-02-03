//
// Created by Ingun Jon on 2/3/26.
//

#ifndef NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#define NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#include "include/scenario/App.h"


struct ConstantBufferDraw : public App {
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
        : App(ctx, "RunDrawCommand", "", ""),
          colorTexture(colorTexture),
          vertex(vertex),
          pixel(pixel),
          framebuffer(framebuffer),
          constantBuffer(constantBuffer),
          commandList(commandList) {
    }

    AppPtr run(std::string) override;
};

struct ConstantBuffer : public App {
    ConstantBuffer() = delete;

    explicit ConstantBuffer(const Context& ctx)
        : App(ctx, "ConstantBuffer", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_CONSTANTBUFFER_H