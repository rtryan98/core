#include "core/d3d12/d3d12_device.hpp"

namespace core::d3d12
{
HRESULT create_d3d12_command_queue(
    ID3D12Device* device,
    D3D12_COMMAND_LIST_TYPE type,
    bool disable_tdr,
    ID3D12CommandQueue** queue)
{
    D3D12_COMMAND_QUEUE_DESC desc = {
        .Type = type,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags = disable_tdr
            ? D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT
            : D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0
    };
    return device->CreateCommandQueue(&desc, IID_PPV_ARGS(queue));
}

void get_d3d12_features(D3D12_Context* context)
{
    context->features = {};
    auto check_feature = [context](D3D12_FEATURE feature, auto& feature_data) {
        context->device->CheckFeatureSupport(feature, &feature_data, sizeof(feature_data));
    };
    check_feature(D3D12_FEATURE_FEATURE_LEVELS, context->features.levels);
    check_feature(D3D12_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT, context->features.gpu_virtual_address_support);
    check_feature(D3D12_FEATURE_SHADER_MODEL, context->features.shader_model);
    check_feature(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_SUPPORT, context->features.protected_resource_session_support);
    check_feature(D3D12_FEATURE_ROOT_SIGNATURE, context->features.root_signature);
    check_feature(D3D12_FEATURE_ARCHITECTURE1, context->features.architecture1);
    check_feature(D3D12_FEATURE_SHADER_CACHE, context->features.shader_cache);
    check_feature(D3D12_FEATURE_EXISTING_HEAPS, context->features.existing_heaps);
    check_feature(D3D12_FEATURE_SERIALIZATION, context->features.serialization);
    check_feature(D3D12_FEATURE_DISPLAYABLE, context->features.displayable);
    check_feature(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_TYPE_COUNT, context->features.protected_resource_session_type_count);
    check_feature(D3D12_FEATURE_PROTECTED_RESOURCE_SESSION_TYPES, context->features.protected_resource_session_types);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS, context->features.options);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS1,  context->features.options1);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS2,  context->features.options2);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS3,  context->features.options3);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS4,  context->features.options4);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS5,  context->features.options5);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS6,  context->features.options6);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS7,  context->features.options7);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS8,  context->features.options8);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS9,  context->features.options9);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS10, context->features.options10);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS11, context->features.options11);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS12, context->features.options12);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS13, context->features.options13);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS14, context->features.options14);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS15, context->features.options15);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS16, context->features.options16);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS17, context->features.options17);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS18, context->features.options18);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS19, context->features.options19);
    check_feature(D3D12_FEATURE_D3D12_OPTIONS20, context->features.options20);
}

