//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_SHADER_H
#define NVRHI_UNIT_TEST_SHADER_H
#include "App.h"


struct Shader : public App {
    Shader() = delete;

    Shader(const Context& webGpu)
        : App(webGpu, "Shader") {
    }

    AppPtr run() override;
};


#endif //NVRHI_UNIT_TEST_SHADER_H