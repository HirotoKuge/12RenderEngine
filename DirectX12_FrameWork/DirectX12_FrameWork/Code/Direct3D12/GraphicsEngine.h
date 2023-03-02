/*****************************************************************//**
 * \file   GraphicsEngine.h
 * \brief  DirectX12�̕`��G���W��
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include "../../DirectXTK/Inc/DDSTextureLoader.h"
#include "../../DirectXTK/Inc/ResourceUploadBatch.h"

#include "ComPtr.h"
#include "RenderTarget.h"
#include "Sprite.h"
#include "RenderContext.h"
#include "Camera.h"


//-----------------------------------------------------------------------------
// Forword Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment( lib, "d3d12.lib") 
#pragma comment( lib, "dxgi.lib") 
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

const UINT FRAME_BUFFER_W = 1280;	//�t���[���o�b�t�@�̕��B
const UINT FRAME_BUFFER_H = 720;	//�t���[���o�b�t�@�̍����B



//=============================================================================
// GraphicsEngine Class
//=============================================================================
class GraphicsEngine {
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
	
	enum { FRAME_BUFFER_COUNT = 2 };	//�t���[���o�b�t�@�̐�

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief �C���X�^���X���擾
	 * 
	 * \return	�C���X�^���X�A�h���X
	 *********************************************************************/
	static GraphicsEngine* GetInstance();
	
	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~GraphicsEngine();
	
	/*****************************************************************//**
	 * \brief ������
	 *  
	 * \param hwnd				�E�B���h�E�n���h��
	 * \param frameBufferWidth	�t���[���o�b�t�@�̕�
	 * \param frameBufferHeight	�t���[���o�b�t�@�̍���
	 * \return true	 �������ɐ���
	 * \return false �������Ɏ��s
	 *********************************************************************/
	bool Init(HWND hwnd, uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	
	/*****************************************************************//**
	 * \brief �����_�����O�J�n
	 *********************************************************************/
	void BeginRender();
	
	/*****************************************************************//**
	 * \brief �����_�����O�I��
	 *********************************************************************/
	void EndRender();
	
	/*****************************************************************//**
	 * \brief D3D�f�o�C�X�̎擾
	 *  
	 * \return �f�o�C�X
	 *********************************************************************/
	ID3D12Device5* GetDevice(){
		return m_pDevice;
	}

	/*****************************************************************//**
	 * \brief �o�b�N�o�b�t�@�ԍ����擾
	 *  
	 * \return �o�b�N�o�b�t�@�ԍ�
	 *********************************************************************/
	uint32_t GetBackBufferIndex() const{
		return m_frameIndex;
	}
	
	/*****************************************************************//**
	 * \brief �R�}���h�L���[���擾
	 *  
	 * \return �R�}���h�L���[
	 *********************************************************************/
	ID3D12CommandQueue* GetCommandQueue() const{
		return m_pCommandQueue;
	}

	/*****************************************************************//**
	 * \brief �R�}���h���X�g���擾
	 *  
	 * \return �R�}���h���X�g
	 *********************************************************************/
	ID3D12GraphicsCommandList4* GetCommandList() const{
		return m_pCommandList;
	}
	
	/*****************************************************************//**
	 * \brief CBR_SRV�̃f�B�X�N���v�^�T�C�Y���擾
	 *  
	 * \return �f�B�X�N���v�^�T�C�Y
	 *********************************************************************/
	uint32_t GetCbrSrvDescriptorSize() const{
		return m_cbrSrvDescriptorSize;
	}
	
	/*****************************************************************//**
	 * \brief �T���v���̃f�B�X�N���v�^�T�C�Y���擾
	 *  
	 * \return �f�B�X�N���v�^�T�C�Y
	 *********************************************************************/
	uint32_t GetSapmerDescriptorSize() const{
		return m_samplerDescriptorSize;
	}
	
	/*****************************************************************//**
	 * \brief �����_�����O�R���e�L�X�g���擾
	 *  
	 * \return �����_�����O�R���e�L�X�g
	 *********************************************************************/
	RenderContext& GetRenderContext(){
		return m_renderContext;
	}
	
	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�̕����擾
	 *  
	 * \return �t���[���o�b�t�@�̕�
	 *********************************************************************/
	uint32_t GetFrameBufferWidth() const{
		return m_frameBufferWidth;
	}
	
	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�̍������擾
	 *  
	 * \return �t���[���o�b�t�@�̍���
	 *********************************************************************/
	uint32_t GetFrameBufferHeight() const{
		return m_frameBufferHeight;
	}
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g���t���[���o�b�t�@�ɕύX
	 *  
	 * \param rc �����_�����O�R���e�L�X�g
	 *********************************************************************/
	void ChangeRenderTargetToFrameBuffer(RenderContext& rc);
	
	/*****************************************************************//**
	 * \brief ���݂̃t���[���o�b�t�@��RTV���擾
	 *  
	 * \return ���݂̃t���[���o�b�t�@��RTV 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerRTV() const{
		return m_currentFrameBufferRTVHandle;
	}
	
	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�̕`��Ɏg�p���Ă���DSV���擾
	 *  
	 * \return�@�t���[���o�b�t�@�̕`��Ɏg�p���Ă���DSV 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerDSV() const{
		return m_currentFrameBufferDSVHandle;
	}

	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�ɃR�s�[
	 *  
	 * \param rc	�����_�����O�R���e�L�X�g
	 * \param pSrc	�R�s�[��
	 *********************************************************************/
	void CopyToFrameBuffer(RenderContext& rc, ID3D12Resource* pSrc){
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST);
		rc.ResourceBarrier(barrier);
		rc.CopyResource(m_pRenderTargets[m_frameIndex], pSrc);

		D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		rc.ResourceBarrier(barrier2);
	}
	
	/*****************************************************************//**
	 * \brief �r���[�|�[�g���擾
	 *  
	 * \return �r���[�|�[�g
	 *********************************************************************/
	D3D12_VIEWPORT& GetFrameBufferViewport(){
		return m_viewport;
	}

	/*****************************************************************//**
	 * \brief 2D�J�����̃C���X�^���X�A�h���X���擾
	 * 
	 * \return 2D�J�����̃C���X�^���X�A�h���X
	 *********************************************************************/
	Camera* GetCamera2D() {
		return &m_camera2D;
	}

	/*****************************************************************//**
	 * \brief 3D�J�����̃C���X�^���X�A�h���X���擾
	 *
	 * \return 3D�J�����̃C���X�^���X�A�h���X
	 *********************************************************************/
	Camera* GetCamera3D() {
		return &m_camera3D;
	}
	
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{ 
	
	// �C���X�^���X�ł���[
	static GraphicsEngine* pInstance;
	
	//GPU�x���_�[��`
	enum GPU_Vender {
		GPU_VenderNvidia,	//NVIDIA
		GPU_VenderAMD,		//Intel
		GPU_VenderIntel,	//AMD
		Num_GPUVender,
	};

	ID3D12Device5* m_pDevice = nullptr;									//D3D�f�o�C�X
	ID3D12CommandQueue* m_pCommandQueue = nullptr;						//�R�}���h�L���[
	IDXGISwapChain3* m_pSwapChain = nullptr;							//�X���b�v�`�F�C��
	ID3D12DescriptorHeap* m_pRTVHeap = nullptr;							//RTV�̃f�B�X�N���v�^�q�[�v
	ID3D12DescriptorHeap* m_pDSVHeap = nullptr;							//DSV�̃f�B�X�N���v�^�q�[�v
	ID3D12CommandAllocator* m_pCommandAllocator = nullptr;				//�R�}���h�A���P�[�^
	ID3D12GraphicsCommandList4* m_pCommandList = nullptr;				//�R�}���h���X�g
	ID3D12PipelineState* m_pPipelineState = nullptr;					//�p�C�v���C���X�e�[�g
	uint32_t m_currentBackBufferIndex = 0;								//���݂̃o�b�N�o�b�t�@�̔ԍ�
	uint32_t m_rtvDescriptorSize = 0;									//�t���[���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y
	uint32_t m_dsvDescriptorSize = 0;									//�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�̃T�C�Y
	uint32_t m_cbrSrvDescriptorSize = 0;								//CBR_SRV�̃f�B�X�N���v�^�̃T�C�Y
	uint32_t m_samplerDescriptorSize = 0;								//�T���v���̃f�B�X�N���v�^�̃T�C�Y			
	ID3D12Resource* m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//�t���[���o�b�t�@�p�̃����_�����O�^�[�Q�b�g
	ID3D12Resource* m_pDepthStencilBuffer = nullptr;					//�[�x�X�e���V���o�b�t�@
	D3D12_VIEWPORT m_viewport;											//�r���[�|�[�g
	D3D12_RECT m_scissorRect;											//�V�U�����O��`
	RenderContext m_renderContext;										//�����_�����O�R���e�L�X�g
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;			//���ݏ������ݒ��̃t���[���o�b�t�@��RTV�̃n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;			//���ݏ������ݒ��̃t���[���o�b�t�@��DSV�̃n���h��
	
	
	// GPU�Ƃ̓����Ŏg�p����ϐ�
	uint32_t m_frameIndex = 0;
	HANDLE m_fenceEvent = nullptr;
	ID3D12Fence* m_pFence = nullptr;
	UINT64 m_fenceValue = 0;
	uint32_t m_frameBufferWidth = 0;	//�t���[���o�b�t�@�̕�
	uint32_t m_frameBufferHeight = 0;	//�t���[���o�b�t�@�̍���
	Camera m_camera2D;					//2D�J����
	Camera m_camera3D;					//3D�J����

	std::unique_ptr<DirectX::GraphicsMemory> m_directXTKGfxMemroy;	//DirectXTK�̃O���t�B�b�N�������V�X�e��
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	
	/*****************************************************************//**
	 * \brief �铽�R���X�g���N�^(1�ȏ�̃C���X�^���X�����̋֎~)�@
	 *********************************************************************/
	GraphicsEngine();
	
	/*****************************************************************//**
	 * \brief �f�o�C�X���쐬
	 *  
	 * \param dxgiFactory	DXGI�t�@�N�g���[
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateD3DDevice(IDXGIFactory4* dxgiFactory);
	
	/*****************************************************************//**
	 * \brief �R�}���h�L���[���쐬
	 *  
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateCommandQueue();
	
	/*****************************************************************//**
	 * \brief �X���b�v�`�F�C�����쐬
	 *  
	 * \param hwnd				�E�B���h�E�n���h��
	 * \param frameBufferWidth	�t���[���o�b�t�@�̕�
	 * \param frameBufferHeight �t���[���o�b�t�@�̍���
	 * \param dxgiFactory		DXGI�t�@�N�g���[
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s 
	 *********************************************************************/
	bool CreateSwapChain(
		HWND hwnd,
		uint32_t frameBufferWidth,
		uint32_t frameBufferHeight,
		IDXGIFactory4* dxgiFactory
	);
	
	/*****************************************************************//**
	 * \brief DXGI�t�@�N�g���[���쐬
	 *  
	 * \return true �t�@�N�g���[
	 * \memo �֗��ȓz
 	 *********************************************************************/
	IDXGIFactory4* CreateDXGIFactory();
	
	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�p�̃f�B�X�N���v�^�q�[�v���쐬
	 *  
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateDescriptorHeapForFrameBuffer();

	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�p��RTV���쐬
	 *  
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateRTVForFameBuffer();
	
	/*****************************************************************//**
	 * \brief �t���[���o�b�t�@�p��DSV���쐬
	 *  
	 * \param frameBufferWidth	�t���[���o�b�t�@�̕�
	 * \param frameBufferHeight	�t���[���o�b�t�@�̍���
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateDSVForFrameBuffer(uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	
	/*****************************************************************//**
	 * \brief �R�}���h���X�g���쐬
	 *  
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateCommandList();
	
	/*****************************************************************//**
	 * \brief GPU�Ƃ̓����p�I�u�W�F�N�g���쐬
	 *  
	 * \return true  �쐬�ɐ���
	 * \return false �쐬�Ɏ��s
	 *********************************************************************/
	bool CreateSynchronizationWithGPUObject();
	
	/*****************************************************************//**
	 * \brief �`��̊����҂�
	 *********************************************************************/
	void WaitDraw();
	
	//!} 
};