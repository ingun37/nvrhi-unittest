//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_MAP3DSTAGING_H
#define NVRHI_UNIT_TEST_MAP3DSTAGING_H
#include "App.h"


struct Map3DStaging : public App {
    Map3DStaging() = delete;

    Map3DStaging(const Context& ctx)
        : App(ctx, "Map 3D Staging Texture") {
    }

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_MAP3DSTAGING_H