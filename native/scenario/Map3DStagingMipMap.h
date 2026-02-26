//
// Created by Ingun Jon on 1/12/26.
//

#ifndef NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H
#define NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H


#include "include/scenario/Step.h"


struct Map3DStagingMipMap : public Step {
    Map3DStagingMipMap() = delete;

    Map3DStagingMipMap(const Context& ctx)
        : Step(ctx, "Map 3D Staging Texture MipMap", "", "") {
    }

    StepFuture run(std::string) override;
};

#endif //NVRHI_UNIT_TEST_MAP3DSTAGINGMIPMAP_H