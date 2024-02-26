#include "core/d3d12/d3d12_shader.hpp"

namespace core::d3d12
{
HRESULT create_d3d12_shader_reflect_context(D3D12_Shader_Reflect_Context* context)
{
    return DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&context->dxc_utils));
}

void destroy_d3d12_shader_reflect_context(D3D12_Shader_Reflect_Context* context)
{
    context->dxc_utils->Release();
}

D3D12_Shader_Workgroups D3D12_Shader_Reflect_Context::reflect_workgroups(std::span<char> shader_bytecode)
{
    D3D12_Shader_Workgroups result = {};
    ID3D12ShaderReflection* reflection = nullptr;
    DxcBuffer reflection_buffer = {
        .Ptr = shader_bytecode.data(),
        .Size = shader_bytecode.size_bytes(),
        .Encoding = DXC_CP_ACP
    };
    dxc_utils->CreateReflection(&reflection_buffer, IID_PPV_ARGS(&reflection)); // TODO: error check
    reflection->GetThreadGroupSize(&result.x, &result.y, &result.z);
    reflection->Release();
    return result;
}
}
