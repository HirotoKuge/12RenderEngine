/*****************************************************************//**
 * \file   DescriptorHeap_inline.h
 * \brief  DescriptorHeap Class inline�֐��̒�`�w�b�_�[
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#pragma once

/*****************************************************************//**
 * \brief �f�B�X�N���v�^�q�[�v���擾
 *
 * \return �f�B�X�N���v�^�q�[�v
 *********************************************************************/
inline ID3D12DescriptorHeap* DescriptorHeap::Get()const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_descriptorHeap[backBufferIndex];
}

/*****************************************************************//**
 * \brief �萔�o�b�t�@�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �萔�o�b�t�@�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetConstantBufferGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_cbGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief �V�F�[�_�[���\�[�X(�e�N�X�`��)�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �V�F�[�_�[���\�[�X(�e�N�X�`��)�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetShaderResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_srGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief UAV���\�[�X�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return UAV���\�[�X�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetUavResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_uavGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief �T���v���[�̐擪�f�B�X�N���v�^�n���h�����擾
 *
 * \return �T���v���[�̐擪�f�B�X�N���v�^�n���h��
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetSamplerResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_samplerGpuDescriptorStart[backBufferIndex];
}

