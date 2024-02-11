#include "bindless_descriptor_allocator.hpp"

namespace core::sf
{
Bindless_Descriptor_Allocator::Bindless_Descriptor_Allocator(
    const Bindless_Descriptor_Allocator_Create_Info& create_info) noexcept
    : m_resource_indices(), m_sampler_indices()
{
    m_resource_indices.reserve(create_info.max_resource_descriptors / 2);
    for (int32_t index = int32_t(create_info.max_resource_descriptors / 2) - 1; index >= 0; --index)
    {
        m_resource_indices.push_back(uint32_t(index));
    }
    m_sampler_indices.reserve(create_info.max_sampler_descriptors);
    for (int32_t index = int32_t(create_info.max_sampler_descriptors) - 1; index >= 0; --index)
    {
        m_sampler_indices.push_back(uint32_t(index));
    }
    m_rtv_indices.reserve(create_info.max_rtv_descriptors);
    for (int32_t index = int32_t(create_info.max_rtv_descriptors) - 1; index >= 0; --index)
    {
        m_rtv_indices.push_back(uint32_t(index));
    }
    m_dsv_indices.reserve(create_info.max_dsv_descriptors);
    for (int32_t index = int32_t(create_info.max_dsv_descriptors) - 1; index >= 0; --index)
    {
        m_dsv_indices.push_back(uint32_t(index));
    }
}

uint32_t Bindless_Descriptor_Allocator::acquire_resource_descriptor_index() noexcept
{
    auto index = m_resource_indices.back();
    m_resource_indices.pop_back();
    return index;
}

uint32_t Bindless_Descriptor_Allocator::acquire_sampler_descriptor_index() noexcept
{
    auto index = m_sampler_indices.back();
    m_sampler_indices.pop_back();
    return index;
}

uint32_t Bindless_Descriptor_Allocator::acquire_render_target_view_descriptor_index() noexcept
{
    auto index = m_rtv_indices.back();
    m_rtv_indices.pop_back();
    return index;
}

uint32_t Bindless_Descriptor_Allocator::acquire_depth_stencil_view_descriptor_index() noexcept
{
    auto index = m_dsv_indices.back();
    m_dsv_indices.pop_back();
    return index;
}

void Bindless_Descriptor_Allocator::discard_resource_descriptor_index(uint32_t index) noexcept
{
    m_resource_indices.push_back(index);
}

void Bindless_Descriptor_Allocator::discard_sampler_descriptor_index(uint32_t index) noexcept
{
    m_sampler_indices.push_back(index);
}

void Bindless_Descriptor_Allocator::discard_render_target_view_descriptor_index(uint32_t index) noexcept
{
    m_rtv_indices.push_back(index);
}

void Bindless_Descriptor_Allocator::discard_depth_stencil_view_descriptor_index(uint32_t index) noexcept
{
    m_dsv_indices.push_back(index);
}

}
