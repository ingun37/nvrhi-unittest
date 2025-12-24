//
// Created by Ingun Jon on 12/24/25.
//

#ifndef NVRHI_UNIT_TEST_NVRHI_UTIL_H
#define NVRHI_UNIT_TEST_NVRHI_UTIL_H
#include <nvrhi/nvrhi.h>

namespace nvrhi_util {
nvrhi::StagingTextureHandle create3DStaging(uint32_t width,
                                            uint32_t height,
                                            uint32_t depth,
                                            const nvrhi::Format format,
                                            const nvrhi::DeviceHandle& device);
}


#endif //NVRHI_UNIT_TEST_NVRHI_UTIL_H