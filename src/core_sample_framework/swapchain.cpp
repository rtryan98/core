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
    , m_descriptor_heap(nullptr)
{
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        .NumDescriptors = DXGI_MAX_SWAP_CHAIN_BUFFERS,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        .NodeMask = 0
    };
    m_d3d12_context->device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&m_descriptor_heap));
    recreate_resources();
}

Swapchain::~Swapchain()
{
    m_descriptor_heap->Release();
    reset_buffers();
    m_swapchain->Release();
}

Swapchain_Texture Swapchain::get_next_buffer()
{
    auto index = m_swapchain->GetCurrentBackBufferIndex();
    Swapchain_Texture result = {
        .resource = m_buffers[index].Get(),
        .descriptor = m_descriptors[index]
    };
    return result;
}

Swapchain_Resize_Result Swapchain::resize_if_size_changed()
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
        return {
            .is_resized = false,
            .width = client_width,
            .height = client_height
        };
    }

    d3d12::await_queue(m_d3d12_context->device, m_d3d12_context->direct_queue, INFINITE);
    reset_buffers();
    m_swapchain->ResizeBuffers(0, client_width, client_height, DXGI_FORMAT_UNKNOWN, desc.Flags);
    recreate_resources();

    return {
        .is_resized = resize,
        .width = client_width,
        .height = client_height
    };
}

void Swapchain::present(bool vsync, uint32_t sync_interval)
{
    DXGI_SWAP_CHAIN_DESC1 desc;
    m_swapchain->GetDesc1(&desc);
    DXGI_PRESENT_PARAMETERS present_parameters = {
        .DirtyRectsCount = 0,
        .pDirtyRects = nullptr,
        .pScrollRect = nullptr,
        .pScrollOffset = nullptr
    };
    m_swapchain->Present1(sync_interval, vsync ? desc.Flags & DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : false, &present_parameters);
}

void Swapchain::reset_buffers()
{
    m_buffers = {};
}

void Swapchain::recreate_resources()
{
    auto descriptor_cpu_handle_start = m_descriptor_heap->GetCPUDescriptorHandleForHeapStart();
    auto descriptor_increment = m_d3d12_context->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    DXGI_SWAP_CHAIN_DESC1 desc;
    m_swapchain->GetDesc1(&desc);

    for (uint32_t i = 0; i < desc.BufferCount; ++i)
    {
        auto descriptor_cpu_handle = descriptor_cpu_handle_start;
        descriptor_cpu_handle.ptr += i * descriptor_increment;
        m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_buffers[i]));
        m_buffers[i]->SetName(L"Texture::Swapchain");
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {
            .Format = desc.Format,
            .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D,
            .Texture2D = {
                .MipSlice = 0,
                .PlaneSlice = 0
            }
        };
        m_d3d12_context->device->CreateRenderTargetView(m_buffers[i].Get(), &rtv_desc, descriptor_cpu_handle);
        m_descriptors[i] = descriptor_cpu_handle;
    }
}
}
