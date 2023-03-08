/*****************************************************************//**
 * \file   ConstantBuffer.cpp
 * \brief  定数バッファクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "ConstantBuffer.h"

//=============================================================================
// デストラクタ
//=============================================================================
ConstantBuffer::~ConstantBuffer()
{
	//アンマップ
	CD3DX12_RANGE readRange(0, 0);
	for (auto& cb : m_pConstantBuffer) {
		if (cb != nullptr) {
			cb->Unmap(0, &readRange);
			cb->Release();
		}
	}
}

//=============================================================================
// 初期化
//=============================================================================
void ConstantBuffer::Init(size_t size, void* srcData){

	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();

	if (size == 0){
		return;
	}

	// サイズ確保
	m_size = size;

	//定数バッファは256バイトアライメントが要求されるので、256の倍数に切り上げる
	size_t align = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
	m_allocSize = (size + align) & 0xFFFFFF00;
	
	//定数バッファの作成
	int bufferNo = 0;
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(m_allocSize);
	for (auto& cb : m_pConstantBuffer) {
		
		pDevice->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&rDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&cb)
		);

		//定数バッファをCPUからアクセス可能な仮想アドレス空間にマッピングする
		{
			CD3DX12_RANGE readRange(0, 0); // CPU上のこのリソースから読み取ることを意図している
			cb->Map(0, &readRange, reinterpret_cast<void**>(&m_pConstBufferCPU[bufferNo]));
		}

		// 初期化データがあれば設定
		if (srcData != nullptr) {
			memcpy(m_pConstBufferCPU[bufferNo], srcData, m_size);
		}
		bufferNo++;
	}

	//利用可能にする
	m_isValid = true;
}

//=============================================================================
// CBVに登録
//=============================================================================
void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle){
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	RegistConstantBufferView(descriptorHandle, backBufferIndex);
}
void ConstantBuffer::RegistConstantBufferView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo){
	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = m_pConstantBuffer[bufferNo]->GetGPUVirtualAddress();
	desc.SizeInBytes = m_allocSize;
	pDevice->CreateConstantBufferView(&desc, descriptorHandle);
}

//=============================================================================
// VRAM(GPU側)上の仮想アドレスを取得
//=============================================================================
D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress(){
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_pConstantBuffer[backBufferIndex]->GetGPUVirtualAddress();
}

//=============================================================================
// マッピング済のポインタを取得
//=============================================================================
void* ConstantBuffer::GetMappedPtr() const{
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_pConstBufferCPU[backBufferIndex];
}

//=============================================================================
// コピー
//=============================================================================
void ConstantBuffer::CopyToVRAM(void* data){
	auto backBufferIndex = GraphicsEngine::GetInstance()->GetBackBufferIndex();
	memcpy(m_pConstBufferCPU[backBufferIndex], data, m_size);
}
