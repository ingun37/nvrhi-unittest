//
// Created by Ingun Jon on 12/20/25.
//

#include <webgpu/webgpu_cpp.h>
#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include "util.h"
#include "cli_app.h"


int main() {

    wgpu::InstanceDescriptor instanceDescriptor = nvrhi::webgpu::utils::create_instance_descriptor();
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    wgpu::Adapter adapter = nvrhi::webgpu::utils::create_adapter(
        instance,
        nvrhi::webgpu::utils::create_adapter_option(wgpu::BackendType::Metal, wgpu::AdapterType::IntegratedGPU));
    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();
    nvrhi::DeviceHandle nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});;

    run_app(Context(nvrhiDevice));

    return 0;
}