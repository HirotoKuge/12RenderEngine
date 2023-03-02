/*****************************************************************//**
 * \file   DescriptorHeap.cpp
 * \brief  �f�B�X�N���v�^�q�[�v�N���X
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
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
	ID3D12Device5* pDevice = GraphicsEngine::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numSamplerDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit �f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂����B", L"�G���[", MB_OK);
			std::abort();
		}
	}
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int i = 0; i < m_numSamplerDesc; i++) {
			//�T���v���X�e�[�g���f�B�X�N���v�^�q�[�v�ɓo�^���Ă����B
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
	//�萔�o�b�t�@��V�F�[�_�[���\�[�X�̃f�B�X�N���v�^���q�[�v�ɏ�������ł����B
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		//�萔�o�b�t�@��o�^���Ă����B
		for (int i = 0; i < m_numConstantBuffer; i++) {
			//@todo bug
			if (m_constantBuffers[i] != nullptr) {
				m_constantBuffers[i]->RegistConstantBufferView(cpuHandle,bufferNo);
			}
			//���ɐi�߂�B
			cpuHandle.ptr += m_cbrSrvDescriptorSize;
		}

		//�����ăV�F�[�_�[���\�[�X�B
		for (int i = 0; i < m_numShaderResource; i++) {
			if (m_shaderResources[i] != nullptr) {
				m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
			}
			//���ɐi�߂�B
			cpuHandle.ptr += m_cbrSrvDescriptorSize;
		}

		//������UAV�B
		for (int i = 0; i < m_numUavResource; i++) {
			if (m_uavResources[i] != nullptr) {
				m_uavResources[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
			}
			//���ɐi�߂�B
			cpuHandle.ptr += m_cbrSrvDescriptorSize;
		}

		//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
		m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
		//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
		m_srGpuDescriptorStart[bufferNo] = gpuHandle;
		m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)m_cbrSrvDescriptorSize * m_numConstantBuffer;
		//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h�����v�Z�B
		m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
		m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)m_cbrSrvDescriptorSize * (m_numShaderResource + m_numConstantBuffer);

		gpuHandle.ptr += (UINT64)m_cbrSrvDescriptorSize * (m_numShaderResource + m_numConstantBuffer + m_numUavResource);

		bufferNo++;
	}
}


/*****************************************************************//**
 * \brief �f�B�X�N���v�^�q�[�v���擾
 *
 * \return �f�B�X�N���v�^�q�[�v
 *********************************************************************/
 ID3D12DescriptorHeap* DescriptorHeap::Get()const {
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_descriptorHeap[backBufferIndex];
}

/*****************************************************************//**
 * \brief �萔�o�b�t�@�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �萔�o�b�t�@�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
 D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetConstantBufferGpuDescriptorStartHandle() const {
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_cbGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief �V�F�[�_�[���\�[�X(�e�N�X�`��)�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �V�F�[�_�[���\�[�X(�e�N�X�`��)�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
 D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetShaderResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_srGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief UAV���\�[�X�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return UAV���\�[�X�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
 D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetUavResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_uavGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief �T���v���[�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �T���v���[�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
 D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetSamplerResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_samplerGpuDescriptorStart[backBufferIndex];
}

#pragma endregio