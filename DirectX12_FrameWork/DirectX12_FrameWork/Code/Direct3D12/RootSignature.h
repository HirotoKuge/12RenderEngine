/*****************************************************************//**
 * \file   RootSignature.h
 * \brief  ���[�g�V�O�l�`���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <stdint.h>
#include "ComPtr.h"


class RootSignature{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief �R���X�g���N�^
	 *********************************************************************/
	RootSignature();

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~RootSignature();

	
	/*****************************************************************//**
	 * \brief ������
	 *  
	 * \param samplerFilter		 �T���v���[�t�B���^�[
	 * \param textureAdressModeU �e�N�X�`���̃A�h���b�V���O���[�h(U����)
	 * \param textureAdressModeV �e�N�X�`���̃A�h���b�V���O���[�h(V����)
	 * \param textureAdressModeW �e�N�X�`���̃A�h���b�V���O���[�h(W����)
	 * \param maxCbvDescriptor	 �萔�o�b�t�@�p�̃f�B�X�N���v�^�̍ő吔
	 * \param maxSrvDescriptor	 �V�F�[�_�[���\�[�X�p�̃f�B�X�N���v�^�̍ő吔
	 * \param maxUavDescritor	 UAV�p�̃f�B�X�N���v�^�̍ő吔
	 * \return true  �������ɐ���
	 * \return false �������Ɏ��s
	 *********************************************************************/
	bool Init(
		D3D12_FILTER samplerFilter,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW,
		uint32_t maxCbvDescriptor = 8,
		uint32_t maxSrvDescriptor = 32,
		uint32_t maxUavDescritor = 8
	);
	
	/*****************************************************************//**
	 * \brief ������
	 * 
	 * \param pSamplerDescArray �T���v���[�ݒ�z��̐퓬�A�h���X
	 * \param numSampler		�T���v���[�̐�
	 * \param maxCbvDescriptor	�萔�o�b�t�@�p�̃f�B�X�N���v�^�̍ő吔
	 * \param maxSrvDescriptor	�V�F�[�_�[���\�[�X�p�̃f�B�X�N���v�^�̍ő吔
	 * \param maxUavDescritor	UAV�p�̃f�B�X�N���v�^�̍ő吔
	 * \param offsetInDescriptorsFromTableStartCB	�q�[�v��̊J�n�ʒu�̃I�t�Z�b�g
	 * \param offsetInDescriptorsFromTableStartSRV	�q�[�v��̊J�n�ʒu�̃I�t�Z�b�g
	 * \param offsetInDescriptorsFromTableStartUAV	�q�[�v��̊J�n�ʒu�̃I�t�Z�b�g
	 * \return true  �������ɐ���
	 * \return false �������Ɏ��s
	 *********************************************************************/
	bool Init(
		D3D12_STATIC_SAMPLER_DESC* pSamplerDescArray,
		uint32_t numSampler,
		uint32_t maxCbvDescriptor = 8,
		uint32_t maxSrvDescriptor = 32,
		uint32_t maxUavDescritor = 8,
		uint32_t offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		uint32_t offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		uint32_t offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
	);

	/*****************************************************************//**
	 * \brief �I������
	 *********************************************************************/
	void Term();
		
	/*****************************************************************//**
	 * \brief ���[�g�V�O�l�`�����擾
	 *  
	 * \return ���[�g�V�O�l�`��
	 *********************************************************************/
	ID3D12RootSignature* Get();

	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	ComPtr<ID3D12RootSignature> m_pRootSignature;
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	

	RootSignature(const RootSignature&) = delete;		// �A�N�Z�X�֎~
	void operator = (const RootSignature&) = delete;	// �A�N�Z�X�֎~

	//!} 
};

