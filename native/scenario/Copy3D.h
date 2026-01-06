//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_COPY3D_H
#define NVRHI_UNIT_TEST_COPY3D_H
#include "App.h"


struct Copy3D : public App {
    Copy3D() = delete;

    Copy3D(const Context& webGpu)
        : App(webGpu, "Copy3D") {
    }

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_COPY3D_H