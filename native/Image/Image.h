//
// Created by Ingun Jon on 12/21/25.
//

#ifndef NVRHI_UNIT_TEST_IMAGE_H
#define NVRHI_UNIT_TEST_IMAGE_H
#include <vector>

enum struct ComponentType {
    Float,
    UChar
};

inline size_t componentTypeSize(ComponentType type) {
    switch (type) {
        case ComponentType::Float:
            return sizeof(float);
        case ComponentType::UChar:
            return sizeof(uint8_t);
    }
    throw std::runtime_error("Invalid component type");
}

struct Image {
    Image() = delete;

    const int width;
    const int height;
    const int channel;
    const int componentSize;
    const std::vector<uint8_t> data;

    Image(int width, int height, int channel, std::vector<uint8_t> data)
        : width(width),
          height(height),
          channel(channel),
          data(std::move(data)),
          componentSize(data.size() / (width * height * channel)) {
    }

    static Image load(const std::string& path);

    static Image create(int width, int height, int channel, ComponentType componentType);
};


#endif //NVRHI_UNIT_TEST_IMAGE_H