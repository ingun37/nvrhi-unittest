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
#include "webgpu-nvrhi-util.h"
#include <fstream>
#include <ranges>

#include "my_math.h"

namespace dm = donut::math;
namespace math = donut::math;

struct Vertex {
    math::float3 position;
    math::float2 uv;
};

static const uint32_t g_Indices[] = {
    0, 1, 2, 0, 3, 1,       // front face
    4, 5, 6, 4, 7, 5,       // left face
    8, 9, 10, 8, 11, 9,     // right face
    12, 13, 14, 12, 15, 13, // back face
    16, 17, 18, 16, 19, 17, // top face
    20, 21, 22, 20, 23, 21, // bottom face
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
constexpr uint32_t c_NumViews = 4;

static const math::float3 g_RotationAxes[c_NumViews] = {
    math::float3(1.f, 0.f, 0.f),
    math::float3(0.f, 1.f, 0.f),
    math::float3(0.f, 0.f, 1.f),
    math::float3(1.f, 1.f, 1.f),
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
        .setName("position")
        .setFormat(nvrhi::Format::RGB32_FLOAT)
        .setOffset(0)
        .setBufferIndex(0)
        .setElementStride(sizeof(Vertex)),
        nvrhi::VertexAttributeDesc()
        .setName("uv")
        .setFormat(nvrhi::Format::RG32_FLOAT)
        .setOffset(0)
        .setBufferIndex(1)
        .setElementStride(sizeof(Vertex)),
    };
    std::string shaderFilePath =
        "/Users/ingun/CLionProjects/nvrhi-unit-test/cmake-build-webgpu/shaders/generated_wgsl/shader.wgsl";
    auto m_VertexShader = CreateShader(shaderFilePath.c_str(),
                                       "vertexMain",
                                       nvrhi::ShaderType::Vertex,
                                       GetDevice());

    auto m_PixelShader = CreateShader(shaderFilePath.c_str(),
                                      "fragmentMain",
                                      nvrhi::ShaderType::Pixel,
                                      GetDevice());
    auto m_InputLayout = GetDevice()->createInputLayout(attributes, uint32_t(std::size(attributes)), m_VertexShader);
    auto m_CommandList = GetDevice()->createCommandList();
    nvrhi::BufferDesc vertexBufferDesc;
    vertexBufferDesc.byteSize = sizeof(g_Vertices);
    vertexBufferDesc.isVertexBuffer = true;
    vertexBufferDesc.debugName = "VertexBuffer";
    vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
    auto m_VertexBuffer = GetDevice()->createBuffer(vertexBufferDesc);
    m_CommandList->open();

    m_CommandList->writeBuffer(m_VertexBuffer, g_Vertices, sizeof(g_Vertices));

    nvrhi::BufferDesc indexBufferDesc;
    indexBufferDesc.byteSize = sizeof(g_Indices);
    indexBufferDesc.isIndexBuffer = true;
    indexBufferDesc.debugName = "IndexBuffer";
    indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
    auto m_IndexBuffer = GetDevice()->createBuffer(indexBufferDesc);

    m_CommandList->writeBuffer(m_IndexBuffer, g_Indices, sizeof(g_Indices));

    m_CommandList->close();
    GetDevice()->executeCommandList(m_CommandList);

    glfwMakeContextCurrent(window);

    auto backBuffer = CreateBackBuffer(nvrhiDevice, surface);
    auto framebuffer = GetDevice()->createFramebuffer(
        nvrhi::FramebufferDesc().addColorAttachment(backBuffer));
    auto samplerDesc = nvrhi::SamplerDesc()
        .setAllFilters(false)
        .setAllAddressModes(nvrhi::SamplerAddressMode::Clamp);
    samplerDesc.setAllAddressModes(nvrhi::SamplerAddressMode::Wrap);
    auto m_LinearWrapSampler = nvrhiDevice->createSampler(samplerDesc);

    nvrhi::BindingSetDesc bindingSetDesc;
    bindingSetDesc.bindings = {
        // Note: using viewIndex to construct a buffer range.
        nvrhi::BindingSetItem::ConstantBuffer(0,
                                              m_ConstantBuffer,
                                              nvrhi::BufferRange(0,
                                                                 sizeof(ConstantBufferEntry))),
        // Texutre and sampler are the same for all model views.
        // nvrhi::BindingSetItem::Sampler(0, m_LinearWrapSampler)
    };

    auto transformBSI2BLI = [](const nvrhi::BindingSetItem& item) {
        nvrhi::BindingLayoutItem layoutItem{};
        layoutItem.slot = item.slot;
        layoutItem.type = item.type;
        layoutItem.size = 1;
        if (item.type == nvrhi::ResourceType::PushConstants)
            layoutItem.size = uint32_t(item.range.byteSize);
        return layoutItem;
    };

    auto bindingLayoutItems = bindingSetDesc.bindings | std::views::transform(transformBSI2BLI);
    nvrhi::BindingLayoutDesc bindingLayoutDesc;
    bindingLayoutDesc.visibility = nvrhi::ShaderType::All;
    bindingLayoutDesc.registerSpace = 0;
    bindingLayoutDesc.registerSpaceIsDescriptorSet = false;
    bindingLayoutDesc.bindings = bindingLayoutItems | std::ranges::to<std::vector>();
    auto m_BindingLayout = nvrhiDevice->createBindingLayout(bindingLayoutDesc);

    auto m_BindingSets = nvrhiDevice->createBindingSet(bindingSetDesc, m_BindingLayout);

    nvrhi::GraphicsPipelineDesc psoDesc;
    psoDesc.VS = m_VertexShader;
    psoDesc.PS = m_PixelShader;
    psoDesc.inputLayout = m_InputLayout;
    psoDesc.bindingLayouts = {m_BindingLayout};
    psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
    psoDesc.renderState.depthStencilState.depthTestEnable = false;
    psoDesc.renderState.rasterState.cullMode = nvrhi::RasterCullMode::None;

    auto m_Pipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
    float m_Rotation = 0.f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */

        m_CommandList->open();

        // Fill out the constant buffer slices for multiple views of the model.
        ConstantBufferEntry modelConstants[1];
        const int viewIndex = 0;

        math::affine3 viewMatrix = math::rotation(normalize(g_RotationAxes[viewIndex]), m_Rotation)
                                   * math::yawPitchRoll(0.f, math::radians(-30.f), 0.f)
                                   * math::translation(math::float3(0, 0, 2));
        math::float4x4 projMatrix = math::perspProjD3DStyle(math::radians(60.f),
                                                            float(width) / float(height),
                                                            0.1f,
                                                            10.f);
        math::float4x4 viewProjMatrix = math::affineToHomogeneous(viewMatrix) * projMatrix;
        modelConstants[viewIndex].viewProjMatrix = viewProjMatrix;
        // Upload all constant buffer slices at once.
        m_CommandList->writeBuffer(m_ConstantBuffer, modelConstants, sizeof(modelConstants));

        nvrhi::GraphicsState state;
        // Pick the right binding set for this view.
        state.bindings = {m_BindingSets};
        state.indexBuffer = {m_IndexBuffer, nvrhi::Format::R32_UINT, 0};
        // Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
        state.vertexBuffers = {
            {m_VertexBuffer, 1, offsetof(Vertex, uv)},
            {m_VertexBuffer, 0, offsetof(Vertex, position)}
        };
        state.pipeline = m_Pipeline;
        state.framebuffer = framebuffer;

        // Construct the viewport so that all viewports form a grid.

        const nvrhi::Viewport viewport = nvrhi::Viewport(0, width, 0, height, 0.f, 1.f);
        state.viewport.addViewportAndScissorRect(viewport);

        // Update the pipeline, bindings, and other state.
        m_CommandList->setGraphicsState(state);

        // Draw the model.
        nvrhi::DrawArguments args;
        args.vertexCount = std::size(g_Indices);
        m_CommandList->drawIndexed(args);

        m_CommandList->close();
        GetDevice()->executeCommandList(m_CommandList);
        // glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
}