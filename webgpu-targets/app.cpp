//
// Created by Ingun Jon on 12/20/25.
//

#include <nvrhi/nvrhi.h>
#include <nvrhi/webgpu.h>
#include <webgpu/webgpu_cpp.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include "webgpu-util.h"
#include "webgpu-glfw-util.h"
#include <fstream>

namespace math {
using float3 = std::array<float, 3>;
using float2 = std::array<float, 2>;
using float4x4 = std::array<float, 16>;
}

namespace dm = math;

struct Vertex {
    math::float3 position;
    math::float2 uv;
};

static const Vertex g_Vertices[] = {
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}}, // front face
    {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},

    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}}, // right side face
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f}},

    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // left side face
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f}},

    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}}, // back face
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},

    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f}}, // top face
    {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f}},

    {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f}}, // bottom face
    {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f}},
};

namespace nvrhi {
namespace utils {
nvrhi::BufferDesc CreateStaticConstantBufferDesc(
    uint32_t byteSize,
    const char* debugName) {
    nvrhi::BufferDesc constantBufferDesc;
    constantBufferDesc.byteSize = byteSize;
    constantBufferDesc.debugName = debugName;
    constantBufferDesc.isConstantBuffer = true;
    constantBufferDesc.isVolatile = false;
    return constantBufferDesc;
}
}
}

struct ConstantBufferEntry {
    dm::float4x4 viewProjMatrix;
    float padding[16 * 3];
};

nvrhi::ShaderHandle CreateShader(const char* fileName,
                                 const char* entryPoint,
                                 const nvrhi::ShaderType type,
                                 const nvrhi::DeviceHandle& m_Device
    ) {
    nvrhi::ShaderDesc desc;
    if (desc.debugName.empty())
        desc.debugName = fileName;
    desc.entryName = entryPoint;
    desc.shaderType = type;

    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        // Handle error: file not found
        return nullptr;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> byteCode(size);
    if (!file.read(reinterpret_cast<char*>(byteCode.data()), size)) {
        // Handle error: read failed
        return nullptr;
    }
    return m_Device->createShader(desc, byteCode.data(), byteCode.size());
}

int main() {
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize GLFW");
    int width = 1024, height = 1024;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to open GLFW window");
    }
    wgpu::InstanceDescriptor instanceDescriptor = nvrhi::webgpu::utils::create_instance_descriptor();
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDescriptor);
    wgpu::Adapter adapter = nvrhi::webgpu::utils::create_adapter(
        instance,
        nvrhi::webgpu::utils::create_adapter_option(wgpu::BackendType::Metal, wgpu::AdapterType::IntegratedGPU));
    wgpu::Device device = create_device(instance, adapter);
    wgpu::Queue queue = device.GetQueue();
    nvrhi::DeviceHandle nvrhiDevice = nvrhi::webgpu::createDevice({device, queue});
    wgpu::Surface surface = create_webgpu_surface(window, instance, adapter, device, width, height);

    auto GetDevice = [&]() { return nvrhiDevice; };

    auto m_ConstantBuffer = GetDevice()->createBuffer(
        nvrhi::utils::CreateStaticConstantBufferDesc(sizeof(ConstantBufferEntry), "ConstantBuffer")
        .setInitialState(nvrhi::ResourceStates::ConstantBuffer).setKeepInitialState(true));
    nvrhi::VertexAttributeDesc attributes[] = {
        nvrhi::VertexAttributeDesc()
        .setName("POSITION")
        .setFormat(nvrhi::Format::RGB32_FLOAT)
        .setOffset(0)
        .setBufferIndex(0)
        .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
        .setName("UV")
        .setFormat(nvrhi::Format::RG32_FLOAT)
        .setOffset(0)
        .setBufferIndex(1)
        .setElementStride(sizeof(Vertex)),
    };
    std::string shaderFilePath =
        "/Users/ingun/CLionProjects/nvrhi-unit-test/cmake-build-vulkan/shaders/generated_wgsl/shader.wgsl";
    auto m_VertexShader = CreateShader(shaderFilePath.c_str(),
                                       "vertexMain",
                                       nvrhi::ShaderType::Vertex,
                                       GetDevice());

    auto m_FragShader = CreateShader(shaderFilePath.c_str(),
                                     "fragmentMain",
                                     nvrhi::ShaderType::Pixel,
                                     GetDevice());
    auto m_InputLayout = GetDevice()->createInputLayout(attributes, uint32_t(std::size(attributes)), m_VertexShader);

    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */

        // glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}