#pragma once
#include "DescriptorManager.h"

class StructuredBuffer
{
public:
	StructuredBuffer() {}
	virtual ~StructuredBuffer() {}
public:
	ID3D12Resource* defaultBuffer; 
	ID3D12Resource* uploadBuffer;

	UINT elementSize = 0;
	UINT elementCount = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

	DescriptorManager* dm = nullptr;
public:
	void SetDescriptorManager(DescriptorManager* desc) { dm = desc; }

	virtual void Initialize(
		ID3D12Device* pd3dDevice,
		ID3D12GraphicsCommandList* pd3dCommandList,
		void* srcData,
		UINT count,
		UINT size);

	virtual void Update(ID3D12GraphicsCommandList* pd3dCommandList, void* srcData);

};
