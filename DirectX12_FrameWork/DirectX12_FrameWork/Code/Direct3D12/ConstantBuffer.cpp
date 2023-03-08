/*****************************************************************//**
 * \file   ConstantBuffer.cpp
 * \brief  �萔�o�b�t�@�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "ConstantBuffer.h"

//=============================================================================
// �f�X�g���N�^
//=============================================================================
ConstantBuffer::~ConstantBuffer()
{
	//�A���}�b�v
	CD3DX12_RANGE readRange(0, 0);
	for (auto& cb : m_pConstantBuffer) {
		if (cb != nullptr) {
			cb->Unmap(0, &readRange);
			cb->Release();
		}
	}
}

//=============================================================================
// ������
//=============================================================================
void ConstantBuffer::Init(size_t size, void* srcData){

	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();

	if (size == 0){
		return;
	}

	// �T�C�Y�m��
	m_size = size;

	//�萔�o�b�t�@��256�o�C�g�A���C�����g���v�������̂ŁA256�̔{���ɐ؂�グ��
	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	m_allocSize = (size + align) & 0xFFFFFF00;
	
	//�萔�o�b�t�@�̍쐬
	int bufferNo = 0;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
	for (auto& cb : m_pConstantBuffer) {
		
		pDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&cb)
		);

		//�萔�o�b�t�@��CPU����A�N�Z�X�\�ȉ��z�A�h���X��ԂɃ}�b�s���O����
		{
			CD3DX12_RANGE readRange(0, 0); // CPU��̂��̃��\�[�X����ǂݎ�邱�Ƃ��Ӑ}���Ă���
			cb->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstBufferCPU[bufferNo]));
		}

		// �������f�[�^������ΐݒ�
		if (srcData != nullptr) {
			memcpy(m_pConstBufferCPU[bufferNo], srcData, m_size);
		}
		bufferNo++;
	}

	//���p�\�ɂ���
	m_isValid = true;
}

//=============================================================================
// CBV�ɓo�^
//=============================================================================
void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle){
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	RegistConstantBufferView(descriptorHandle, backBufferIndex);
}
void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo){
	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = m_pConstantBuffer[bufferNo]->GetGPUVirtualAddress();
	desc.SizeInBytes = m_allocSize;
	pDevice->CreateConstantBufferView(&desc, descriptorHandle);
}

//=============================================================================
// VRAM(GPU��)��̉��z�A�h���X���擾
//=============================================================================
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress(){
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_pConstantBuffer[backBufferIndex]->GetGPUVirtualAddress();
}

//=============================================================================
// �}�b�s���O�ς̃|�C���^���擾
//=============================================================================
void* ConstantBuffer::GetMappedPtr() const{
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_pConstBufferCPU[backBufferIndex];
}

//=============================================================================
// �R�s�[
//=============================================================================
void ConstantBuffer::CopyToVRAM(void* data){
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	memcpy(m_pConstBufferCPU[backBufferIndex], data, m_size);
}