HRESULT create_d3d12_context(const D3D12_Context_Create_Info& create_info, D3D12_Context* context)
{
    HRESULT result = S_OK;
    uint32_t factory_flags = 0;
    if (create_info.enable_validation)
    {
        factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
        ID3D12Debug6* debug = nullptr;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
        {
            debug->EnableDebugLayer();
            if (create_info.enable_gpu_validation)
            {
                debug->SetEnableGPUBasedValidation(true);
            }
            debug->Release();
        }
    }
    result = CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&context->factory));
    if (FAILED(result))
    {
        return result;
    }
    result = context->factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&context->adapter));
    if (FAILED(result))
    {
        return result;
    }
    result = D3D12CreateDevice(context->adapter, create_info.feature_level, IID_PPV_ARGS(&context->device));
    if (FAILED(result))
    {
        return result;
    }
    get_d3d12_features(context);
    result = create_d3d12_command_queue(context->device, D3D12_COMMAND_LIST_TYPE_DIRECT, create_info.disable_tdr, &context->direct_queue);
    if (FAILED(result))
    {
        return result;
    }
    result = create_d3d12_command_queue(context->device, D3D12_COMMAND_LIST_TYPE_COMPUTE, create_info.disable_tdr, &context->compute_queue);
    if (FAILED(result))
    {
        return result;
    }
    result = create_d3d12_command_queue(context->device, D3D12_COMMAND_LIST_TYPE_COPY, create_info.disable_tdr, &context->copy_queue);
    if (FAILED(result))
    {
        return result;
    }
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
        .Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        .NumDescriptors = create_info.resource_descriptor_heap_size,
        .Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
        .NodeMask = 0
    };
    result = context->device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&context->resource_descriptor_heap));
    if (FAILED(result))
    {
        return result;
    }

    descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
    descriptor_heap_desc.NumDescriptors = create_info.sampler_descriptor_heap_size;
    result = context->device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&context->sampler_descriptor_heap));
    if (FAILED(result))
    {
        return result;
    }

    descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    descriptor_heap_desc.NumDescriptors = create_info.rtv_descriptor_heap_size;
    descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    result = context->device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&context->rtv_descriptor_heap));
    if (FAILED(result))
    {
        return result;
    }

    descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    descriptor_heap_desc.NumDescriptors = create_info.dsv_descriptor_heap_size;
    result = context->device->CreateDescriptorHeap(&descriptor_heap_desc, IID_PPV_ARGS(&context->dsv_descriptor_heap));
    if (FAILED(result))
    {
        return result;
    }

    D3D12_ROOT_PARAMETER1 push_constants_root_para = {
        .ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
        .Constants = {
            .ShaderRegister = 0,
            .RegisterSpace = 0,
            .Num32BitValues = create_info.push_constant_size >> 2
        },
        .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL
    };
    D3D12_VERSIONED_ROOT_SIGNATURE_DESC versioned_root_signature_desc = {
        .Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
        .Desc_1_1 = {
            .NumParameters = 1,
            .pParameters = &push_constants_root_para,
            .NumStaticSamplers = uint32_t(create_info.static_samplers.size()),
            .pStaticSamplers = create_info.static_samplers.data(),
            .Flags = D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED
                | D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED
        }
    };
    ID3DBlob* root_signature_blob;
    result = D3D12SerializeVersionedRootSignature(&versioned_root_signature_desc, &root_signature_blob, nullptr);
    if (FAILED(result))
    {
        return result;
    }
    result = context->device->CreateRootSignature(0,
        root_signature_blob->GetBufferPointer(),
        root_signature_blob->GetBufferSize(),
        IID_PPV_ARGS(&context->bindless_root_signature));
    root_signature_blob->Release();
    if (FAILED(result))
    {
        return result;
    }

    return S_OK;
}

HRESULT destroy_d3d12_context(D3D12_Context* context)
{
    context->bindless_root_signature->Release();
    context->dsv_descriptor_heap->Release();
    context->rtv_descriptor_heap->Release();
    context->sampler_descriptor_heap->Release();
    context->resource_descriptor_heap->Release();
    context->copy_queue->Release();
    context->compute_queue->Release();
    context->direct_queue->Release();
    ID3D12DebugDevice* debug_device;
    bool has_debug_device = SUCCEEDED(context->device->QueryInterface(IID_PPV_ARGS(&debug_device)));
    context->device->Release();
    context->adapter->Release();
    context->factory->Release();
    if (has_debug_device)
    {
        debug_device->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL | D3D12_RLDO_IGNORE_INTERNAL);
    }
    return S_OK;
}

DWORD await_fence(ID3D12Fence* fence, uint64_t val, uint64_t timeout)
{
    DWORD result = WAIT_FAILED;
    if (fence->GetCompletedValue() < val)
    {
        HANDLE event_handle = CreateEvent(NULL, FALSE, FALSE, NULL);
        fence->SetEventOnCompletion(val, event_handle);
        if (event_handle != 0)
        {
            result = WaitForSingleObject(event_handle, timeout);
            CloseHandle(event_handle);
        }
    }
    else
    {
        result = WAIT_OBJECT_0;
    }
    return result;
}

DWORD await_queue(ID3D12Device* device, ID3D12CommandQueue* queue, uint64_t timeout)
{
    ID3D12Fence1* fence;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    if (!fence)
    {
        return WAIT_FAILED;
    }
    queue->Signal(fence, 1);
    auto result = await_fence(fence, 1, timeout);
    fence->Release();
    return result;
}

DWORD await_context(D3D12_Context* context)
{
    if (auto result = await_queue(context->device, context->direct_queue,  INFINITE)) return result;
    if (auto result = await_queue(context->device, context->compute_queue, INFINITE)) return result;
    if (auto result = await_queue(context->device, context->copy_queue,    INFINITE)) return result;
    return WAIT_OBJECT_0;
}
}
