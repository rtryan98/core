#pragma once

#include <span>
#include <d3d12shader.h>
#include <dxcapi.h>

namespace core::d3d12
{
struct D3D12_Shader_Workgroups
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

struct D3D12_Shader_Reflect_Context
{
    IDxcUtils* dxc_utils;

    D3D12_Shader_Workgroups reflect_workgroups(std::span<char> shader_bytecode);
};

HRESULT create_d3d12_shader_reflect_context(D3D12_Shader_Reflect_Context* context);
void destroy_d3d12_shader_reflect_context(D3D12_Shader_Reflect_Context* context);
}
