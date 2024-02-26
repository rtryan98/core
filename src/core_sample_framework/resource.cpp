#include "resource.hpp"

#include <core/d3d12/d3d12_device.hpp>
#include <core/d3d12/d3d12_pso.hpp>
#include <core/d3d12/d3d12_descriptor_util.hpp>
#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
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
    , m_descriptor_index_allocator({
        D3D12_MAX_SHADER_VISIBLE_DESCRIPTOR_HEAP_SIZE_TIER_2,
        D3D12_MAX_SHADER_VISIBLE_SAMPLER_HEAP_SIZE,
        1024,
        1024}) // TODO: pass the rtv and dsv descriptor count
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
        .Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS,
        .SamplerFeedbackMipRegion = {}
    };
    m_allocator->CreateResource3(&alloc_desc, &resource_desc,
        D3D12_BARRIER_LAYOUT_UNDEFINED, nullptr, 0, nullptr,
        &buffer->allocation, IID_PPV_ARGS(&buffer->resource));

    auto srv_descriptor_index = m_descriptor_index_allocator.acquire_resource_descriptor_index();
    auto uav_descriptor_index = srv_descriptor_index + 1;
    buffer->bindless_index = srv_descriptor_index;
    auto descriptor_increment = m_d3d12_context->device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto cpu_descriptor_handle = m_d3d12_context->resource_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    cpu_descriptor_handle.ptr += uint64_t(srv_descriptor_index) * descriptor_increment;

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = d3d12::make_raw_buffer_srv(uint32_t(create_info.size));
    m_d3d12_context->device->CreateShaderResourceView(buffer->resource, &srv_desc, cpu_descriptor_handle);

    // TODO: always allow unordered access?
    D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = d3d12::make_raw_buffer_uav(uint32_t(create_info.size));
    cpu_descriptor_handle.ptr += descriptor_increment;
    m_d3d12_context->device->CreateUnorderedAccessView(buffer->resource, nullptr, &uav_desc, cpu_descriptor_handle);

    buffer->alive = true;
    return buffer;
}

