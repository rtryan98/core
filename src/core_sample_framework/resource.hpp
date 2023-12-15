#pragma once

#include <cstdint>
#include <d3d12.h>
#include <d3d12/d3d12_shader.hpp>
#include <string>
#include <vector>

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
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor;
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

};

struct Texture_Create_Info
{

};

struct Texture : public Base_GPU_Resource
{

};

struct Shader : public Base_Resource
{
    d3d12::D3D12_Shader_Workgroups workgroups;
    D3D12_SHADER_BYTECODE bytecode;
    std::string relative_path;
};

struct Base_Pipeline : public Base_Resource
{
    ID3D12PipelineState* pso;
};

struct Graphics_Pipeline_Create_Info
{
    struct
    {
        Shader* vs;
        Shader* ds;
        Shader* hs;
        Shader* gs;
    } standard_stages;
    struct
    {
        Shader* as;
        Shader* ms;
    } mesh_shading_stages;
    Shader* ps;
};

struct Graphics_Pipeline : public Base_Pipeline
{
    Shader* vs;
    Shader* ds;
    Shader* hs;
    Shader* gs;
    Shader* as;
    Shader* ms;
    Shader* ps;
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
    Compute_Pipeline* create_compute_pipeline(Shader* shader) noexcept;

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
};

}
