#pragma once
#include "Object.h"

class DescriptorManager
{
public:
	DescriptorManager();
	~DescriptorManager();

public:
	ID3D12DescriptorHeap* m_pHeap = NULL;

	D3D12_CPU_DESCRIPTOR_HANDLE			m_cpu;
	D3D12_GPU_DESCRIPTOR_HANDLE			m_gpu;
public:
	void AdvanceHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpu, D3D12_GPU_DESCRIPTOR_HANDLE& gpu);

	// [HEAP 持失]
	void CreateCbvSrvUavHeap(ID3D12Device* pd3dDevice, int nConstantBufferViews, int nShaderResourceViews);

	// [CBV 持失]
	void CreateCBVs(ID3D12Device* pd3dDevice, int nConstantBufferViews, ID3D12Resource* pd3dConstantBuffers, UINT nStride);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateCBV(ID3D12Device* pd3dDevice, ID3D12Resource* pd3dConstantBuffer, UINT nStride);
	D3D12_GPU_DESCRIPTOR_HANDLE CreateCBVFromGpuAddress(ID3D12Device* pd3dDevice, D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress, UINT nStride);
	
	// [SRV 持失]
	void CreateTextureSRVs(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex);
	void CreateTextureSRV(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex, UINT nRootParameterStartIndex);
	void CreateTextureSRVNoRootParam(ID3D12Device* pd3dDevice, CTexture* pTexture, int nIndex);
	std::pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE> CreateSRV(
		ID3D12Device* pd3dDevice, 
		ID3D12Resource*& defaultBuffer,
		UINT elementCount,
		UINT elementSize);
};

