cmake_minimum_required(VERSION 3.20)

set(CORE_LIBRARY_NAME core_lib)

option(CORE_LIBRARY_COMPILE_D3D12 ON)

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

add_library(
    ${CORE_LIBRARY_NAME}
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

add_subdirectory(src)