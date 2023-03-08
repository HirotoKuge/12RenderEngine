/*****************************************************************//**
 * \file   GPUBuffer.h
 * \brief  GPUBufferクラス
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once

//=============================================================================
// GPUBuffer Class
//=============================================================================
class GPUBuffer : public UnorderAccessResrouce {
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
			/* Nothing */
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~GPUBuffer(){
		if (m_buffer != nullptr) {
			m_buffer->Release();
		}
	}

	/*****************************************************************//**
	 * \brief 初期化
	 * 
	 * \param desc リソース構成
	 *********************************************************************/
	void Init(D3D12_RESOURCE_DESC desc);
	
	/*****************************************************************//**
	 * \brief UAVとしてディスクリプタヒープに登録
	 * 
	 * \param cpuHandle 登録するCPUディスクリプタハンドル
	 * \param bufferNo	バッファ番号
	 *********************************************************************/
	void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, int bufferNo) override;
	
	/*****************************************************************//**
	 * \brief SRBとしてディスクリプタヒープに登録
	 * 
	 * \param cpuHandle 登録するCPUディスクリプタハンドル
	 * \param bufferNo	バッファ番号
	 *********************************************************************/
	void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) override;

	/*****************************************************************//**
	 * \brief リソースを取得
	 * 
	 * \return リソース
	 *********************************************************************/
	ID3D12Resource* Get(){
		return m_buffer;
	}
	
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	ID3D12Resource* m_buffer = nullptr;
	D3D12_RESOURCE_DESC m_desc;
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
			/* Nothing*/
	//!} 
};