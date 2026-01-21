//
// Created by Ingun Jon on 1/21/26.
//

#ifndef NVRHI_UNIT_TEST_RENDERPASS_H
#define NVRHI_UNIT_TEST_RENDERPASS_H

#include "include/scenario/App.h"

struct RenderPass : public App {
    RenderPass() = delete;

    explicit RenderPass(const Context& ctx)
        : App(ctx, "RenderPass", "", "") {
    }

    AppPtr run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_RENDERPASS_H