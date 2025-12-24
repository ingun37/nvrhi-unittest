//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_SCENARIO_H
#define NVRHI_UNIT_TEST_SCENARIO_H

#include "Context.h"

enum struct Scenario {
    COPY_2D_STAGING_TO_TEXTURE,
    COPY_3D,
    TRIANGLE
};

void run_app(Context&& webGpu, Scenario scenario);

#endif //NVRHI_UNIT_TEST_SCENARIO_H