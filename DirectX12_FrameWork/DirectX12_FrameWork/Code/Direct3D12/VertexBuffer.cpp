/*****************************************************************//**
 * \file   VertexBuffer.cpp
 * \brief  頂点バッファクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "VertexBuffer.h"

//=============================================================================
// デストラクタ
//=============================================================================
VertexBuffer::~VertexBuffer(){
	if (m_pVertexBuffer) {
		m_pVertexBuffer->Release();
	}
}

//=============================================================================
// 初期化処理
//=============================================================================
void VertexBuffer::Init(uint32_t size, uint32_t stride){
	
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(size);
	auto hr = pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_pVertexBuffer)
	);

	m_pVertexBuffer->SetName(L"VertexBuffer");

	//頂点バッファのビューを作成
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = size;
	m_vertexBufferView.StrideInBytes = stride;
}

//=============================================================================
// コピー
//=============================================================================
void VertexBuffer::Copy(void* srcVertices){
	uint8_t* pData;
	m_pVertexBuffer->Map(0, nullptr, (void**)&pData);
	memcpy(pData, srcVertices, m_vertexBufferView.SizeInBytes);
	m_pVertexBuffer->Unmap(0, nullptr);
}