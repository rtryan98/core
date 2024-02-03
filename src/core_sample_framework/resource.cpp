#include "resource.hpp"
#include "d3d12/d3d12_device.hpp"
#include "d3d12/d3d12_pso.hpp"

#include <D3D12MemAlloc.h>

namespace core::sf
{
Resource_Manager::Resource_Manager(const Resource_Manager_Create_Info& create_info, d3d12::D3D12_Context* d3d12_context) noexcept
    : m_d3d12_context(d3d12_context)
    , m_resource_descriptor_heap(nullptr)
    , m_sampler_descriptor_heap(nullptr)
    , m_buffers()
    , m_textures()
    , m_shaders()
    , m_graphics_pipelines()
    , m_compute_pipelines()
    , m_allocator(nullptr)
{
    m_buffers.reserve(create_info.max_buffer_count);
    m_textures.reserve(create_info.max_texture_count);
    m_shaders.reserve(create_info.max_shader_count);
    m_graphics_pipelines.reserve(create_info.max_graphics_pipeline_count);
    m_compute_pipelines.reserve(create_info.max_compute_pipeline_count);

    D3D12MA::ALLOCATOR_DESC allocator_desc = {
        .Flags = D3D12MA::ALLOCATOR_FLAG_SINGLETHREADED,
        .pDevice = d3d12_context->device,
        .PreferredBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pAdapter = d3d12_context->adapter
    };
    D3D12MA::CreateAllocator(&allocator_desc, &m_allocator);
}

Resource_Manager::~Resource_Manager()
{
    for (auto& buffer : m_buffers)
    {
        if (!buffer.alive)
            continue;
        destroy_gpu_resource(buffer.resource, buffer.allocation);
    }
    for (auto& texture : m_textures)
    {
        if (!texture.alive)
            continue;
        destroy_gpu_resource(texture.resource, texture.allocation);
    }
    m_allocator->Release();
}

Buffer* Resource_Manager::create_buffer(const Buffer_Create_Info & create_info) noexcept
{
    auto* buffer =  &m_buffers.emplace_back();
    buffer->alive = true;
    buffer->size = create_info.size;
    buffer->heap_type = create_info.heap_type;

    D3D12MA::ALLOCATION_DESC alloc_desc = {
        .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
        .HeapType = create_info.heap_type,
        .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
        .CustomPool = nullptr,
        .pPrivateData = nullptr
    };
    D3D12_RESOURCE_DESC1 resource_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0ull,
        .Width = create_info.size,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .Format = DXGI_FORMAT_UNKNOWN,
        .SampleDesc = { .Count = 1, .Quality = 0 },
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Flags = D3D12_RESOURCE_FLAG_NONE,
        .SamplerFeedbackMipRegion = {}
    };
    m_allocator->CreateResource3(&alloc_desc, &resource_desc,
        D3D12_BARRIER_LAYOUT_UNDEFINED, nullptr, 0, nullptr,
        &buffer->allocation, IID_PPV_ARGS(&buffer->resource));

    return buffer;
}

D3D12_SHADER_BYTECODE create_bytecode_ref_for_shader(Shader* shader) noexcept
{
    if (!shader) {
        return D3D12_SHADER_BYTECODE {
            .pShaderBytecode = nullptr,
            .BytecodeLength = 0
        };
    }
    return D3D12_SHADER_BYTECODE{
        .pShaderBytecode = shader->bytecode.data(),
        .BytecodeLength = uint32_t(shader->bytecode.size())
    };
}

