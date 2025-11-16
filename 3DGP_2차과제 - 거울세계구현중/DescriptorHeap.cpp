#include "stdafx.h"
#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap()
{
	m_d3dCPUDescriptorHandle.ptr = NULL;
	m_d3dGPUDescriptorHandle.ptr = NULL;
	m_d3dGPUObjectDescriptorHandle[0].ptr = NULL;
	m_d3dGPUObjectDescriptorHandle[1].ptr = NULL;
	m_d3dGPUSphereDescriptorHandle.ptr = NULL;
	m_d3dGPUBoundingBoxDescriptorHandle.ptr = NULL;
}

DescriptorHeap::~DescriptorHeap()
{
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();
}