#include "window_win32_default.hpp"

#include "d3d12/d3d12_device.hpp"

namespace core::sf
{
LRESULT CALLBACK wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Window_Data* data = reinterpret_cast<Window_Data*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg)
    {
    case WM_CLOSE:
        if (data)
        {
            data->is_alive = false;
        }
        break;
    case WM_GETMINMAXINFO:
    {
        auto& size = reinterpret_cast<LPMINMAXINFO>(lparam)->ptMinTrackSize;
        size.x = 256;
        size.y = 144;
        break;
    }
    case WM_SIZE:
    {
        if (data)
        {
            RECT rect;
            GetClientRect(hwnd, &rect);
            data->width = rect.right;
            data->height = rect.bottom;
        }
        break;
    }
    default:
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

Window_Win32::Window_Win32(const Window_Create_Info& info)
    : Window(info)
    , m_hwnd(NULL)
{
    RECT wr = {
        .left = LONG((GetSystemMetrics(SM_CXSCREEN) - m_data.width) / 2),
        .top = LONG((GetSystemMetrics(SM_CYSCREEN) - m_data.height) / 2),
        .right = LONG(m_data.width),
        .bottom = LONG(m_data.height)
    };
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRectEx(&wr, style, FALSE, WS_EX_TOOLWINDOW);
    WNDCLASSEX wc = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = 0,
        .lpfnWndProc = wnd_proc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = GetModuleHandle(NULL),
        .hIcon = LoadIcon(NULL, IDI_WINLOGO),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = HBRUSH(GetStockObject(BLACK_BRUSH)),
        .lpszMenuName = nullptr,
        .lpszClassName = m_data.title.c_str(),
        .hIconSm = wc.hIcon
    };
    RegisterClassEx(&wc);
    m_hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        wc.lpszClassName,
        style,
        wr.left,
        wr.top,
        wr.right,
        wr.bottom,
        nullptr,
        nullptr,
        GetModuleHandle(NULL),
        0);
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)(&m_data));
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    SetForegroundWindow(m_hwnd);
    SetFocus(m_hwnd);
    m_data.is_alive = true;
}

Window_Win32::~Window_Win32()
{
    DestroyWindow(m_hwnd);
}

IDXGISwapChain4* Window_Win32::create_dxgi_swapchain(d3d12::D3D12_Context* d3d12_context) noexcept
{
    DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {
        .Format = DXGI_FORMAT_B8G8R8A8_UNORM,
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

void Window_Win32::poll_events() noexcept
{
    MSG message = {};
    ZeroMemory(&message, sizeof(MSG));
    while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}
}
