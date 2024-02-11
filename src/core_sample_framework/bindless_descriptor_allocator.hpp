#pragma once

#include <cstdint>
#include <vector>

namespace core::sf
{
struct Bindless_Descriptor_Allocator_Create_Info
{
    uint32_t max_resource_descriptors;
    uint32_t max_sampler_descriptors;
    uint32_t max_rtv_descriptors;
    uint32_t max_dsv_descriptors;
};

class Bindless_Descriptor_Allocator
{
public:
    Bindless_Descriptor_Allocator(const Bindless_Descriptor_Allocator_Create_Info& create_info) noexcept;

    uint32_t acquire_resource_descriptor_index() noexcept;
    uint32_t acquire_sampler_descriptor_index() noexcept;
    uint32_t acquire_render_target_view_descriptor_index() noexcept;
    uint32_t acquire_depth_stencil_view_descriptor_index() noexcept;
    void discard_resource_descriptor_index(uint32_t index) noexcept;
    void discard_sampler_descriptor_index(uint32_t index) noexcept;
    void discard_render_target_view_descriptor_index(uint32_t index) noexcept;
    void discard_depth_stencil_view_descriptor_index(uint32_t index) noexcept;

private:
    std::vector<uint32_t> m_resource_indices;
    std::vector<uint32_t> m_sampler_indices;
    std::vector<uint32_t> m_rtv_indices;
    std::vector<uint32_t> m_dsv_indices;
};
}
