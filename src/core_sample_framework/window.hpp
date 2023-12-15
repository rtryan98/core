#pragma once

#include <cstdint>
#include <string>

struct IDXGISwapChain4;

namespace core::d3d12
{
struct D3D12_Context;
}

namespace core::sf
{
struct Window_Data
{
    uint32_t width;
    uint32_t height;
    std::string title;
    bool is_alive;
};

struct Window_Create_Info
{
    uint32_t width;
    uint32_t height;
    std::string title;
};

class Window
{
public:
    Window(const Window_Create_Info& create_info)
        : m_data{ create_info.width, create_info.height, create_info.title }
    {}
    virtual ~Window() = default;

    virtual [[nodiscard]] IDXGISwapChain4* create_dxgi_swapchain(d3d12::D3D12_Context* d3d12_context) noexcept = 0;
    [[nodiscard]] const Window_Data& get_data() const noexcept { return m_data; }
    virtual void poll_events() noexcept = 0;

protected:
    Window_Data m_data = {};
};

}
