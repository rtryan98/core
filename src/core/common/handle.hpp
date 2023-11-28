#pragma once

#include <cstdint>
#include <functional>
#include <type_traits>
#include <vector>

namespace core
{
template<typename ID, uint32_t Generation_Size, uint32_t Flag_Size = 32u - Generation_Size>
struct Handle_Specification
{
    using Type = ID;
    constexpr static uint32_t GENERATION_SIZE = Generation_Size;
    constexpr static uint32_t FLAG_SIZE = Flag_Size;
};

template<Handle_Specification Specification>
struct Handle
{
    uint32_t index;
    uint32_t generation : Specification::GENERATION_SIZE;
    uint32_t flags : Specification::FLAG_SIZE;
};

template<Handle_Specification Specification, Handle<Specification> Handle_Type, class Data>
class Handle_Allocator
{
    static_assert(std::is_standard_layout_v<Data>);

    constexpr static uint32_t NO_HEAD = ~0u;

public:
    explicit Handle_Allocator(std::size_t initial_capacity)
        : m_data()
        , m_head(NO_HEAD)
    {
        m_data.reserve(initial_capacity);
    }

    void for_each(std::function<void(uint32_t generation, uint32_t flags)>&& per_element_fn)
    {
        for (auto i = 0; i < m_data.size(); ++i)
        {
            per_element_fn(generation, flags);
        }
    }

    [[nodiscard]] Handle_Type insert(uint32_t flags) noexcept
    {
        uint32_t idx = 0u;
        if (NO_HEAD != m_head)
        {
            idx = m_head;
            m_head = m_data[idx].next;
        }
        else
        {
            m_data.push_back({});
            idx = uint32_t(m_data.size() - 1u);
        }
        auto& element = m_data[idx];
        element.value = {};
        element.flags = flags;
        return Handle_Type {
            .index = idx,
            .generation = element.generation,
            .flags = flags
        };
    }

    void remove(Handle_Type handle) noexcept
    {
        auto& element = m_data[handle.index];
        element.flags = 0;
        element.gen += 1;
        element.value = {};
        element.next = m_head;
        m_head = handle.index;
    }

    [[nodiscard]] Data& operator[](Handle_Type handle)
    {
        return m_data[handle.index].value;
    }

private:
    struct Handle_Data_Store
    {
        uint32_t generation;
        uint32_t flags;
        uint32_t next;
        Data value;
    };
    std::vector<Handle_Data_Store> m_data;
    uint32_t m_head;
};
}
