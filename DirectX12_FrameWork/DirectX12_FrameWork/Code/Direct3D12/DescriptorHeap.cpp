/*****************************************************************//**
 * \file   DescriptorHeap.cpp
 * \brief  ディスクリプタヒープクラス
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#include "../stdafx.h"
#include "DescriptorHeap.h"


//=============================================================================
// コンストラクタ
//=============================================================================
DescriptorHeap::DescriptorHeap(){
	m_shaderResources.resize(MAX_SHADER_RESOURCE);
	m_uavResources.resize(MAX_SHADER_RESOURCE);
	m_constantBuffers.resize(MAX_CONSTANT_BUFFER);
	for (auto& srv : m_shaderResources) {
		srv = nullptr;
	}
	for (auto& uav : m_uavResources) {
		uav = nullptr;
	}
	for (auto& cbv : m_constantBuffers) {
		cbv = nullptr;
	}
}

//=============================================================================
// デストラクタ
//=============================================================================
DescriptorHeap::~DescriptorHeap(){
	for (auto& ds : m_descriptorHeap) {
		if (ds) {
			ds->Release();
		}
	}
}

//=============================================================================
// サンプラー
//=============================================================================
void DescriptorHeap::CommitSamplerHeap(){
	ID3D12Device5* pDevice =GraphicsEngine::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numSamplerDesc;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました", L"エラー", MB_OK);
			std::abort();
		}
	}
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		for (int i = 0; i < m_numSamplerDesc; i++) {
			//サンプラステートをディスクリプタヒープに登録していく
			pDevice->CreateSampler(&m_samplerDescs[i], cpuHandle);
			cpuHandle.ptr += m_samplerDescriptorSize;
		}
		m_samplerGpuDescriptorStart[bufferNo] = gpuHandle;
		bufferNo++;
	}

}

int g_numDescriptorHeap = 0;


//=============================================================================
// ヒープへの登録を確定
//=============================================================================
void DescriptorHeap::Commit(){
	ID3D12Device5* pDevice = GraphicsEngine::GetInstance()->GetDevice();
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	srvHeapDesc.NumDescriptors = m_numShaderResource + m_numConstantBuffer + m_numUavResource;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	for (auto& ds : m_descriptorHeap) {
		auto hr = pDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&ds));
		g_numDescriptorHeap++;
		if (FAILED(hr)) {
			MessageBox(nullptr, L"DescriptorHeap::Commit ディスクリプタヒープの作成に失敗しました", L"エラー", MB_OK);
			std::abort();
		}
	}
	//定数バッファやシェーダーリソースのディスクリプタをヒープに書き込んでいく
	int bufferNo = 0;
	for (auto& descriptorHeap : m_descriptorHeap) {
		auto cpuHandle = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		auto gpuHandle = descriptorHeap->GetGPUDescriptorHandleForHeapStart();

		//定数バッファを登録していく
		for (int i = 0; i < m_numConstantBuffer; i++) {
			//@todo bug
			if (m_constantBuffers[i] != nullptr) {
				m_constantBuffers[i]->RegistConstantBufferView(cpuHandle, bufferNo);
			}
			//次に進める
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//続いてシェーダーリソース
		for (int i = 0; i < m_numShaderResource; i++) {
			if (m_shaderResources[i] != nullptr) {
				m_shaderResources[i]->RegistShaderResourceView(cpuHandle, bufferNo);
			}
			//次に進める
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//続いてUAV
		for (int i = 0; i < m_numUavResource; i++) {
			if (m_uavResources[i] != nullptr) {
				m_uavResources[i]->RegistUnorderAccessView(cpuHandle, bufferNo);
			}
			//次に進める
			cpuHandle.ptr += GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize();
		}

		//定数バッファのディスクリプタヒープの開始ハンドルを計算
		m_cbGpuDescriptorStart[bufferNo] = gpuHandle;
		//シェーダーリソースのディスクリプタヒープの開始ハンドルを計算
		m_srGpuDescriptorStart[bufferNo] = gpuHandle;
		m_srGpuDescriptorStart[bufferNo].ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * m_numConstantBuffer;
		//UAVリソースのディスクリプタヒープの開始ハンドルを計算
		m_uavGpuDescriptorStart[bufferNo] = gpuHandle;
		m_uavGpuDescriptorStart[bufferNo].ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer);

		gpuHandle.ptr += (UINT64)GraphicsEngine::GetInstance()->GetCbrSrvDescriptorSize() * (m_numShaderResource + m_numConstantBuffer + m_numUavResource);

		bufferNo++;
	}
}


