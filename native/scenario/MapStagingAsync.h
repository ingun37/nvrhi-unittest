//
// Created by Ingun Jon on 1/14/26.
//

#ifndef NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#define NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H
#include "include/scenario/Step.h"


struct MapStagingAsync : public Step {
    MapStagingAsync() = delete;

    explicit MapStagingAsync(const Context& ctx)
        : Step(ctx, "Map Staging Async", "", "") {
    }

    StepFuture run(std::string) override;
};


#endif //NVRHI_UNIT_TEST_MAPSTAGINGASYNC_H