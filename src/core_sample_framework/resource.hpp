#pragma once

#include <cstdint>
#include <d3d12.h>

namespace core::sf
{
struct Base_GPU_Resource
{
    uint32_t bindless_index;
    DXGI_FORMAT format;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_descriptor;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_descriptor;
};

struct Buffer : public Base_GPU_Resource
{

};

struct Texture : public Base_GPU_Resource
{
    D3D12_BARRIER_LAYOUT current_layout;
};

struct Shader
{

};

struct Graphics_Pipeline
{

};

class Resource_Manager
{

};

}
