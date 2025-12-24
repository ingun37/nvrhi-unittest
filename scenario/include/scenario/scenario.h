//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_SCENARIO_H
#define NVRHI_UNIT_TEST_SCENARIO_H

#include "Context.h"

enum struct Scenario {
    COPY_2D_STAGING_TO_TEXTURE,
    COPY_3D,
    TRIANGLE,
    MAP_2D_STAGING,
    MAP_3D_STAGING
};

void run_app(Context&& webGpu);

#endif //NVRHI_UNIT_TEST_SCENARIO_H