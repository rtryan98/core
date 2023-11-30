#pragma once

#include "d3d12/d3d12_device.hpp"
#include "resource.hpp"
#include "window.hpp"

#include <memory>

namespace core::sf
{
struct Sample_Application_Create_Info
{
    Window_Create_Info window_create_info;
    d3d12::D3D12_Context_Create_Info context_create_info;
};

class Sample_Application
{
public:
    explicit Sample_Application(const Sample_Application_Create_Info& create_info);
    virtual ~Sample_Application();

    void run();

    virtual void update(double delta_time) noexcept {};
    virtual void update_gui() noexcept {};
    virtual void render(ID3D12GraphicsCommandList9* cmd, double delta_time) noexcept = 0;

private:
    void render_gui(ID3D12GraphicsCommandList9* cmd) noexcept;

protected:
    bool m_running;
    std::unique_ptr<Window> m_window;
    d3d12::D3D12_Context m_d3d12_context;
    Resource_Manager m_resource_manager;
};
}
