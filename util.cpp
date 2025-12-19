//
// Created by Ingun Jon on 12/19/25.
//
#include <iostream>
#include "util.h"

wgpu::Device create_device(const wgpu::Instance &instance, const wgpu::Adapter &adapter) {
    wgpu::DeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.SetDeviceLostCallback(
        wgpu::CallbackMode::AllowSpontaneous,
        [](const wgpu::Device &, wgpu::DeviceLostReason reason, wgpu::StringView message) {
            const char *reasonName = "";
            switch (reason) {
                case wgpu::DeviceLostReason::Unknown:
                    reasonName = "Unknown";
                    break;
                case wgpu::DeviceLostReason::Destroyed:
                    reasonName = "Destroyed";
                    break;
                case wgpu::DeviceLostReason::CallbackCancelled:
                    reasonName = "CallbackCancelled";
                    break;
                case wgpu::DeviceLostReason::FailedCreation:
                    reasonName = "FailedCreation";
                    break;
                default:
                    throw std::runtime_error("Unexpected device lost reason");
            }
            std::cerr << "Device lost because of " << reasonName << ": " << message.data;
        });
    deviceDesc.SetUncapturedErrorCallback(
        [](const wgpu::Device &, wgpu::ErrorType type, wgpu::StringView message) {
            const char *errorTypeName = "";
            switch (type) {
                case wgpu::ErrorType::Validation:
                    errorTypeName = "Validation";
                    break;
                case wgpu::ErrorType::OutOfMemory:
                    errorTypeName = "Out of memory";
                    break;
                case wgpu::ErrorType::Internal:
                    errorTypeName = "Internal";
                    break;
                case wgpu::ErrorType::Unknown:
                    errorTypeName = "Unknown";
                    break;
                default:
                    throw std::runtime_error("Unexpected error type");
            }
            std::cerr << errorTypeName << " error: " << message.data;
        });

    wgpu::Device device = nullptr;

    // Synchronously create the device
    instance.WaitAny(
        adapter.RequestDevice(
            &deviceDesc, wgpu::CallbackMode::WaitAnyOnly,
            [&device](wgpu::RequestDeviceStatus status, wgpu::Device dv, wgpu::StringView message) {
                if (status != wgpu::RequestDeviceStatus::Success) {
                    std::cerr << "Failed to get an device: " << message.data;
                    throw std::runtime_error("Failed to get an device");
                }
                device = std::move(dv);
            }),
        UINT64_MAX);

    return device;
}
