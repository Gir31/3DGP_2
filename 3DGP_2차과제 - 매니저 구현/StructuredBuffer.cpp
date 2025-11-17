#include "stdafx.h"
#include "StructuredBuffer.h"

void StructuredBuffer::Initialize(
	ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList,
	void* srcData,
	UINT count,
	UINT size)
{
    elementSize = size;
    elementCount = count;
    UINT totalSize = count * size;

    // 1) Default Heap
    defaultBuffer = CreateBufferResource(
        pd3dDevice, nullptr, nullptr, totalSize,
        D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST, nullptr);

    // 2) Upload Heap
    uploadBuffer = CreateBufferResource(
        pd3dDevice, nullptr, srcData, totalSize,
        D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);

    // 3) CopyResource
    pd3dCommandList->CopyResource(defaultBuffer, uploadBuffer);

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = defaultBuffer;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    pd3dCommandList->ResourceBarrier(1, &barrier);

    std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> handle = dm->CreateSRV( 
        pd3dDevice, 
        defaultBuffer, 
        elementCount, 
        elementSize  
    );

    cpuHandle = handle.first;
    gpuHandle = handle.second;
}

void StructuredBuffer::Update(ID3D12GraphicsCommandList* pd3dCommandList, void* srcData)
{
    UINT totalSize = elementCount * elementSize;

    void* mapped = nullptr;
    uploadBuffer->Map(0, nullptr, &mapped);
    memcpy(mapped, srcData, totalSize);
    uploadBuffer->Unmap(0, nullptr);

    D3D12_RESOURCE_BARRIER toCopy = {};
    toCopy.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toCopy.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toCopy.Transition.pResource = defaultBuffer;
    toCopy.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    toCopy.Transition.StateBefore = D3D12_RESOURCE_STATE_GENERIC_READ;
    toCopy.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
    pd3dCommandList->ResourceBarrier(1, &toCopy);

	pd3dCommandList->CopyResource(defaultBuffer, uploadBuffer);

    D3D12_RESOURCE_BARRIER toRead = {};
    toRead.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    toRead.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    toRead.Transition.pResource = defaultBuffer;
    toRead.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    toRead.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    toRead.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
    pd3dCommandList->ResourceBarrier(1, &toRead);
}