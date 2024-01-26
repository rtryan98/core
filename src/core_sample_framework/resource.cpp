#include "resource.hpp"
#include "d3d12/d3d12_device.hpp"

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

Graphics_Pipeline* Resource_Manager::create_graphics_pipeline(
    const Graphics_Pipeline_Create_Info& create_info) noexcept
{
    auto* pso = &m_graphics_pipelines.emplace_back();

    return pso;
}

void Resource_Manager::destroy_gpu_resource(ID3D12Resource* resource, D3D12MA::Allocation* allocation)
{
    allocation->Release();
    resource->Release();
}

}
