/*****************************************************************//**
 * \file   GraphicsEngine.h
 * \brief  DirectX12の描画エンジン
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include <d3d12.h>
#include <d3dx12.h>
#include <DirectXTex.h>
#include <dxgi.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <dxgi1_3.h>
#include <dxgi1_4.h>
#include "../../DirectXTK/Inc/DDSTextureLoader.h"
#include "../../DirectXTK/Inc/ResourceUploadBatch.h"

#include "ComPtr.h"
#include "RenderTarget.h"
#include "Sprite.h"
#include "RenderContext.h"
#include "Camera.h"


//-----------------------------------------------------------------------------
// Forword Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Linker
//-----------------------------------------------------------------------------
#pragma comment( lib, "d3d12.lib") 
#pragma comment( lib, "dxgi.lib") 
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )

const UINT FRAME_BUFFER_W = 1280;	//フレームバッファの幅。
const UINT FRAME_BUFFER_H = 720;	//フレームバッファの高さ。



//=============================================================================
// GraphicsEngine Class
//=============================================================================
class GraphicsEngine {
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
	
	enum { FRAME_BUFFER_COUNT = 2 };	//フレームバッファの数

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief インスタンスを取得
	 * 
	 * \return	インスタンスアドレス
	 *********************************************************************/
	static GraphicsEngine* GetInstance();
	
	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~GraphicsEngine();
	
	/*****************************************************************//**
	 * \brief 初期化
	 *  
	 * \param hwnd				ウィンドウハンドル
	 * \param frameBufferWidth	フレームバッファの幅
	 * \param frameBufferHeight	フレームバッファの高さ
	 * \return true	 初期化に成功
	 * \return false 初期化に失敗
	 *********************************************************************/
	bool Init(HWND hwnd, uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	
	/*****************************************************************//**
	 * \brief レンダリング開始
	 *********************************************************************/
	void BeginRender();
	
	/*****************************************************************//**
	 * \brief レンダリング終了
	 *********************************************************************/
	void EndRender();
	
	/*****************************************************************//**
	 * \brief D3Dデバイスの取得
	 *  
	 * \return デバイス
	 *********************************************************************/
	ID3D12Device5* GetDevice(){
		return m_pDevice;
	}

	/*****************************************************************//**
	 * \brief バックバッファ番号を取得
	 *  
	 * \return バックバッファ番号
	 *********************************************************************/
	uint32_t GetBackBufferIndex() const{
		return m_frameIndex;
	}
	
	/*****************************************************************//**
	 * \brief コマンドキューを取得
	 *  
	 * \return コマンドキュー
	 *********************************************************************/
	ID3D12CommandQueue* GetCommandQueue() const{
		return m_pCommandQueue;
	}

	/*****************************************************************//**
	 * \brief コマンドリストを取得
	 *  
	 * \return コマンドリスト
	 *********************************************************************/
	ID3D12GraphicsCommandList4* GetCommandList() const{
		return m_pCommandList;
	}
	
	/*****************************************************************//**
	 * \brief CBR_SRVのディスクリプタサイズを取得
	 *  
	 * \return ディスクリプタサイズ
	 *********************************************************************/
	uint32_t GetCbrSrvDescriptorSize() const{
		return m_cbrSrvDescriptorSize;
	}
	
	/*****************************************************************//**
	 * \brief サンプラのディスクリプタサイズを取得
	 *  
	 * \return ディスクリプタサイズ
	 *********************************************************************/
	uint32_t GetSapmerDescriptorSize() const{
		return m_samplerDescriptorSize;
	}
	
	/*****************************************************************//**
	 * \brief レンダリングコンテキストを取得
	 *  
	 * \return レンダリングコンテキスト
	 *********************************************************************/
	RenderContext& GetRenderContext(){
		return m_renderContext;
	}
	
	/*****************************************************************//**
	 * \brief フレームバッファの幅を取得
	 *  
	 * \return フレームバッファの幅
	 *********************************************************************/
	uint32_t GetFrameBufferWidth() const{
		return m_frameBufferWidth;
	}
	
	/*****************************************************************//**
	 * \brief フレームバッファの高さを取得
	 *  
	 * \return フレームバッファの高さ
	 *********************************************************************/
	uint32_t GetFrameBufferHeight() const{
		return m_frameBufferHeight;
	}
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットをフレームバッファに変更
	 *  
	 * \param rc レンダリングコンテキスト
	 *********************************************************************/
	void ChangeRenderTargetToFrameBuffer(RenderContext& rc);
	
	/*****************************************************************//**
	 * \brief 現在のフレームバッファのRTVを取得
	 *  
	 * \return 現在のフレームバッファのRTV 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerRTV() const{
		return m_currentFrameBufferRTVHandle;
	}
	
	/*****************************************************************//**
	 * \brief フレームバッファの描画に使用しているDSVを取得
	 *  
	 * \return　フレームバッファの描画に使用しているDSV 
	 *********************************************************************/
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentFrameBuffuerDSV() const{
		return m_currentFrameBufferDSVHandle;
	}

	/*****************************************************************//**
	 * \brief フレームバッファにコピー
	 *  
	 * \param rc	レンダリングコンテキスト
	 * \param pSrc	コピー元
	 *********************************************************************/
	void CopyToFrameBuffer(RenderContext& rc, ID3D12Resource* pSrc){
		D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_COPY_DEST);
		rc.ResourceBarrier(barrier);
		rc.CopyResource(m_pRenderTargets[m_frameIndex], pSrc);

		D3D12_RESOURCE_BARRIER barrier2 = CD3DX12_RESOURCE_BARRIER::Transition(
			m_pRenderTargets[m_frameIndex],
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_RENDER_TARGET);
		rc.ResourceBarrier(barrier2);
	}
	
	/*****************************************************************//**
	 * \brief ビューポートを取得
	 *  
	 * \return ビューポート
	 *********************************************************************/
	D3D12_VIEWPORT& GetFrameBufferViewport(){
		return m_viewport;
	}

	/*****************************************************************//**
	 * \brief 2Dカメラのインスタンスアドレスを取得
	 * 
	 * \return 2Dカメラのインスタンスアドレス
	 *********************************************************************/
	Camera* GetCamera2D() {
		return &m_camera2D;
	}

	/*****************************************************************//**
	 * \brief 3Dカメラのインスタンスアドレスを取得
	 *
	 * \return 3Dカメラのインスタンスアドレス
	 *********************************************************************/
	Camera* GetCamera3D() {
		return &m_camera3D;
	}
	
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{ 
	
	// インスタンスですわー
	static GraphicsEngine* pInstance;
	
	//GPUベンダー定義
	enum GPU_Vender {
		GPU_VenderNvidia,	//NVIDIA
		GPU_VenderAMD,		//Intel
		GPU_VenderIntel,	//AMD
		Num_GPUVender,
	};

	ID3D12Device5* m_pDevice = nullptr;									//D3Dデバイス
	ID3D12CommandQueue* m_pCommandQueue = nullptr;						//コマンドキュー
	IDXGISwapChain3* m_pSwapChain = nullptr;							//スワップチェイン
	ID3D12DescriptorHeap* m_pRTVHeap = nullptr;							//RTVのディスクリプタヒープ
	ID3D12DescriptorHeap* m_pDSVHeap = nullptr;							//DSVのディスクリプタヒープ
	ID3D12CommandAllocator* m_pCommandAllocator = nullptr;				//コマンドアロケータ
	ID3D12GraphicsCommandList4* m_pCommandList = nullptr;				//コマンドリスト
	ID3D12PipelineState* m_pPipelineState = nullptr;					//パイプラインステート
	uint32_t m_currentBackBufferIndex = 0;								//現在のバックバッファの番号
	uint32_t m_rtvDescriptorSize = 0;									//フレームバッファのディスクリプタのサイズ
	uint32_t m_dsvDescriptorSize = 0;									//深度ステンシルバッファのディスクリプタのサイズ
	uint32_t m_cbrSrvDescriptorSize = 0;								//CBR_SRVのディスクリプタのサイズ
	uint32_t m_samplerDescriptorSize = 0;								//サンプラのディスクリプタのサイズ			
	ID3D12Resource* m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//フレームバッファ用のレンダリングターゲット
	ID3D12Resource* m_pDepthStencilBuffer = nullptr;					//深度ステンシルバッファ
	D3D12_VIEWPORT m_viewport;											//ビューポート
	D3D12_RECT m_scissorRect;											//シザリング矩形
	RenderContext m_renderContext;										//レンダリングコンテキスト
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;			//現在書き込み中のフレームバッファのRTVのハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;			//現在書き込み中のフレームバッファのDSVのハンドル
	
	
	// GPUとの同期で使用する変数
	uint32_t m_frameIndex = 0;
	HANDLE m_fenceEvent = nullptr;
	ID3D12Fence* m_pFence = nullptr;
	UINT64 m_fenceValue = 0;
	uint32_t m_frameBufferWidth = 0;	//フレームバッファの幅
	uint32_t m_frameBufferHeight = 0;	//フレームバッファの高さ
	Camera m_camera2D;					//2Dカメラ
	Camera m_camera3D;					//3Dカメラ

	std::unique_ptr<DirectX::GraphicsMemory> m_directXTKGfxMemroy;	//DirectXTKのグラフィックメモリシステム
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	
	/*****************************************************************//**
	 * \brief 秘匿コンストラクタ(1つ以上のインスタンス生成の禁止)　
	 *********************************************************************/
	GraphicsEngine();
	
	/*****************************************************************//**
	 * \brief デバイスを作成
	 *  
	 * \param dxgiFactory	DXGIファクトリー
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateD3DDevice(IDXGIFactory4* dxgiFactory);
	
	/*****************************************************************//**
	 * \brief コマンドキューを作成
	 *  
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateCommandQueue();
	
	/*****************************************************************//**
	 * \brief スワップチェインを作成
	 *  
	 * \param hwnd				ウィンドウハンドル
	 * \param frameBufferWidth	フレームバッファの幅
	 * \param frameBufferHeight フレームバッファの高さ
	 * \param dxgiFactory		DXGIファクトリー
	 * \return true  作成に成功
	 * \return false 作成に失敗 
	 *********************************************************************/
	bool CreateSwapChain(
		HWND hwnd,
		uint32_t frameBufferWidth,
		uint32_t frameBufferHeight,
		IDXGIFactory4* dxgiFactory
	);
	
	/*****************************************************************//**
	 * \brief DXGIファクトリーを作成
	 *  
	 * \return true ファクトリー
	 * \memo 便利な奴
 	 *********************************************************************/
	IDXGIFactory4* CreateDXGIFactory();
	
	/*****************************************************************//**
	 * \brief フレームバッファ用のディスクリプタヒープを作成
	 *  
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateDescriptorHeapForFrameBuffer();

	/*****************************************************************//**
	 * \brief フレームバッファ用のRTVを作成
	 *  
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateRTVForFameBuffer();
	
	/*****************************************************************//**
	 * \brief フレームバッファ用のDSVを作成
	 *  
	 * \param frameBufferWidth	フレームバッファの幅
	 * \param frameBufferHeight	フレームバッファの高さ
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateDSVForFrameBuffer(uint32_t frameBufferWidth, uint32_t frameBufferHeight);
	
	/*****************************************************************//**
	 * \brief コマンドリストを作成
	 *  
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateCommandList();
	
	/*****************************************************************//**
	 * \brief GPUとの同期用オブジェクトを作成
	 *  
	 * \return true  作成に成功
	 * \return false 作成に失敗
	 *********************************************************************/
	bool CreateSynchronizationWithGPUObject();
	
	/*****************************************************************//**
	 * \brief 描画の完了待ち
	 *********************************************************************/
	void WaitDraw();
	
	//!} 
};