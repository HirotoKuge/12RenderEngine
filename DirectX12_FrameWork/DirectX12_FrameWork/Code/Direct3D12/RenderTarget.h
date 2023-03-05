/*****************************************************************//**
 * \file   RenderTarget.h
 * \brief  レンダーターゲットクラス(MRT用)
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//=============================================================================
// Includes.
//=============================================================================
#include "Texture.h"

class GraphicsEngine;

//=============================================================================
// RenderTarget Classa
//=============================================================================
class RenderTarget{
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
			/* Nothing*/
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{

	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~RenderTarget();

	/*****************************************************************//**
	 * \brief レンダリングターゲットの作成
	 *  
	 * \param width					レンダリングターゲットの幅
	 * \param height				レンダリングターゲットの高さ
	 * \param mipLevel				ミップマップレベル：0を指定した場合は、サポートされているGPUの場合1*1ピクセルまでのミップマップが作成される
	 * \param arraySize				テクスチャ配列サイズ
	 * \param colorFormat			カラーバッファのフォーマット
	 * \param depthStencilFormat	深度ステンシルバッファのフォーマット
	 * \param clearColor			カラーバッファのクリアカラー(指定しなければデフォルト)
	 * \return true	 作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool Create(
		uint32_t width,
		uint32_t height,
		uint32_t mipLevel,
		uint32_t arraySize,
		DXGI_FORMAT colorFormat,
		DXGI_FORMAT depthStencilFormat,
		float clearColor[4] = nullptr
	);
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットのCPUディスクリプタハンドルを取得
	 *  
	 * \return 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCpuDescriptorHandle() const{
		return m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	}
	
	/*****************************************************************//**
	 * \brief 震度ステンシルバッファのCPUディスクリプタハンドルを取得
	 *  
	 * \return 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCpuDescriptorHandle() const{
		return m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	}
	
	/*****************************************************************//**
	 * \brief レンダリングターゲット(Textureクラス)を取得
	 *  
	 * \return 
	 *********************************************************************/
	Texture& GetRenderTargetTexture(){
		return m_rtTexture;
	}
	
	/*****************************************************************//**
	 * \brief 深度ステンシルバッファが存在するか確認
	 *  
	 * \return 
	 *********************************************************************/
	bool IsExsitDepthStencilBuffer() const{
		return m_pDSTexture;
	}
	
	/*****************************************************************//**
	 * \brief カラーバッファのフォーマットを取得
	 *  
	 * \return 
	 *********************************************************************/
	DXGI_FORMAT GetColorBufferFormat() const{
		return m_rtTexture.GetFormat();
	}

	int GetWidth() const{
		return m_width;
	}
	int GetHeight() const{
		return m_height;
	}
	const float* GetRTVClearColor() const{
		return m_rtvClearColor;
	}
	float GetDSVClearValue() const{
		return m_dsvClearValue;
	}



	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	Texture m_rtTexture;

	ComPtr<ID3D12Resource>		 m_pRTTextureDx12;	// レンダリングターゲットとなるテクスチャ
	ComPtr<ID3D12Resource>		 m_pDSTexture;		// 深度ステンシルバッファとなるテクスチャ
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_pDSVHeap;
	
	uint32_t m_rtvDescriptorSize = 0; //フレームバッファのディスクリプタのサイズ
	uint32_t m_dsvDescriptorSize = 0; //深度ステンシルバッファのディスクリプタのサイズ

	uint32_t m_width = 0;
	uint32_t m_height = 0;

	float m_rtvClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// レンダリングターゲットビューのクリアカラー
	float m_dsvClearValue = 1.0f;							// 深度ステンシルビューのクリアカラー
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{

	/*****************************************************************//**
	 * \brief ディスクリプタヒープを作成
	 *  
	 * \param pDevice デバイス
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateDescriptorHeap(ID3D12Device5*& pDevice);
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットになるテクスチャを作成
	 *  
	 * \param pDevice	 デバイス
	 * \param width		 テクスチャの幅	
	 * \param height	 テクスチャの高さ
	 * \param mipLevel	 ミップマップレベル
	 * \param arraySize	 テクスチャの配列サイズ
	 * \param format	 テクスチャフォーマット
	 * \param clearColor クリアカラー
	 * \return true	 作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateRenderTargetTexture(
		ID3D12Device5*& pDevice,
		uint32_t width,
		uint32_t height,
		uint32_t mipLevel,
		uint32_t arraySize,
		DXGI_FORMAT format,
		float clearColor[4]
	);
	
	/*****************************************************************//**
	 * \brief 深度ステンシルバッファとなるテクスチャを作成
	 *  
	 * \param pDevice	デバイス
	 * \param width		テクスチャの幅
	 * \param height	テクスチャの高さ
	 * \param format	テクスチャフォーマット	
	 * \return true	 作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateDepthStencilTexture(
		ID3D12Device5*& pDevice,
		uint32_t width, 
		uint32_t height,
		DXGI_FORMAT format);
	
	/*****************************************************************//**
	 * \brief ディスクリプタの作成
	 *  
	 * \param pDevice デバイス
	 * \return true	 作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	void CreateDescriptor(ID3D12Device5*& pDevice);

	//!} 
};

