#pragma once
class DescriptorHeap
{
public:
	DescriptorHeap();
	~DescriptorHeap();

	ID3D12DescriptorHeap* m_pd3dCbvSrvDescriptorHeap = NULL;
public:
	D3D12_CPU_DESCRIPTOR_HANDLE			m_d3dCPUDescriptorHandle; 
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dGPUDescriptorHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE         m_d3dGPUObjectDescriptorHandle[2] = { 0, 0 };
	D3D12_GPU_DESCRIPTOR_HANDLE			m_d3dGPUBillboardDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE         m_d3dGPUSphereDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE         m_d3dGPUBoundingBoxDescriptorHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() { return(m_d3dCPUDescriptorHandle); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle() { return(m_d3dGPUDescriptorHandle); }
};