Graphics_Pipeline* Resource_Manager::create_graphics_pipeline(
    const Graphics_Pipeline_Create_Info& create_info) noexcept
{
    auto* pso = &m_graphics_pipelines.emplace_back();
    d3d12::Graphics_Pipeline_Desc pso_desc = {
        .root_signature = {.data = m_d3d12_context->bindless_root_signature },
        .vs = {.data = create_bytecode_ref_for_shader(create_info.vs) },
        .hs = {.data = create_bytecode_ref_for_shader(create_info.hs) },
        .ds = {.data = create_bytecode_ref_for_shader(create_info.ds) },
        .gs = {.data = create_bytecode_ref_for_shader(create_info.gs) },
        .ps = {.data = create_bytecode_ref_for_shader(create_info.ps) },
        .stream_output = {.data = {} }, // Leave empty
        .blend_state = {.data = create_info.blend_desc },
        .sample_mask = {.data = create_info.sample_mask },
        .rasterizer_state = {.data = create_info.rasterizer_desc },
        .depth_stencil_state = {.data = create_info.depth_stencil_desc },
        .input_layout_desc = {.data = {} }, // Leave empty
        .primitive_topology_type = {.data = create_info.primitive_topology_type },
        .render_target_formats = {.data = create_info.render_target_formats },
        .depth_stencil_format = {.data = create_info.depth_stencil_format },
        .sample_desc = {.data = { .Count = 1, .Quality = 0 } },
        .node_mask = {.data = 0 },
        .cached_pso = {.data = {} },
        .flags = {.data = D3D12_PIPELINE_STATE_FLAG_NONE }
    };
    D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {
        .SizeInBytes = sizeof(pso_desc),
        .pPipelineStateSubobjectStream = &pso_desc
    };
    m_d3d12_context->device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso->pso));
    return pso;
}

Graphics_Pipeline* Resource_Manager::create_mesh_shader_pipeline(const Mesh_Shader_Pipeline_Create_Info& create_info) noexcept
{
    auto* pso = &m_graphics_pipelines.emplace_back();
    d3d12::Mesh_Shader_Pipeline_Desc pso_desc = {
        .root_signature = {.data = m_d3d12_context->bindless_root_signature },
        .as = {.data = create_bytecode_ref_for_shader(create_info.as) },
        .ms = {.data = create_bytecode_ref_for_shader(create_info.ms) },
        .ps = {.data = create_bytecode_ref_for_shader(create_info.ps) },
        .blend_state = {.data = create_info.blend_desc },
        .sample_mask = {.data = create_info.sample_mask },
        .rasterizer_state = {.data = create_info.rasterizer_desc },
        .depth_stencil_state = {.data = create_info.depth_stencil_desc },
        .input_layout_desc = {.data = {} }, // Leave empty
        .primitive_topology_type = {.data = create_info.primitive_topology_type },
        .render_target_formats = {.data = create_info.render_target_formats },
        .depth_stencil_format = {.data = create_info.depth_stencil_format },
        .sample_desc = {.data = {.Count = 1, .Quality = 0 } },
        .node_mask = {.data = 0 },
        .cached_pso = {.data = {} },
        .flags = {.data = D3D12_PIPELINE_STATE_FLAG_NONE }
    };
    D3D12_PIPELINE_STATE_STREAM_DESC stream_desc = {
        .SizeInBytes = sizeof(pso_desc),
        .pPipelineStateSubobjectStream = &pso_desc
    };
    m_d3d12_context->device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso->pso));
    return pso;
}

Compute_Pipeline* Resource_Manager::create_compute_pipeline(Shader* shader) noexcept
{
    auto* pso = &m_compute_pipelines.emplace_back();
    D3D12_COMPUTE_PIPELINE_STATE_DESC pso_desc = {
        .pRootSignature = m_d3d12_context->bindless_root_signature,
        .CS = create_bytecode_ref_for_shader(shader),
        .NodeMask = 0,
        .CachedPSO = {},
        .Flags = D3D12_PIPELINE_STATE_FLAG_NONE
    };
    m_d3d12_context->device->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&pso->pso));
    return pso;
}

void Resource_Manager::destroy_gpu_resource(ID3D12Resource* resource, D3D12MA::Allocation* allocation)
{
    allocation->Release();
    resource->Release();
}

}
