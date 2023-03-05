/*****************************************************************//**
 * \file   GPUBuffer.cpp
 * \brief  GPUBuffer�N���X
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "GPUBuffer.h"

//=============================================================================
// ������
//=============================================================================
void GPUBuffer::Init(D3D12_RESOURCE_DESC desc){
	m_desc = desc;
	
	const D3D12_HEAP_PROPERTIES kDefaultHeapProps ={
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		D3D12_MEMORY_POOL_UNKNOWN,
		0,
		0
	};

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();

	pDevice->CreateCommittedResource(
		&kDefaultHeapProps,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		nullptr,
		IID_PPV_ARGS(&m_buffer)
	);
}

//=============================================================================
// UAV�Ƃ��ăf�B�X�N���v�^�q�[�v�ɓo�^
//=============================================================================
void GPUBuffer::RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, int bufferNo){
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	if (m_desc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D) {
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
	}
	else {
		//���Ή��B
		std::abort();
	}

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	pDevice->CreateUnorderedAccessView(
		m_buffer,
		nullptr,
		&uavDesc,
		cpuHandle
	);


}
void GPUBuffer::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo)
{
}