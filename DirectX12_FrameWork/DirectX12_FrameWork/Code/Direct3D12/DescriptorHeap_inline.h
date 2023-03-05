/*****************************************************************//**
 * \file   DescriptorHeap_inline.h
 * \brief  DescriptorHeap Class inline関数の定義ヘッダー
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 *********************************************************************/
#pragma once

/*****************************************************************//**
 * \brief ディスクリプタヒープを取得
 *
 * \return ディスクリプタヒープ
 *********************************************************************/
inline ID3D12DescriptorHeap* DescriptorHeap::Get()const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_descriptorHeap[backBufferIndex];
}

/*****************************************************************//**
 * \brief 定数バッファの先頭ディスクリプタハンドルを取得
 *
 * \return 定数バッファの先頭ディスクリプタハンドル
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetConstantBufferGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_cbGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief シェーダーリソース(テクスチャ)の先頭ディスクリプタハンドルを取得
 *
 * \return シェーダーリソース(テクスチャ)の先頭ディスクリプタハンドル
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetShaderResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_srGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief UAVリソースの先頭ディスクリプタハンドルを取得
 *
 * \return UAVリソースの先頭ディスクリプタハンドル
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetUavResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_uavGpuDescriptorStart[backBufferIndex];
}

/*****************************************************************//**
 * \brief サンプラーの先頭ディスクリプタハンドルを取得
 *
 * \return サンプラーの先頭ディスクリプタハンドル
 *********************************************************************/
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetSamplerResourceGpuDescriptorStartHandle() const {
	auto backBufferIndex =GraphicsEngine::GetInstance()->GetBackBufferIndex();
	return m_samplerGpuDescriptorStart[backBufferIndex];
}

