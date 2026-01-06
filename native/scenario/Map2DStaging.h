//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_MAP2DSTAGING_H
#define NVRHI_UNIT_TEST_MAP2DSTAGING_H
#include "App.h"


struct Map2DStaging : public App {
    Map2DStaging() = delete;

    Map2DStaging(const Context& ctx)
        : App(ctx, "Map2DStaging") {
    }

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_MAP2DSTAGING_H