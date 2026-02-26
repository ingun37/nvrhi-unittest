//
// Created by Ingun Jon on 2/9/26.
//

#ifndef NVRHI_UNIT_TEST_TEXTUREOVER16_H
#define NVRHI_UNIT_TEST_TEXTUREOVER16_H
#include <utility>

#include "include/scenario/Step.h"


struct TextureOver16 : public Step {
    TextureOver16() = delete;

    explicit TextureOver16(const Context& ctx)
        : Step(ctx, "TextureOver16", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_TEXTUREOVER16_H