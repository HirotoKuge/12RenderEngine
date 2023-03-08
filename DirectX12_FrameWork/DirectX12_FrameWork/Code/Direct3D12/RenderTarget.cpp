/*****************************************************************//**
 * \file   RenderTarget.cpp
 * \brief  レンダーターゲットクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#include "../stdafx.h"
#include "RenderTarget.h"
#include "GraphicsEngine.h"

//=============================================================================
// デストラクタ
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
// 作成
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
	//レンダリングターゲットとなるテクスチャを作成
	if (!CreateRenderTargetTexture(pDevice, width, height, mipLevel, arraySize, colorFormat, clearColor)) {
		MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗", "エラー", MB_OK);
		return false;
	}
	//深度ステンシルバッファとなるテクスチャを作成
	if (depthStencilFormat != DXGI_FORMAT_UNKNOWN) {
		if (!CreateDepthStencilTexture(pDevice, width, height, depthStencilFormat)) {
			MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗", "エラー", MB_OK);
			return false;
		}
	}
	if (!CreateDescriptorHeap(pDevice)) {
		//ディスクリプタヒープの作成に失敗
		MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗", "エラー", MB_OK);
		return false;
	}
	//ディスクリプタを作成する
	CreateDescriptor(pDevice);

	// クリアカラー
	if (clearColor) {
		memcpy(m_rtvClearColor, clearColor, sizeof(m_rtvClearColor));
	}
	return true;
	
}

//=============================================================================
// ディスクリプタヒープを作成
//=============================================================================
bool RenderTarget::CreateDescriptorHeap(ID3D12Device5*& pDevice){
	//RTV用のディスクリプタヒープを作成する
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = GraphicsEngine::FRAME_BUFFER_COUNT;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRTVHeap));
	if (m_pRTVHeap == nullptr) {
		//RTV用のディスクリプタヒープの作成に失敗
		return false;
	}

	//ディスクリプタのサイズを取得
	m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (m_pDSTexture) {
		//DSV用のディスクリプタヒープを作成
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDSVHeap));
		if (m_pDSVHeap == nullptr) {
			//DSV用のディスクリプタヒープの作成に失敗
			return false;
		}
		//ディスクリプタのサイズを取得
		m_dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	}
	return true;
}

//=============================================================================
// レンダリングターゲットになるテクスチャを作成
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
	//リソースを作成
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
		//作成に失敗
		return false;
	}

	m_rtTexture.InitFromD3DResource(m_pRTTextureDx12);
	
	return true;

}

//=============================================================================
// 深度ステンシルバッファとなるテクスチャを作成
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
		//深度ステンシルバッファの作成に失敗
		return false;
	}
	return true;
}

//=============================================================================
// ディスクリプタを作成
//=============================================================================
void RenderTarget::CreateDescriptor(ID3D12Device5*& pDevice){
	//カラーテクスチャのディスクリプタを作成
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();

	pDevice->CreateRenderTargetView(m_rtTexture.Get(), nullptr, rtvHandle);
	if (m_pDSTexture) {
		//深度テクスチャのディスクリプタを作成
		auto dsvHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
		pDevice->CreateDepthStencilView(m_pDSTexture, nullptr, dsvHandle);
	}

}





