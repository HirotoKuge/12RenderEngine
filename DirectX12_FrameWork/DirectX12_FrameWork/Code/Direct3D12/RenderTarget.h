/*****************************************************************//**
 * \file   RenderTarget.h
 * \brief  �����_�[�^�[�Q�b�g�N���X(MRT�p)
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//=============================================================================
// Includes.
//=============================================================================
#include "Texture.h"

class GraphicsEngine;

//=============================================================================
// RenderTarget Classa
//=============================================================================
class RenderTarget{
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
	//-----------------------------------------------------------------------------	
	//!{
			/* Nothing */
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing*/
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~RenderTarget();

	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�̍쐬
	 *  
	 * \param width					�����_�����O�^�[�Q�b�g�̕�
	 * \param height				�����_�����O�^�[�Q�b�g�̍���
	 * \param mipLevel				�~�b�v�}�b�v���x���F0���w�肵���ꍇ�́A�T�|�[�g����Ă���GPU�̏ꍇ1*1�s�N�Z���܂ł̃~�b�v�}�b�v���쐬�����
	 * \param arraySize				�e�N�X�`���z��T�C�Y
	 * \param colorFormat			�J���[�o�b�t�@�̃t�H�[�}�b�g
	 * \param depthStencilFormat	�[�x�X�e���V���o�b�t�@�̃t�H�[�}�b�g
	 * \param clearColor			�J���[�o�b�t�@�̃N���A�J���[(�w�肵�Ȃ���΃f�t�H���g)
	 * \return true	 �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool Create(
		uint32_t width,
		uint32_t height,
		uint32_t mipLevel,
		uint32_t arraySize,
		DXGI_FORMAT colorFormat,
		DXGI_FORMAT depthStencilFormat,
		float clearColor[4] = nullptr
	);
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g��CPU�f�B�X�N���v�^�n���h�����擾
	 *  
	 * \return 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuDescriptorHandle() const{
		return m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	}
	
	/*****************************************************************//**
	 * \brief �k�x�X�e���V���o�b�t�@��CPU�f�B�X�N���v�^�n���h�����擾
	 *  
	 * \return 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuDescriptorHandle() const{
		return m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	}
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g(Texture�N���X)���擾
	 *  
	 * \return 
	 *********************************************************************/
	Texture& GetRenderTargetTexture(){
		return m_rtTexture;
	}
	
	/*****************************************************************//**
	 * \brief �[�x�X�e���V���o�b�t�@�����݂��邩�m�F
	 *  
	 * \return 
	 *********************************************************************/
	bool IsExsitDepthStencilBuffer() const{
		return m_pDSTexture;
	}
	
	/*****************************************************************//**
	 * \brief �J���[�o�b�t�@�̃t�H�[�}�b�g���擾
	 *  
	 * \return 
	 *********************************************************************/
	DXGI_FORMAT GetColorBufferFormat() const{
		return m_rtTexture.GetFormat();
	}

	int GetWidth() const{
		return m_width;
	}
	int GetHeight() const{
		return m_height;
	}
	const float* GetRTVClearColor() const{
		return m_rtvClearColor;
	}
	float GetDSVClearValue() const{
		return m_dsvClearValue;
	}



	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	Texture m_rtTexture;

	ComPtr<ID3D12Resource>		 m_pRTTextureDx12;	// �����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`��
	ComPtr<ID3D12Resource>		 m_pDSTexture;		// �[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`��
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;
	
	uint32_t m_rtvDescriptorSize = 0; //�t���[���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y
	uint32_t m_dsvDescriptorSize = 0; //�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y

	uint32_t m_width = 0;
	uint32_t m_height = 0;

	float m_rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// �����_�����O�^�[�Q�b�g�r���[�̃N���A�J���[
	float m_dsvClearValue = 1.0f;							// �[�x�X�e���V���r���[�̃N���A�J���[
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{

	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�q�[�v���쐬
	 *  
	 * \param pDevice �f�o�C�X
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateDescriptorHeap(ID3D12Device5*& pDevice);
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�ɂȂ�e�N�X�`�����쐬
	 *  
	 * \param pDevice	 �f�o�C�X
	 * \param width		 �e�N�X�`���̕�	
	 * \param height	 �e�N�X�`���̍���
	 * \param mipLevel	 �~�b�v�}�b�v���x��
	 * \param arraySize	 �e�N�X�`���̔z��T�C�Y
	 * \param format	 �e�N�X�`���t�H�[�}�b�g
	 * \param clearColor �N���A�J���[
	 * \return true	 �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateRenderTargetTexture(
		ID3D12Device5*& pDevice,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevel,
		uint32_t arraySize,
		DXGI_FORMAT format,
		float clearColor[4]
	);
	
	/*****************************************************************//**
	 * \brief �[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`�����쐬
	 *  
	 * \param pDevice	�f�o�C�X
	 * \param width		�e�N�X�`���̕�
	 * \param height	�e�N�X�`���̍���
	 * \param format	�e�N�X�`���t�H�[�}�b�g	
	 * \return true	 �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateDepthStencilTexture(
		ID3D12Device5*& pDevice,
		uint32_t width, 
		uint32_t height,
		DXGI_FORMAT format);
	
	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�̍쐬
	 *  
	 * \param pDevice �f�o�C�X
	 * \return true	 �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	void CreateDescriptor(ID3D12Device5*& pDevice);

	//!} 
};

