//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_IMAGE_H
#define NVRHI_UNIT_TEST_IMAGE_H
#include <vector>

struct Image {
    Image() = delete;

    int width;
    int height;
    int channel;
    std::vector<uint8_t> data;

    Image(int width, int height, int channel, std::vector<uint8_t> data)
        : width(width),
          height(height),
          channel(channel),
          data(std::move(data)) {
    }

    static Image load(const std::string& path);
};


#endif //NVRHI_UNIT_TEST_IMAGE_H