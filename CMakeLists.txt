cmake_minimum_required(VERSION 3.20)

set(CORE_LIBRARY_NAME core_lib)
set(CORE_SF_LIBRARY_NAME core_sample_framework)

include(${CMAKE_CURRENT_LIST_DIR}/scripts/core.cmake)

project(
    ${CORE_LIBRARY_NAME}
    VERSION 0.1.0
    LANGUAGES CXX
)

if(NOT DEFINED CORE_D3D12_AGILITY_SDK_VERSION)
    set(CORE_D3D12_AGILITY_SDK_VERSION 611)
endif()
set(
    CORE_D3D12_AGILITY_SDK_FULL_VERSION
    1.${CORE_D3D12_AGILITY_SDK_VERSION}.0
)
set(
    CORE_D3D12_AGILITY_SDK_DOWNLOAD_PATH
    https://www.nuget.org/api/v2/package/Microsoft.Direct3D.D3D12/${CORE_D3D12_AGILITY_SDK_FULL_VERSION}
)
if(NOT DEFINED CORE_D3D12_AGILITY_SDK_DLL_PATH)
    set(CORE_D3D12_AGILITY_SDK_DLL_PATH .\\\\D3D12\\\\) # Double-escape required.
endif()

core_download_and_extract_zip(
    ${CORE_D3D12_AGILITY_SDK_DOWNLOAD_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
    d3d12_agility_sdk
)

set(D3D12MA_AGILITY_SDK_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/thirdparty/d3d12_agility_sdk CACHE STRING "Agility SDK Path for D3D12MA." FORCE)
add_subdirectory(thirdparty)

add_library(
    ${CORE_LIBRARY_NAME}
)
target_link_libraries(
    ${CORE_LIBRARY_NAME} PUBLIC
    d3d12.lib
    dxgi.lib
    D3D12MemoryAllocator
)
target_include_directories(
    ${CORE_LIBRARY_NAME} PUBLIC
    src/core
    thirdparty/d3d12_agility_sdk/build/native/include
)
set_target_properties(
    ${CORE_LIBRARY_NAME} PROPERTIES
    CXX_STANDARD 23
)

add_library(
    ${CORE_SF_LIBRARY_NAME}
)
target_link_libraries(
    ${CORE_SF_LIBRARY_NAME} PUBLIC
    ${CORE_LIBRARY_NAME}
    nlohmann_json::nlohmann_json
)
target_include_directories(
    ${CORE_SF_LIBRARY_NAME} PUBLIC
    src/core_sample_framework
)
set_target_properties(
    ${CORE_SF_LIBRARY_NAME} PROPERTIES
    CXX_STANDARD 23
)

add_subdirectory(src)

core_add_shader_include_path("${CMAKE_CURRENT_LIST_DIR}/shaders/hlsl/")
core_generate_shader_include_path_json_string()
