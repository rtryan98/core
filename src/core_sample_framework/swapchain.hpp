#pragma once

#include <array>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

namespace core::d3d12
{
struct D3D12_Context;
}

namespace core::sf
{
class Window;

class Swapchain
{
public:
    Swapchain(Window* window, d3d12::D3D12_Context* d3d12_context);
    ~Swapchain();

    [[nodiscard]] ID3D12Resource* get_next_buffer();
    bool resize_if_size_changed();

private:
    void reset_buffers();
    void recreate_resources();

private:
    d3d12::D3D12_Context* m_d3d12_context;
    Window* m_window;
    IDXGISwapChain4* m_swapchain;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, DXGI_MAX_SWAP_CHAIN_BUFFERS> m_buffers;
};
}
