/*****************************************************************//**
 * \file   RenderTarget.cpp
 * \brief  �����_�[�^�[�Q�b�g�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#include "../stdafx.h"
#include "RenderTarget.h"
#include "GraphicsEngine.h"

//=============================================================================
// �f�X�g���N�^
//=============================================================================
RenderTarget::~RenderTarget() {
	if (m_pRTTextureDx12) {
		m_pRTTextureDx12->Release();
	}
	if (m_pDSTexture) {
		m_pDSTexture->Release();
	}
	if (m_pRTVHeap) {
		m_pRTVHeap->Release();
	}
	if (m_pDSVHeap) {
		m_pDSVHeap->Release();
	}
}
//=============================================================================
// �쐬
//=============================================================================
bool RenderTarget::Create(
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevel, 
	uint32_t arraySize, 
	DXGI_FORMAT colorFormat, 
	DXGI_FORMAT depthStencilFormat, 
	float clearColor[4]
){
	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	
	m_width = width;
	m_height = height;
	//�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`�����쐬
	if (!CreateRenderTargetTexture(pDevice, width, height, mipLevel, arraySize, colorFormat, clearColor)) {
		MessageBoxA(nullptr, "�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���̍쐬�Ɏ��s", "�G���[", MB_OK);
		return false;
	}
	//�[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`�����쐬
	if (depthStencilFormat != DXGI_FORMAT_UNKNOWN) {
		if (!CreateDepthStencilTexture(pDevice, width, height, depthStencilFormat)) {
			MessageBoxA(nullptr, "�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���̍쐬�Ɏ��s", "�G���[", MB_OK);
			return false;
		}
	}
	if (!CreateDescriptorHeap(pDevice)) {
		//�f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s
		MessageBoxA(nullptr, "�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���̍쐬�Ɏ��s", "�G���[", MB_OK);
		return false;
	}
	//�f�B�X�N���v�^���쐬����
	CreateDescriptor(pDevice);

	// �N���A�J���[
	if (clearColor) {
		memcpy(m_rtvClearColor, clearColor, sizeof(m_rtvClearColor));
	}
	return true;
	
}

//=============================================================================
// �f�B�X�N���v�^�q�[�v���쐬
//=============================================================================
bool RenderTarget::CreateDescriptorHeap(ID3D12Device5*& pDevice){
	//RTV�p�̃f�B�X�N���v�^�q�[�v���쐬����
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = GraphicsEngine::FRAME_BUFFER_COUNT;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRTVHeap));
	if (m_pRTVHeap == nullptr) {
		//RTV�p�̃f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s
		return false;
	}

	//�f�B�X�N���v�^�̃T�C�Y���擾
	m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (m_pDSTexture) {
		//DSV�p�̃f�B�X�N���v�^�q�[�v���쐬
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDSVHeap));
		if (m_pDSVHeap == nullptr) {
			//DSV�p�̃f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s
			return false;
		}
		//�f�B�X�N���v�^�̃T�C�Y���擾
		m_dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	return true;
}

//=============================================================================
// �����_�����O�^�[�Q�b�g�ɂȂ�e�N�X�`�����쐬
//=============================================================================
bool RenderTarget::CreateRenderTargetTexture(
	ID3D12Device5*& pDevice, 
	uint32_t width, 
	uint32_t height, 
	uint32_t mipLevel, 
	uint32_t arraySize, 
	DXGI_FORMAT format, 
	float clearColor[4]
){
	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		arraySize,
		mipLevel,
		format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	if (clearColor != nullptr) {
		clearValue.Color[0] = clearColor[0];
		clearValue.Color[1] = clearColor[1];
		clearValue.Color[2] = clearColor[2];
		clearValue.Color[3] = clearColor[3];
	}
	else {
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;
	}
	//���\�[�X���쐬
	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COMMON,
		&clearValue,
		IID_PPV_ARGS(&m_pRTTextureDx12)
	);

	if (FAILED(hr)) {
		//�쐬�Ɏ��s
		return false;
	}

	m_rtTexture.InitFromD3DResource(m_pRTTextureDx12);
	
	return true;

}

//=============================================================================
// �[�x�X�e���V���o�b�t�@�ƂȂ�e�N�X�`�����쐬
//=============================================================================
bool RenderTarget::CreateDepthStencilTexture(ID3D12Device5*& pDevice, uint32_t width, uint32_t height, DXGI_FORMAT format){
	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = format;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		static_cast<UINT>(width),
		static_cast<UINT>(height),
		1,
		1,
		format,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

	auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(&m_pDSTexture)
	);
	if (FAILED(hr)) {
		//�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s
		return false;
	}
	return true;
}

//=============================================================================
// �f�B�X�N���v�^���쐬
//=============================================================================
void RenderTarget::CreateDescriptor(ID3D12Device5*& pDevice){
	//�J���[�e�N�X�`���̃f�B�X�N���v�^���쐬
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();

	pDevice->CreateRenderTargetView(m_rtTexture.Get(), nullptr, rtvHandle);
	if (m_pDSTexture) {
		//�[�x�e�N�X�`���̃f�B�X�N���v�^���쐬
		auto dsvHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
		pDevice->CreateDepthStencilView(m_pDSTexture, nullptr, dsvHandle);
	}

}





