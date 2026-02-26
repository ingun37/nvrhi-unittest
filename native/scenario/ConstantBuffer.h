//
// Created by Ingun Jon on 2/3/26.
//

#ifndef NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#define NVRHI_UNIT_TEST_CONSTANTBUFFER_H
#include "include/scenario/Step.h"


struct ConstantBuffer : public Step {
    ConstantBuffer() = delete;

    explicit ConstantBuffer(const Context& ctx)
        : Step(ctx, "ConstantBuffer", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_CONSTANTBUFFER_H