target_sources(
    ${CORE_SF_LIBRARY_NAME} PRIVATE
    bindless_descriptor_allocator.cpp
    bindless_descriptor_allocator.hpp
    resource.cpp
    resource.hpp
    sample_base_app.cpp
    sample_base_app.hpp
    swapchain.cpp
    swapchain.hpp
    window.hpp
    window_win32_default.cpp
    window_win32_default.hpp
)
