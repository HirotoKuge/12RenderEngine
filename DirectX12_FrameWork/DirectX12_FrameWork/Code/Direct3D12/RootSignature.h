/*****************************************************************//**
 * \file   RootSignature.h
 * \brief  ルートシグネチャクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <stdint.h>
#include "ComPtr.h"


class RootSignature{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
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
	 * \brief コンストラクタ
	 *********************************************************************/
	RootSignature();

	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~RootSignature();

	
	/*****************************************************************//**
	 * \brief 初期化
	 *  
	 * \param samplerFilter		 サンプラーフィルター
	 * \param textureAdressModeU テクスチャのアドレッシングモード(U方向)
	 * \param textureAdressModeV テクスチャのアドレッシングモード(V方向)
	 * \param textureAdressModeW テクスチャのアドレッシングモード(W方向)
	 * \param maxCbvDescriptor	 定数バッファ用のディスクリプタの最大数
	 * \param maxSrvDescriptor	 シェーダーリソース用のディスクリプタの最大数
	 * \param maxUavDescritor	 UAV用のディスクリプタの最大数
	 * \return true  初期化に成功
	 * \return false 初期化に失敗
	 *********************************************************************/
	bool Init(
		D3D12_FILTER samplerFilter,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeU,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeV,
		D3D12_TEXTURE_ADDRESS_MODE textureAdressModeW,
		uint32_t maxCbvDescriptor = 8,
		uint32_t maxSrvDescriptor = 32,
		uint32_t maxUavDescritor = 8
	);
	
	/*****************************************************************//**
	 * \brief 初期化
	 * 
	 * \param pSamplerDescArray サンプラー設定配列の戦闘アドレス
	 * \param numSampler		サンプラーの数
	 * \param maxCbvDescriptor	定数バッファ用のディスクリプタの最大数
	 * \param maxSrvDescriptor	シェーダーリソース用のディスクリプタの最大数
	 * \param maxUavDescritor	UAV用のディスクリプタの最大数
	 * \param offsetInDescriptorsFromTableStartCB	ヒープ上の開始位置のオフセット
	 * \param offsetInDescriptorsFromTableStartSRV	ヒープ上の開始位置のオフセット
	 * \param offsetInDescriptorsFromTableStartUAV	ヒープ上の開始位置のオフセット
	 * \return true  初期化に成功
	 * \return false 初期化に失敗
	 *********************************************************************/
	bool Init(
		D3D12_STATIC_SAMPLER_DESC* pSamplerDescArray,
		uint32_t numSampler,
		uint32_t maxCbvDescriptor = 8,
		uint32_t maxSrvDescriptor = 32,
		uint32_t maxUavDescritor = 8,
		uint32_t offsetInDescriptorsFromTableStartCB = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		uint32_t offsetInDescriptorsFromTableStartSRV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND,
		uint32_t offsetInDescriptorsFromTableStartUAV = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
	);

	/*****************************************************************//**
	 * \brief 終了処理
	 *********************************************************************/
	void Term();
		
	/*****************************************************************//**
	 * \brief ルートシグネチャを取得
	 *  
	 * \return ルートシグネチャ
	 *********************************************************************/
	ID3D12RootSignature* Get();

	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	ComPtr<ID3D12RootSignature> m_pRootSignature;
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	

	RootSignature(const RootSignature&) = delete;		// アクセス禁止
	void operator = (const RootSignature&) = delete;	// アクセス禁止

	//!} 
};

