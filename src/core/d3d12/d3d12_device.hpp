#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <cstdint>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace core::d3d12
{
struct D3D12_Features
{
    // D3D12_FEATURE_DATA_ARCHITECTURE architecture; // Superseeded by architecture1
    D3D12_FEATURE_DATA_FEATURE_LEVELS levels;
    D3D12_FEATURE_DATA_GPU_VIRTUAL_ADDRESS_SUPPORT gpu_virtual_address_support;
    D3D12_FEATURE_DATA_SHADER_MODEL shader_model;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_SUPPORT protected_resource_session_support;
    D3D12_FEATURE_DATA_ROOT_SIGNATURE root_signature;
    D3D12_FEATURE_DATA_ARCHITECTURE1 architecture1;
    D3D12_FEATURE_DATA_SHADER_CACHE shader_cache;
    D3D12_FEATURE_DATA_EXISTING_HEAPS existing_heaps;
    D3D12_FEATURE_DATA_SERIALIZATION serialization;
    D3D12_FEATURE_DATA_DISPLAYABLE displayable;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPE_COUNT protected_resource_session_type_count;
    D3D12_FEATURE_DATA_PROTECTED_RESOURCE_SESSION_TYPES protected_resource_session_types;
    D3D12_FEATURE_DATA_D3D12_OPTIONS   options;
    D3D12_FEATURE_DATA_D3D12_OPTIONS1  options1;
    D3D12_FEATURE_DATA_D3D12_OPTIONS2  options2;
    D3D12_FEATURE_DATA_D3D12_OPTIONS3  options3;
    D3D12_FEATURE_DATA_D3D12_OPTIONS4  options4;
    D3D12_FEATURE_DATA_D3D12_OPTIONS5  options5;
    D3D12_FEATURE_DATA_D3D12_OPTIONS6  options6;
    D3D12_FEATURE_DATA_D3D12_OPTIONS7  options7;
    D3D12_FEATURE_DATA_D3D12_OPTIONS8  options8;
    D3D12_FEATURE_DATA_D3D12_OPTIONS9  options9;
    D3D12_FEATURE_DATA_D3D12_OPTIONS10 options10;
    D3D12_FEATURE_DATA_D3D12_OPTIONS11 options11;
    D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12;
    D3D12_FEATURE_DATA_D3D12_OPTIONS13 options13;
    D3D12_FEATURE_DATA_D3D12_OPTIONS14 options14;
    D3D12_FEATURE_DATA_D3D12_OPTIONS15 options15;
    D3D12_FEATURE_DATA_D3D12_OPTIONS16 options16;
    D3D12_FEATURE_DATA_D3D12_OPTIONS17 options17;
    D3D12_FEATURE_DATA_D3D12_OPTIONS18 options18;
    D3D12_FEATURE_DATA_D3D12_OPTIONS19 options19;
    D3D12_FEATURE_DATA_D3D12_OPTIONS20 options20;
};

struct D3D12_Context_Create_Info
{
    bool enable_validation;
    bool enable_gpu_validation;
    bool disable_tdr;
    D3D_FEATURE_LEVEL feature_level;
};

struct D3D12_Context
{
    IDXGIFactory7* factory;
    IDXGIAdapter4* adapter;
    ID3D12Device10* device;
    ID3D12CommandQueue* direct_queue;
    ID3D12CommandQueue* compute_queue;
    ID3D12CommandQueue* copy_queue;
    D3D12_Features features;
};

HRESULT create_d3d12_context(D3D12_Context_Create_Info* create_info, D3D12_Context* context);
HRESULT destroy_d3d12_context(D3D12_Context* context);
}
