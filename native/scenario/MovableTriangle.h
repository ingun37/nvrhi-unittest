//
// Created by Ingun Jon on 2/26/26.
//

#ifndef NVRHI_UNIT_TEST_MOVABLETRIANGLE_H
#define NVRHI_UNIT_TEST_MOVABLETRIANGLE_H
#include "include/scenario/Step.h"


struct MovableTriangle : Step {
    MovableTriangle() = delete;

    explicit MovableTriangle(const Context& ctx)
        : Step(ctx, "Movable Triangle", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_MOVABLETRIANGLE_H