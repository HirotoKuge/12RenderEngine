/*****************************************************************//**
 * \file   RenderContext.h
 * \brief  レンダーコンテキスト
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once


//=============================================================================
// Include
//=============================================================================
#include <array>
#include <vector>
#include <d3d12.h>
#include <d3dx12.h>
#include "ComPtr.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "DescriptorHeap.h"


//-----------------------------------------------------------------------------
// Forword Decolations
//-----------------------------------------------------------------------------
//!{
class ConstantBuffer;
class Texture;
class DescriptorHeap;
class RenderTarget;

//!} 
//-----------------------------------------------------------------------------
// Struct 
//-----------------------------------------------------------------------------
//!{

//!} 
//-----------------------------------------------------------------------------
// Enum 
//-----------------------------------------------------------------------------
//!{

//!} 

//=============================================================================
// RenderContext Class
//=============================================================================
class RenderContext{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	RenderContext(){}

	/*****************************************************************//**
	 * \brief 初期化
	 *  
	 * \param commandList　コマンドリスト
	 *********************************************************************/
	void Init(ID3D12GraphicsCommandList4* commandList){
		m_pCommandList = commandList;
	}
	
	/*****************************************************************//**
	 * \brief 頂点バッファを設定
	 *  
	 * \param vb 頂点バッファ
	 *********************************************************************/
	void SetVertexBuffer(VertexBuffer& vb){
		m_pCommandList->IASetVertexBuffers(0, 1, &vb.GetView());
	}
	
	/*****************************************************************//**
	 * \brief インデックスバッファを設定
	 *  
	 * \param ib インデックスバッファ
	 *********************************************************************/
	void SetIndexBuffer(IndexBuffer& ib){
		m_pCommandList->IASetIndexBuffer(&ib.GetView());
	}

	
	/*****************************************************************//**
	 * \brief プリミティブトポロジーを設定
	 *  
	 * \param topology トポロジー
	 *********************************************************************/
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology){
		m_pCommandList->IASetPrimitiveTopology(topology);
	}
	
	/*****************************************************************//**
	 * \brief コマンドリストを設定
	 *  
	 * \param commandList
	 *********************************************************************/
	void SetCommandList(ID3D12GraphicsCommandList4* commandList){
		m_pCommandList = commandList;
	}
	
	/*****************************************************************//**
	 * \brief ビューポートとシザリング矩形を設定
	 *  
	 * \param viewport ビューポート
	 *********************************************************************/
	void SetViewportAndScissor(D3D12_VIEWPORT& viewport){
		//シザリング矩形も設定する。
		D3D12_RECT scissorRect;
		scissorRect.bottom = static_cast<LONG>(viewport.Height);
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = static_cast<LONG>(viewport.Width);
		SetScissorRect(scissorRect);

		m_pCommandList->RSSetViewports(1, &viewport);
		m_currentViewport = viewport;
	}
	
	/*****************************************************************//**
	 * \brief ビューポートを取得
	 *  
	 * \return ビューポート
	 *********************************************************************/
	D3D12_VIEWPORT GetViewport() const{
		return m_currentViewport;
	}
	
	/*****************************************************************//**
	 * \brief シザリング矩形を設定
	 *  
	 * \param rect 矩形
	 *********************************************************************/
	void SetScissorRect(D3D12_RECT& rect){
		m_pCommandList->RSSetScissorRects(1, &rect);
	}

	/*****************************************************************//**
	 * \brief ルートシグネチャを設定
	 *  
	 * \param rootSignature ルートシグネチャ
	 *********************************************************************/
	void SetRootSignature(ID3D12RootSignature* rootSignature){
		m_pCommandList->SetGraphicsRootSignature(rootSignature);
	}
	void SetRootSignature(RootSignature& rootSignature){
		m_pCommandList->SetGraphicsRootSignature(rootSignature.Get());
	}

	/*****************************************************************//**
	 * \brief ルートシグネチャを設定(コンピュートシェーダー用)
	 *  
	 * \param rootSignature ルートシグネチャ
	 *********************************************************************/
	void SetComputeRootSignature(ID3D12RootSignature* rootSignature){
		m_pCommandList->SetComputeRootSignature(rootSignature);
	}
	void SetComputeRootSignature(RootSignature& rootSignature){
		m_pCommandList->SetComputeRootSignature(rootSignature.Get());
	}
	
	/*****************************************************************//**
	 * \brief パイプラインステートを設定
	 *  
	 * \param pipelineState パイプラインステート
	 *********************************************************************/
	void SetPipelineState(ID3D12PipelineState* pipelineState){
		m_pCommandList->SetPipelineState(pipelineState);
	}
	void SetPipelineState(PipelineState& pipelineState)	{
		m_pCommandList->SetPipelineState(pipelineState.Get());
	}

	/*****************************************************************//**
	 * \brief ディスクリプタヒープを設定
	 *  
	 * \param descHeap ディスクリプタヒープ
	 *********************************************************************/
	void SetDescriptorHeap(ID3D12DescriptorHeap* descHeap){
		m_descriptorHeaps[0] = descHeap;
		m_pCommandList->SetDescriptorHeaps(1, m_descriptorHeaps);
	}

	void SetDescriptorHeap(DescriptorHeap& descHeap);
	void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
	
	/*****************************************************************//**
	 * \brief 複数のディスクリプタヒープを登録
	 *  
	 * \param numDescriptorHeap ディスクリプタヒープの数
	 * \param descPools			ディスクリプタヒープの配列
	 *********************************************************************/
	void SetDescriptorHeaps(int numDescriptorHeap, const DescriptorHeap* descHeaps[]){
		for (int i = 0; i < numDescriptorHeap; i++) {
			m_descriptorHeaps[i] = descHeaps[i]->Get();
		}
		m_pCommandList->SetDescriptorHeaps(numDescriptorHeap, m_descriptorHeaps);
	}
	
	/*****************************************************************//**
	 * \brief 定数バッファを設定
	 *  
	 * \param registerNo 設定するレジスタ番号
	 * \param cb		 定数バッファ
	 *********************************************************************/
	void SetConstantBuffer(int registerNo, ConstantBuffer& cb){
		if (registerNo < MAX_CONSTANT_BUFFER) {
			m_constantBuffers[registerNo] = &cb;
		}
		else {
			//範囲外アクセス。
			std::abort();
		}
	}
	
	/*****************************************************************//**
	 * \brief シェーダーリソース(テクスチャ)を設定
	 *  
	 * \param registerNo 設定するレジスタ番号
	 * \param texture	 テクスチャ
	 *********************************************************************/
	void SetShaderResource(int registerNo, Texture& texture){
		if (registerNo < MAX_SHADER_RESOURCE) {
			m_shaderResources[registerNo] = &texture;
		}
		else {
			//範囲外アクセス。
			std::abort();
		}
	}
	
	/*****************************************************************//**
	 * \brief 複数のレンダリングターゲットを設定(MRT用)
	 *  
	 * \param numRT			レンダリングターゲットの数
	 * \param renderTargets レンダリングターゲットの配列
	*********************************************************************/
	void SetRenderTargets(UINT numRT, RenderTarget* renderTargets[]);

	/*****************************************************************//**
	 * \brief レンダリングターゲットを設定
	 *  
	 * \param rtvHandle カラーターゲットのディスクリプタハンドル
	 * \param dsvHandle 深度ターゲットのディスクリプタハンドル
	 *********************************************************************/
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle){
		m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}

	/*****************************************************************//**
	 * \brief レンダリングターゲットをスロット0に設定
	 *  
	 * \param renderTarget レンダリングターゲット
	 * \memo  ビューポート設定はユーザー側で行わないといけない
	 *********************************************************************/
	void SetRenderTarget(RenderTarget& renderTarget){
		RenderTarget* rtArray[] = { &renderTarget };
		SetRenderTargets(1, rtArray);
	}

	/*****************************************************************//**
	 * \brief レンダリングターゲットとビューポートの設定
	 *  
	 * \param renderTarget レンダリングターゲット
	 * \memo  自動でレンダリングターゲットと同じ幅と高さのビューポートが設定される
	 *********************************************************************/
	void SetRenderTargetAndViewport(RenderTarget& renderTarget);
	
	/*****************************************************************//**
	 * \brief 複数枚のレンダリングターゲットとビューポートを同時に設定
	 *  
	 * \param numRT			レンダリングターゲットの個数
	 * \param renderTargets レンダリングターゲットの配列
	 * \memo  自動でレンダリングターゲットと同じ幅と高さのビューポートが設定される
	 *********************************************************************/
	void SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[]);


	/*****************************************************************//**
	 * \brief 複数枚のレンダリングターゲットをクリア
	 *  
	 * \param numRt			レンダリングターゲットの個数
	 * \param renderTargets レンダリングターゲットの配列
	 * \memo クリアカラーはレンダリングターゲットに設定してるやつを使う
	 *********************************************************************/
	void ClearRenderTargetViews(
		int numRt,
		RenderTarget* renderTargets[]
	);
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットをクリア
	 *  
	 * \param rtvHandle  レンダーターゲットのCPUディスクリプタハンドル
	 * \param clearColor クリアカラー
	 *********************************************************************/
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor){
		m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	/*****************************************************************//**
	 * \brief レンダリングターゲットのクリア
	 *  
	 * \param renderTarget レンダリングターゲット
	 *********************************************************************/
	void ClearRenderTargetView(RenderTarget& renderTarget){
		RenderTarget* rtArray[] = { &renderTarget };
		ClearRenderTargetViews(1, rtArray);
	}

	/*****************************************************************//**
	 * \brief 深度ターゲットをクリア
	 *  
	 * \param dsvHandle	 深度ターゲットのCPUディスクリプタハンドル
	 * \param clearValue クリア値
	 *********************************************************************/
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue){
		m_pCommandList->ClearDepthStencilView(
			dsvHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			clearValue,
			0,
			0,
			nullptr);
	}
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットへの書き込み待ち
	 *  
	 * \memo レンダリングターゲットをシェーダーリソース(テクスチャ)として使うには
	 *		 この関数を使って書き込みの完了を待つ必要がある
	 *********************************************************************/
	void WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget);
	void WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget){
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief レンダリングターゲットとして使用可能になるまで待つ
	 *  
	 * \memo レンダリングターゲットとして設定したい場合は
	 *		 この関数を使って使用可能になるまで待つ必要がある
	 *********************************************************************/
	void WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget);
	void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget){
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}

	/*****************************************************************//**
	 * \brief リソースバリア
	 *  
	 * \param barrier バリア
	 *********************************************************************/
	void ResourceBarrier(D3D12_RESOURCE_BARRIER& barrier){
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief リソースステートの遷移
	 *  
	 * \param resrouce	  遷移したいリソース
	 * \param beforeState 遷移前のステート
	 * \param afterState  遷移後のステート
	 *********************************************************************/
	void TransitionResourceState(ID3D12Resource* resrouce, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState){
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resrouce, beforeState, afterState);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief コマンドリストを閉じる
	 *********************************************************************/
	void Close(){
		m_pCommandList->Close();
	}
	
	/*****************************************************************//**
	 * \brief コマンドリストをリセット
	 *  
	 * \param commandAllocator コマンドアロケーター
	 * \param pipelineState	   パイプラインステート
	 *********************************************************************/
	void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState){
		m_pCommandList->Reset(commandAllocator, pipelineState);
		//スクラッチリソースをクリア。
		m_scratchResourceList.clear();
	}
	
	/*****************************************************************//**
	 * \brief インデックス付きプリミティブを描画
	 *  
	 * \param indexCount インデックス数
	 *********************************************************************/
	void DrawIndexed(UINT indexCount){
		m_pCommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}
	
	/*****************************************************************//**
	 * \brief インスタンシング描画
	 *  
	 * \param indexCount  インデックス数
	 * \param numInstance インスタンス数
	 *********************************************************************/
	void DrawIndexedInstanced(UINT indexCount, UINT numInstance){
		m_pCommandList->DrawIndexedInstanced(indexCount, numInstance, 0, 0, 0);
	}

	/*****************************************************************//**
	 * \brief コンピュートシェーダーをディスパッチ
	 *  
	 * \param ThreadGroupCountX
	 * \param ThreadGroupCountY
	 * \param ThreadGroupCountZ
	 *********************************************************************/
	void Dispatch(
		UINT ThreadGroupCountX,
		UINT ThreadGroupCountY,
		UINT ThreadGroupCountZ
	){
		m_pCommandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}
	
	/*****************************************************************//**
	 * \brief リソースをコピー
	 *  
	 * \param pDst コピー先のリソース
	 * \param pSrc コピー元のリソース
	 *********************************************************************/
	void CopyResource(ID3D12Resource* pDst, ID3D12Resource* pSrc){
		m_pCommandList->CopyResource(pDst, pSrc);
	}

	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	enum { MAX_DESCRIPTOR_HEAP = 4 };	//ディスクリプタヒープの最大数(Resourse,RTV,DSV,Samplerの4つ)
	enum { MAX_CONSTANT_BUFFER = 8 };	//定数バッファの最大数:不足したら増やす
	enum { MAX_SHADER_RESOURCE = 16 };	//シェーダーリソースの最大数:不足したら増やす

	D3D12_VIEWPORT				m_currentViewport;										//現在のビューポート
	ID3D12GraphicsCommandList4* m_pCommandList;											//コマンドリスト
	ID3D12DescriptorHeap*		m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];					//ディスクリプタヒープの配列
	ConstantBuffer*				m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//定数バッファの配列
	Texture*					m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };	//シェーダーリソースの配列

	std::vector<ComPtr<ID3D12Resource> > m_scratchResourceList;	//スクラッチリソースのリスト
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief ディスクリプタテーブルを設定
	 *  
	 * \param RootParameterIndex ルートパラメータのインデックス
	 * \param BaseDescriptor	 先頭のディスクリプタハンドル
	 *********************************************************************/
	void SetGraphicsRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor){
		m_pCommandList->SetGraphicsRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}
	
	/*****************************************************************//**
	 * \brief ディスクリプタテーブルを設定
	 *  
	 * \param RootParameterIndex ルートパラメータのインデックス
	 * \param BaseDescriptor	 先頭のディスクリプタハンドル
	 *********************************************************************/
	void SetComputeRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor){
		m_pCommandList->SetComputeRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}

	//!} 	
};
