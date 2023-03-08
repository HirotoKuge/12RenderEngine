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


//=============================================================================
// デストラクタ
//=============================================================================
IndexBuffer::~IndexBuffer(){ 
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
		IID_PPV_ARGS(&m_pIndexBuffer)
	);

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
