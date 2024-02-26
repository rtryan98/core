configure_file(agility.cpp.in ${CMAKE_CURRENT_LIST_DIR}/agility.cpp ESCAPE_QUOTES @ONLY)

target_sources(
    ${CORE_LIBRARY_NAME} PRIVATE
    agility.cpp
    d3d12_descriptor_util.cpp
    d3d12_descriptor_util.hpp
    d3d12_device.cpp
    d3d12_device.hpp
    d3d12_shader.cpp
    d3d12_shader.hpp
    d3d12_pso.cpp
    d3d12_pso.hpp
)
