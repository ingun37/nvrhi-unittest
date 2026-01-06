//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_CONTEXT_H
#define NVRHI_UNIT_TEST_CONTEXT_H
#include <nvrhi/nvrhi.h>

struct Context {
    nvrhi::DeviceHandle nvrhiDevice;

    Context() = delete;

    Context(
        nvrhi::DeviceHandle nvrhiDevice)
        : nvrhiDevice(std::move(nvrhiDevice)) {
    }
};

#endif //NVRHI_UNIT_TEST_CONTEXT_H