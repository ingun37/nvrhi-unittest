//
// Created by Ingun Jon on 12/21/25.
//
#include <iostream>
// #define VOLK_IMPLEMENTATION
// #include "volk.h"
#include <nvrhi/nvrhi.h>
#include <nvrhi/vulkan.h>
#include "../cli_app.h"

struct QueueFamilyIndices {
    uint32_t graphicsFamily;
    uint32_t transferFamily;
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
            indices.transferFamily = i;
        }

        i++;
    }
    return indices;
}

VkInstance create_instance() {
    // if (volkInitialize() != VK_SUCCESS) throw std::runtime_error("failed to initialize volk!");
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledLayerCount = 0;

    std::vector<const char*> requiredExtensions;

    requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VkInstance instance;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
    // volkLoadInstance(instance);
    return instance;
}

class MessageCallback : public nvrhi::IMessageCallback {
    void message(nvrhi::MessageSeverity severity, const char* messageText) override {
        std::cout << "message callback: " << messageText << std::endl;
    }
};

int main() {
    VkInstance instance = create_instance();

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    physicalDevice = devices[0];
    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    VkDevice device;
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    createInfo.enabledLayerCount = 0;
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    VkQueue graphicsQueue, transferQueue;
    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.transferFamily, 0, &transferQueue);

    MessageCallback messageCallback;
    nvrhi::vulkan::DeviceDesc deviceDesc;
    deviceDesc.errorCB = &messageCallback;
    deviceDesc.instance = instance;
    deviceDesc.physicalDevice = physicalDevice;
    deviceDesc.device = device;
    deviceDesc.graphicsQueue = graphicsQueue;
    deviceDesc.graphicsQueueIndex = indices.graphicsFamily;
    // if (m_DeviceParams.enableComputeQueue) {
    //     deviceDesc.computeQueue = m_ComputeQueue;
    //     deviceDesc.computeQueueIndex = m_ComputeQueueFamily;
    // }
    // if (m_DeviceParams.enableCopyQueue) {
    deviceDesc.transferQueue = transferQueue;
    deviceDesc.transferQueueIndex = indices.transferFamily;
    // }
    // deviceDesc.instanceExtensions = vecInstanceExt.data();
    // deviceDesc.numInstanceExtensions = vecInstanceExt.size();
    // deviceDesc.deviceExtensions = vecDeviceExt.data();
    // deviceDesc.numDeviceExtensions = vecDeviceExt.size();
    // deviceDesc.bufferDeviceAddressSupported = m_BufferDeviceAddressSupported;
#if DONUT_WITH_AFTERMATH
    deviceDesc.aftermathEnabled = m_DeviceParams.enableAftermath;
#endif
    // deviceDesc.vulkanLibraryName = m_DeviceParams.vulkanLibraryName;
    // deviceDesc.logBufferLifetime = m_DeviceParams.logBufferLifetime;
    auto nvrhiDevice = nvrhi::vulkan::createDevice(deviceDesc);

    run_app(Context(nvrhiDevice));

    vkDestroyDevice(device, nullptr);

    vkDestroyInstance(instance, nullptr);
}