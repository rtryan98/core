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

struct Swapchain_Texture
{
    ID3D12Resource* resource;
    D3D12_CPU_DESCRIPTOR_HANDLE descriptor;
};

struct Swapchain_Resize_Result
{
    bool is_resized;
    uint32_t width;
    uint32_t height;
};

class Swapchain
{
public:
    Swapchain(Window* window, d3d12::D3D12_Context* d3d12_context);
    ~Swapchain();

    [[nodiscard]] Swapchain_Texture get_next_buffer();
    Swapchain_Resize_Result resize_if_size_changed();
    void present(bool vsync = false, uint32_t sync_interval = 0);

private:
    void reset_buffers();
    void recreate_resources();

private:
    d3d12::D3D12_Context* m_d3d12_context;
    Window* m_window;
    IDXGISwapChain4* m_swapchain;
    std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, DXGI_MAX_SWAP_CHAIN_BUFFERS> m_buffers;
    ID3D12DescriptorHeap* m_descriptor_heap;
    std::array<D3D12_CPU_DESCRIPTOR_HANDLE, DXGI_MAX_SWAP_CHAIN_BUFFERS> m_descriptors;
};
}
