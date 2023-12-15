#pragma once

#include "window.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace core::sf
{
class Window_Win32 : public Window
{
public:
    Window_Win32(const Window_Create_Info& info);
    virtual ~Window_Win32() override;

    virtual [[nodiscard]] IDXGISwapChain4* create_dxgi_swapchain(d3d12::D3D12_Context* d3d12_context) noexcept override;
    virtual void poll_events() noexcept override;

private:
    HWND m_hwnd;
};
}
