#include "swapchain.hpp"

#include "window.hpp"
#include "d3d12/d3d12_device.hpp"

namespace core::sf
{
Swapchain::Swapchain(Window* window, d3d12::D3D12_Context* d3d12_context)
    : m_d3d12_context(d3d12_context)
    , m_window(window)
    , m_swapchain(m_window->create_dxgi_swapchain(m_d3d12_context))
    , m_buffers()
{

}

Swapchain::~Swapchain()
{
    reset_buffers();
    m_swapchain->Release();
}

ID3D12Resource* Swapchain::get_next_buffer()
{
    auto index = m_swapchain->GetCurrentBackBufferIndex();
    return m_buffers[index].Get();
}

bool Swapchain::resize_if_size_changed()
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    m_swapchain->GetDesc1(&desc);
    HWND hwnd = nullptr;
    m_swapchain->GetHwnd(&hwnd);

    RECT rect = {};
    bool client_rect_result = GetClientRect(hwnd, &rect);
    uint32_t client_width = rect.right - rect.left;
    uint32_t client_height = rect.bottom - rect.top;

    bool has_area = false;
    has_area = (client_width > 0) && (client_height > 0);

    bool resize = false;
    resize = ((client_width != desc.Width) || (client_height != desc.Height)) && has_area;

    if (!(resize && client_rect_result))
    {
        return false;
    }

    d3d12::await_queue(m_d3d12_context->device, m_d3d12_context->direct_queue, INFINITE);
    reset_buffers();
    m_swapchain->ResizeBuffers(0, client_width, client_height, DXGI_FORMAT_UNKNOWN, desc.Flags);
    recreate_resources();

    return resize;
}

void Swapchain::reset_buffers()
{
    m_buffers = {};
}

void Swapchain::recreate_resources()
{

}
}
