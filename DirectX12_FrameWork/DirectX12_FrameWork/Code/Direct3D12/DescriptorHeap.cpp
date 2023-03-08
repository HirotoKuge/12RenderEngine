/*****************************************************************//**
 * \file   DescriptorHeap.cpp
 * \brief  �f�B�X�N���v�^�q�[�v�N���X
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#include "../stdafx.h"
#include "DescriptorHeap.h"


//=============================================================================
// �R���X�g���N�^
//=============================================================================
DescriptorHeap::DescriptorHeap(){
	m_shaderResources.resize(MAX_SHADER_RESOURCE);
	m_uavResources.resize(MAX_SHADER_RESOURCE);
	m_constantBuffers.resize(MAX_CONSTANT_BUFFER);
	for (auto& srv : m_shaderResources) {
		srv = nullptr;
	}
	for (auto& uav : m_uavResources) {
		uav = nullptr;
	}
	for (auto& cbv : m_constantBuffers) {
		cbv = nullptr;
	}
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
DescriptorHeap::~DescriptorHeap(){
	for (auto& ds : m_descriptorHeap) {
		if (ds) {
			ds->Release();
		}
	}
}

//=============================================================================
// �T���v���[
//=============================================================================
void DescriptorHeap::CommitSamplerHeap(){
	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numSamplerDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂���", L"�G���[", MB_OK);
			std::abort();
		}
	}
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int i = 0; i < m_numSamplerDesc; i++) {
			//�T���v���X�e�[�g���f�B�X�N���v�^�q�[�v�ɓo�^���Ă���
			pDevice->CreateSampler(&m_samplerDescs[i], cpuHandle);
			cpuHandle.ptr += m_samplerDescriptorSize;
		}
		m_samplerGpuDescriptorStart[bufferNo] = gpuHandle;
		bufferNo++;
	}

}

int g_numDescriptorHeap = 0;


//=============================================================================
// �q�[�v�ւ̓o�^���m��
//=============================================================================
void DescriptorHeap::Commit(){
	ID3D12Device5* pDevice = GraphicsEngine::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numShaderResource + m_numConstantBuffer + m_numUavResource;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		g_numDescriptorHeap++;
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂���", L"�G���[", MB_OK);
			std::abort();
		}
	}
	//�萔�o�b�t�@��V�F�[�_�[���\�[�X�̃f�B�X�N���v�^���q�[�v�ɏ�������ł���
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		//�萔�o�b�t�@��o�^���Ă���
		for (int i = 0; i < m_numConstantBuffer; i++) {
			//@todo bug
			if (m_constantBuffers[i] != nullptr) {
				m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
			}
			//���ɐi�߂�
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//�����ăV�F�[�_�[���\�[�X
		for (int i = 0; i < m_numShaderResource; i++) {
			if (m_shaderResources[i] != nullptr) {
				m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
			}
			//���ɐi�߂�
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//������UAV
		for (int i = 0; i < m_numUavResource; i++) {
			if (m_uavResources[i] != nullptr) {
				m_uavResources[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
			}
			//���ɐi�߂�
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z
		m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
		//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z
		m_srGpuDescriptorStart[bufferNo] = gpuHandle;
		m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * m_numConstantBuffer;
		//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z
		m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
		m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer);

		gpuHandle.ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer + m_numUavResource);

		bufferNo++;
	}
}


