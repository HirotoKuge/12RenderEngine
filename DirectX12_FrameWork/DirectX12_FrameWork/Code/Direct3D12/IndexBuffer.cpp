/*****************************************************************//**
 * \file   IndexBuffer.cpp
 * \brief  インデックスバッファクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes.
//=============================================================================
#include "../stdafx.h"
#include "IndexBuffer.h"

//-----------------------------------------------------------------------------
// コンストラクタ 
//-----------------------------------------------------------------------------
IndexBuffer::IndexBuffer()
	:m_pIB(nullptr)
{ memset(&m_view, 0, sizeof(m_view)); }

//=============================================================================
// デストラクタ
//=============================================================================
IndexBuffer::~IndexBuffer(){ 
	Term(); 
	if (m_pIndexBuffer) {
		m_pIndexBuffer->Release();
	}
}



//=============================================================================
// 初期化処理
//=============================================================================
void IndexBuffer::Init(uint32_t size, uint32_t stride){
	
	if (stride == 2) {
		m_sizeInBytes = size * 2;
	}
	else {
		m_sizeInBytes = size;
	}
	
	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_sizeInBytes);
	auto hr = pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pIndexBuffer.GetAddressOf()));

	//インデックスバッファのビューを作成
	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();


	//ストライドは４バイト固定
	m_strideInBytes = 4;
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = m_sizeInBytes;

	// インデックス数を計算
	m_count = m_sizeInBytes / m_strideInBytes;
}

//=============================================================================
// コピー
//=============================================================================
void IndexBuffer::Copy(uint16_t* srcIndecies){
	uint32_t* pData;
	m_pIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	for (int i = 0; i < m_count; i++) {
		pData[i] = srcIndecies[i];
	}
	m_pIndexBuffer->Unmap(0, nullptr);
}
void IndexBuffer::Copy(uint32_t* srcIndecies){
	uint32_t* pData;
	m_pIndexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pData));
	for (int i = 0; i < m_count; i++) {
		pData[i] = srcIndecies[i];
	}
	m_pIndexBuffer->Unmap(0, nullptr);
}


//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool IndexBuffer::Init(ID3D12Device* pDevice, size_t size, const uint32_t* pInitData) {
	// ヒーププロパティ.
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;

	// リソースの設定.
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = UINT64(size);
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	// リソースを生成.
	auto hr = pDevice->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_pIB.GetAddressOf()));
	if (FAILED(hr))
	{ return false; }

	// インデックスバッファビューの設定.
	m_indexBufferView.BufferLocation = m_pIB->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = UINT(size);

	// 初期化データがあれば，書き込んでおく.
	if (pInitData != nullptr){
		void* ptr = Map();
		if (ptr == nullptr)
		{ return false;}

		memcpy(ptr, pInitData, size);

		m_pIB->Unmap(0, nullptr);
	}

	// 正常終了.
	return true;
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void IndexBuffer::Term() {
	m_pIB.Reset();
	memset(&m_view, 0, sizeof(m_view));
}

//-----------------------------------------------------------------------------
// メモリマッピング
//-----------------------------------------------------------------------------
uint32_t* IndexBuffer::Map() {
	uint32_t* ptr;
	auto hr = m_pIB->Map(0, nullptr, reinterpret_cast<void**>(&ptr));
	if (FAILED(hr))
	{ return nullptr; }

	return ptr;
}

//-----------------------------------------------------------------------------
//	  メモリマッピングを解除します.
//-----------------------------------------------------------------------------
void IndexBuffer::Unmap()
{ m_pIB->Unmap(0, nullptr); }
