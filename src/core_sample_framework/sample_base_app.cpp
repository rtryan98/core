#include "sample_base_app.hpp"
#include "window_win32_default.hpp"

#include <chrono>

namespace core::sf
{
Sample_Application::Sample_Application(const Sample_Application_Create_Info& create_info)
    : m_window(nullptr)
    , m_d3d12_context()
    , m_swapchain(nullptr)
    , m_resource_manager(nullptr)
    , m_current_frame(0)
    , m_current_frame_index(0)
    , m_frame_contexts()
{
    m_window = std::make_unique<Window_Win32>(create_info.window_create_info);
    auto result = d3d12::create_d3d12_context(create_info.context_create_info, &m_d3d12_context);
    if (FAILED(result))
    {
        // print error;
    }
    m_swapchain = std::make_unique<Swapchain>(m_window.get(), &m_d3d12_context);
    m_resource_manager = std::make_unique<Resource_Manager>(create_info.resource_manager_create_info, &m_d3d12_context);

    for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; ++i)
    {
        auto& frame_context = m_frame_contexts[i];
        result = m_d3d12_context.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frame_context.fence));
        if (FAILED(result))
        {
            // print error;
        }
        result = m_d3d12_context.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&frame_context.command_allocator));
        if (FAILED(result))
        {
            // print error;
        }
        result = m_d3d12_context.device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
            D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&frame_context.command_list));
        if (FAILED(result))
        {
            // print error;
        }
        frame_context.frame = 0ull;
    }
}

Sample_Application::~Sample_Application()
{
    d3d12::await_context(&m_d3d12_context);
    for (uint32_t i = 0; i < MAX_CONCURRENT_FRAMES; ++i)
    {
        auto& frame_context = m_frame_contexts[i];
        frame_context.command_list->Release();
        frame_context.command_allocator->Release();
        frame_context.fence->Release();
    }
    m_swapchain = nullptr;
    m_resource_manager = nullptr;
    d3d12::destroy_d3d12_context(&m_d3d12_context);
}

void Sample_Application::run()
{
    auto current_time = std::chrono::system_clock::now();
    auto last_time = current_time;
    while (m_window->get_data().is_alive)
    {
        m_window->poll_events();

        auto& frame_context = m_frame_contexts[m_current_frame_index];
        if (d3d12::await_fence(frame_context.fence, frame_context.frame, INFINITE) != WAIT_OBJECT_0)
        {
            // Warn desync
        }

        auto resize_result = m_swapchain->resize_if_size_changed();
        if (resize_result.is_resized)
        {
            // resize
        }
        auto swapchain_texture = m_swapchain->get_next_buffer();

        using Duration = std::chrono::duration<double>;
        auto delta_time = std::chrono::duration_cast<Duration>(current_time - last_time).count();

        update_gui();
        update(delta_time);

        frame_context.command_list->Reset(frame_context.command_allocator, nullptr);

        auto descriptor_heaps = std::to_array({
            m_d3d12_context.resource_descriptor_heap,
            m_d3d12_context.sampler_descriptor_heap
            });
        frame_context.command_list->SetDescriptorHeaps(descriptor_heaps.size(), descriptor_heaps.data());

        render(frame_context.command_list, delta_time, swapchain_texture);

        frame_context.command_list->Close();

        auto command_lists = std::to_array({
            static_cast<ID3D12CommandList*>(frame_context.command_list)
            });
        m_d3d12_context.direct_queue->ExecuteCommandLists(uint32_t(command_lists.size()), command_lists.data());

        m_swapchain->present();

        m_current_frame += 1;
        frame_context.frame += 1;
        m_current_frame_index = m_current_frame % MAX_CONCURRENT_FRAMES;
        frame_context.fence->Signal(frame_context.frame);

        last_time = current_time;
        current_time = std::chrono::system_clock::now();
    }
}

void Sample_Application::render_gui(ID3D12GraphicsCommandList7* cmd) noexcept
{
}

}
