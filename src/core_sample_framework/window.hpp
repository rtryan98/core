#pragma once

namespace core::sf
{
class Swapchain;

struct Window_Data
{

};

struct Window_Create_Info
{

};

class Window
{
public:
    Window([[maybe_unused]] const Window_Create_Info& create_info) {};
    virtual ~Window() = default;

    virtual [[nodiscard]] Swapchain* create_swapchain() noexcept = 0;
};

}
