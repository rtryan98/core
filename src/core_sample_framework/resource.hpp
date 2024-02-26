#pragma once

#include <cstdint>
#include <agility_sdk/d3d12.h>
#include <core/d3d12/d3d12_shader.hpp>
#include <string>
#include <vector>

#include "bindless_descriptor_allocator.hpp"

namespace D3D12MA
{
class Allocation;
class Allocator;
}

namespace core::d3d12
{
struct D3D12_Context;
}

namespace core::sf
{
struct Base_Resource
{
    bool alive;
};

struct Base_GPU_Resource : public Base_Resource
{
    uint32_t bindless_index;
    DXGI_FORMAT format;
    D3D12MA::Allocation* allocation;
    ID3D12Resource2* resource;
};

struct Buffer_Create_Info
{
    uint64_t size;
    D3D12_HEAP_TYPE heap_type;
};

struct Buffer : public Base_GPU_Resource
{
    uint64_t size;
    D3D12_HEAP_TYPE heap_type;
};

struct Texture_Create_Info
{
    DXGI_FORMAT format;
    uint32_t width;
    uint32_t height;
    uint16_t depth_or_array_layers;
    uint16_t mip_levels;
    D3D12_CLEAR_VALUE optimized_clear_value; // Leave Format as DXGI_FORMAT_UNKNOWN for no clear value.
    D3D12_SRV_DIMENSION srv_dimension;
    D3D12_UAV_DIMENSION uav_dimension;
    D3D12_RTV_DIMENSION rtv_dimension;
    D3D12_DSV_DIMENSION dsv_dimension;
};

struct Texture : public Base_GPU_Resource
{
    uint32_t width;
    uint32_t height;
    uint16_t depth_or_array_layers;
    uint32_t rtv_index;
    uint32_t dsv_index;
    D3D12_GPU_DESCRIPTOR_HANDLE srv_descriptor; // Store duplicate information for imgui
    D3D12_GPU_DESCRIPTOR_HANDLE uav_descriptor;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_descriptor; // Store for convenience, could be derived from index
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_descriptor; // ^
};

struct Shader : public Base_Resource
{
    d3d12::D3D12_Shader_Workgroups workgroups;
    std::vector<char> bytecode;
    std::string relative_path;
};

struct Graphics_Pipeline_Create_Info
{
    Shader* vs;
    Shader* hs;
    Shader* ds;
    Shader* gs;
    Shader* ps;
    D3D12_BLEND_DESC blend_desc;
    uint32_t sample_mask;
    D3D12_RASTERIZER_DESC rasterizer_desc;
    D3D12_DEPTH_STENCIL_DESC1 depth_stencil_desc;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    D3D12_RT_FORMAT_ARRAY render_target_formats;
    DXGI_FORMAT depth_stencil_format;
};

struct Mesh_Shader_Pipeline_Create_Info
{
    Shader* as;
    Shader* ms;
    Shader* ps;
    D3D12_BLEND_DESC blend_desc;
    uint32_t sample_mask;
    D3D12_RASTERIZER_DESC rasterizer_desc;
    D3D12_DEPTH_STENCIL_DESC1 depth_stencil_desc;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    D3D12_RT_FORMAT_ARRAY render_target_formats;
    DXGI_FORMAT depth_stencil_format;
};

struct Base_Pipeline : public Base_Resource
{
    ID3D12PipelineState* pso;
};

enum class Graphics_Pipeline_Type
{
    VERTEX_SHADING,
    MESH_SHADING
};

struct Graphics_Pipeline : public Base_Pipeline
{
    Graphics_Pipeline_Type type;

    Shader* vs;
    Shader* hs;
    Shader* ds;
    Shader* gs;
    Shader* as;
    Shader* ms;
    Shader* ps;

    D3D12_BLEND_DESC blend_desc;
    uint32_t sample_mask;
    D3D12_RASTERIZER_DESC rasterizer_desc;
    D3D12_DEPTH_STENCIL_DESC1 depth_stencil_desc;
    D3D12_PRIMITIVE_TOPOLOGY_TYPE primitive_topology_type;
    D3D12_RT_FORMAT_ARRAY render_target_formats;
    DXGI_FORMAT depth_stencil_format;
};

struct Compute_Pipeline : public Base_Pipeline
{
    Shader* cs;
};

struct Resource_Manager_Create_Info
{
    uint32_t max_buffer_count = 65536;
    uint32_t max_texture_count = 65536;
    uint32_t max_shader_count = 65536;
    uint32_t max_graphics_pipeline_count = 65536;
    uint32_t max_compute_pipeline_count = 65536;
};

class Resource_Manager
{
public:
    explicit Resource_Manager(const Resource_Manager_Create_Info& create_info, d3d12::D3D12_Context* d3d12_context) noexcept;
    ~Resource_Manager();

    Buffer* create_buffer(const Buffer_Create_Info& create_info) noexcept;
    Texture* create_texture(const Texture_Create_Info& create_info) noexcept;
    Shader* create_shader() noexcept;
    Graphics_Pipeline* create_graphics_pipeline(const Graphics_Pipeline_Create_Info& create_info) noexcept;
    Graphics_Pipeline* create_mesh_shader_pipeline(const Mesh_Shader_Pipeline_Create_Info& create_info) noexcept;
    Compute_Pipeline* create_compute_pipeline(Shader* shader) noexcept;

private:
    void destroy_gpu_resource(ID3D12Resource* resource, D3D12MA::Allocation* allocation);

private:
    d3d12::D3D12_Context* m_d3d12_context;

    ID3D12DescriptorHeap* m_resource_descriptor_heap;
    ID3D12DescriptorHeap* m_sampler_descriptor_heap;

    std::vector<Buffer> m_buffers;
    std::vector<Texture> m_textures;
    std::vector<Shader> m_shaders;
    std::vector<Graphics_Pipeline> m_graphics_pipelines;
    std::vector<Compute_Pipeline> m_compute_pipelines;

    D3D12MA::Allocator* m_allocator;
    Bindless_Descriptor_Allocator m_descriptor_index_allocator;
};

}