Texture* Resource_Manager::create_texture(const Texture_Create_Info& create_info) noexcept
{
    auto* texture = &m_textures.emplace_back();

    D3D12MA::ALLOCATION_DESC alloc_desc = {
        .Flags = D3D12MA::ALLOCATION_FLAG_NONE,
        .HeapType = D3D12_HEAP_TYPE_DEFAULT,
        .ExtraHeapFlags = D3D12_HEAP_FLAG_NONE,
        .CustomPool = nullptr,
        .pPrivateData = nullptr
    };
    uint32_t resource_flags = D3D12_RESOURCE_FLAG_NONE;
    resource_flags |= (create_info.uav_dimension != D3D12_UAV_DIMENSION_UNKNOWN)
        ? D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS
        : D3D12_RESOURCE_FLAG_NONE;
    resource_flags |= (create_info.rtv_dimension != D3D12_RTV_DIMENSION_UNKNOWN)
        ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
        : D3D12_RESOURCE_FLAG_NONE;
    resource_flags |= (create_info.dsv_dimension != D3D12_DSV_DIMENSION_UNKNOWN)
        ? D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        : D3D12_RESOURCE_FLAG_NONE;
    D3D12_RESOURCE_DESC1 resource_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Alignment = 0ull,
        .Width = create_info.width,
        .Height = create_info.height,
        .DepthOrArraySize = create_info.depth_or_array_layers,
        .MipLevels = create_info.mip_levels,
        .Format = create_info.format,
        .SampleDesc = {.Count = 1, .Quality = 0 },
        .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
        .Flags = D3D12_RESOURCE_FLAGS(resource_flags),
        .SamplerFeedbackMipRegion = {}
    };
    bool has_clear_value = create_info.optimized_clear_value.Format != DXGI_FORMAT_UNKNOWN;
    m_allocator->CreateResource3(
        &alloc_desc,
        &resource_desc,
        D3D12_BARRIER_LAYOUT_UNDEFINED,
        has_clear_value ? &create_info.optimized_clear_value : nullptr,
        0,
        nullptr,
        &texture->allocation,
        IID_PPV_ARGS(&texture->resource));

    auto srv_descriptor_index = m_descriptor_index_allocator.acquire_resource_descriptor_index();
    auto uav_descriptor_index = srv_descriptor_index + 1;
    texture->bindless_index = srv_descriptor_index;
    auto srv_uav_descriptor_increment = m_d3d12_context->device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    auto srv_uav_cpu_descriptor_handle = m_d3d12_context->resource_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    srv_uav_cpu_descriptor_handle.ptr += uint64_t(srv_descriptor_index) * srv_uav_descriptor_increment;

    D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = d3d12::make_full_texture_srv(
        create_info.format, create_info.srv_dimension, create_info.depth_or_array_layers);
    m_d3d12_context->device->CreateShaderResourceView(
        texture->resource, &srv_desc, srv_uav_cpu_descriptor_handle);

    if (create_info.uav_dimension != D3D12_UAV_DIMENSION_UNKNOWN)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = d3d12::make_full_texture_uav(
            create_info.format, create_info.uav_dimension, create_info.depth_or_array_layers, 0, 0);
        srv_uav_cpu_descriptor_handle.ptr += srv_uav_descriptor_increment;
        m_d3d12_context->device->CreateUnorderedAccessView(
            texture->resource, nullptr, &uav_desc, srv_uav_cpu_descriptor_handle);
    }

    if (create_info.rtv_dimension != D3D12_RTV_DIMENSION_UNKNOWN)
    {
        auto rtv_descriptor_index = m_descriptor_index_allocator.acquire_render_target_view_descriptor_index();
        auto rtv_descriptor_increment = m_d3d12_context->device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtv_descriptor_handle = m_d3d12_context->rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        rtv_descriptor_handle.ptr += uint64_t(rtv_descriptor_index) * rtv_descriptor_increment;
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = d3d12::make_full_texture_rtv(
            create_info.format, create_info.rtv_dimension, create_info.depth_or_array_layers, 0, 0);
        m_d3d12_context->device->CreateRenderTargetView(
            texture->resource, &rtv_desc, rtv_descriptor_handle);
    }

    if (create_info.dsv_dimension != D3D12_DSV_DIMENSION_UNKNOWN)
    {
        auto dsv_descriptor_index = m_descriptor_index_allocator.acquire_depth_stencil_view_descriptor_index();
        auto dsv_descriptor_increment = m_d3d12_context->device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        auto dsv_descriptor_handle = m_d3d12_context->dsv_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
        dsv_descriptor_handle.ptr += uint64_t(dsv_descriptor_index) * dsv_descriptor_increment;
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = d3d12::make_full_texture_dsv(
            create_info.format, create_info.dsv_dimension, create_info.depth_or_array_layers, 0);
        m_d3d12_context->device->CreateDepthStencilView(
            texture->resource, &dsv_desc, dsv_descriptor_handle);
    }

    texture->alive = true;
    return texture;
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
    *pso = {
        .type = Graphics_Pipeline_Type::VERTEX_SHADING,
        .vs = create_info.vs,
        .hs = create_info.hs,
        .ds = create_info.ds,
        .gs = create_info.gs,
        .ps = create_info.ps,
        .blend_desc = create_info.blend_desc,
        .sample_mask = create_info.sample_mask,
        .rasterizer_desc = create_info.rasterizer_desc,
        .depth_stencil_desc = create_info.depth_stencil_desc,
        .primitive_topology_type = create_info.primitive_topology_type,
        .render_target_formats = create_info.render_target_formats,
        .depth_stencil_format = create_info.depth_stencil_format
    };
    m_d3d12_context->device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso->pso));
    pso->alive = true;
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
    *pso = {
        .type = Graphics_Pipeline_Type::MESH_SHADING,
        .as = create_info.as,
        .ms = create_info.ms,
        .ps = create_info.ps,
        .blend_desc = create_info.blend_desc,
        .sample_mask = create_info.sample_mask,
        .rasterizer_desc = create_info.rasterizer_desc,
        .depth_stencil_desc = create_info.depth_stencil_desc,
        .primitive_topology_type = create_info.primitive_topology_type,
        .render_target_formats = create_info.render_target_formats,
        .depth_stencil_format = create_info.depth_stencil_format
    };
    m_d3d12_context->device->CreatePipelineState(&stream_desc, IID_PPV_ARGS(&pso->pso));
    pso->alive = true;
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
    *pso = {
        .cs = shader
    };
    m_d3d12_context->device->CreateComputePipelineState(&pso_desc, IID_PPV_ARGS(&pso->pso));
    pso->alive = true;
    return pso;
}

void Resource_Manager::destroy_gpu_resource(ID3D12Resource* resource, D3D12MA::Allocation* allocation)
{
    allocation->Release();
    resource->Release();
}

}
