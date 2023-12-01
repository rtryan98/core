#include "window_win32_default.hpp"

#include "d3d12/d3d12_device.hpp"

namespace core::sf
{
Window_Win32::Window_Win32(const Window_Create_Info& info)
    : m_hwnd(NULL)
{

}

IDXGISwapChain4* Window_Win32::create_dxgi_swapchain(d3d12::D3D12_Context* d3d12_context) noexcept
{
    DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = {.Count = 1, .Quality = 0 },
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .BufferCount = 3, // TODO: change count?
        .Scaling = DXGI_SCALING_STRETCH,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
        .Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
    };
    IDXGISwapChain1* sc1 = nullptr;
    d3d12_context->factory->CreateSwapChainForHwnd(
        d3d12_context->direct_queue,
        m_hwnd, &swapchain_desc,
        nullptr, nullptr, &sc1);
    IDXGISwapChain4* sc4 = nullptr;
    sc1->QueryInterface(IID_PPV_ARGS(&sc4));
    sc1->Release();
    return sc4;
}
}
