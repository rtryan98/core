#pragma once

#include <array>
#include <vector>
#include <memory>
#include <type_traits>

namespace core
{
template<typename T>
requires std::is_standard_layout_v<T>
struct Stable_Bucket_Vector_Element
{
    uint32_t bucket;
    uint32_t index;
    T* element;
};

template<typename T, std::size_t BUCKET_SIZE = 1024>
requires std::is_standard_layout_v<T> && (BUCKET_SIZE > 0)
class Stable_Bucket_Vector
{
public:
    Stable_Bucket_Vector_Element<T> create()
    {

    }

    Stable_Bucket_Vector_Element<T> create_empty(T&& initial_data)
    {

    }

    void destroy(const Stable_Bucket_Vector_Element<T>& element)
    {

    }

private:
    struct Stable_Bucket_Vector_Stored_Element
    {
        bool valid;
        uint32_t next;
        T element;
    };

    std::vector<std::unique_ptr<std::array<Stable_Bucket_Vector_Stored_Element, BUCKET_SIZE>>> m_buckets;
    Stable_Bucket_Vector_Element m_next;
};
}
