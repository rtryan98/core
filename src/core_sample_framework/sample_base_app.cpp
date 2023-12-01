#include "sample_base_app.hpp"
#include "window_win32_default.hpp"

#include <chrono>

namespace core::sf
{
Sample_Application::Sample_Application(const Sample_Application_Create_Info& create_info)
    : m_running(false)
    , m_window(nullptr)
    , m_d3d12_context()
    , m_resource_manager()
{
    m_window = std::make_unique<Window_Win32>(create_info.window_create_info);
    d3d12::create_d3d12_context(create_info.context_create_info, &m_d3d12_context);
}

Sample_Application::~Sample_Application()
{
    d3d12::await_context(&m_d3d12_context);
    d3d12::destroy_d3d12_context(&m_d3d12_context);
}

void Sample_Application::run()
{
    m_running = true;
    auto current_time = std::chrono::system_clock::now();
    auto last_time = current_time;
    while (m_window->get_data().is_alive)
    {
        using Duration = std::chrono::duration<double>;
        auto delta_time = std::chrono::duration_cast<Duration>(current_time - last_time).count();

        update_gui();
        update(delta_time);

        ID3D12GraphicsCommandList9* cmd = nullptr;
        render(cmd, delta_time);
        render_gui(cmd);

        last_time = current_time;
        current_time = std::chrono::system_clock::now();
    }
}

void Sample_Application::render_gui(ID3D12GraphicsCommandList9* cmd) noexcept
{
}

}
