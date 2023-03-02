/*****************************************************************//**
 * \file   Model.h
 * \brief  モデルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Includes
//=============================================================================
#include "MDLoader.h"
#include "GraphicsEngine.h"
#include "DescriptorHeap.h"
#include "Mesh.h"
#include "Material.h"
#include "ShaderResourse.h"
#include "../Util/Matrix.h"


//=============================================================================
// Forword Decolations 
//=============================================================================
class ShaderResource;



//=============================================================================
// Model InitData
//=============================================================================
struct ModelInitData {	
	const wchar_t* pPathMD;	// モデルデータのリソースファイルパス
	const wchar_t* pPathMT;	// マテリアルデータのリソースファイルパス

	const wchar_t* pVSShaderPath;	// 頂点シェーダーのファイルパス
	const wchar_t* pPSShaderPath;	// ピクセルシェーダーのファイルパス

	void* pExpandConstantBuffer = nullptr;	//　ユーザー拡張の定数バッファ
	int expandConstantBufferSize = 0;		//　ユーザー拡張の定数バッファのサイズ
											//　インスタンシング描画の差異はこのパラメータに最大のインスタンス数を指定

	std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV> pExpandShaderResoruceViews = { nullptr };	//ユーザー拡張のシェーダーリソース

	D3D12_FILTER m_samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//テクスチャサンプリングのフィルタ
	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormats = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット
};

//=============================================================================
// Model Class
//=============================================================================
class Model{
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
	~Model();


	/*****************************************************************//**
	 * \brief 初期化
	 * 
	 * \param initData 初期化データ
	 *********************************************************************/
	void Init( const ModelInitData& initData );
	
	/// <summary>
	/// ワールド行列を計算して、メンバ変数のm_worldMatrixをこぅしんする。
	/// </summary>
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト</param>
	void Draw(RenderContext& renderContext);
	
	/// <summary>
	/// 描画(カメラ指定版)
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト</param>
	/// <param name="camera">カメラ</param>
	void Draw(RenderContext& renderContext, Camera& camera);
	/// <summary>
	/// 描画(カメラ行列指定版)
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト</param>
	/// <param name="viewMatrix">ビュー行列</param>
	/// <param name="projMatrix">プロジェクション行列</param>
	void Draw(RenderContext& renderContext, const Matrix& viewMatrix, const Matrix& projMatrix);
	/// <summary>
	/// インスタンシング描画
	/// </summary>
	/// <param name="renderContext">レンダリングコンテキスト</param>
	/// <param name="numInstance">インスタンスの数</param>
	void DrawInstancing(RenderContext& renderContext, int numInstance);
	/// <summary>
	/// ワールド行列を取得。
	/// </summary>
	/// <returns></returns>
	const Matrix& GetWorldMatrix() const
	{
		return m_worldMtx;
	}
	

	/// <summary>
	/// 初期化されているか判定。
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return m_isInited;
	}
	/// <summary>
	/// ワールド行列を計算する。
	/// </summary>
	/// <remark>
	/// この関数はUpdateWorldMatrix関数の中から使われています。
	/// Modelクラスの使用に沿ったワールド行列の計算を行いたい場合、
	/// 本関数を利用すると計算することができます。
	/// </remark>
	/// <param name="pos">座標</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率。</param>
	/// <returns></returns>
	Matrix CalcWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale){
		Matrix worldMtx;
	
		Matrix transMtx, rotMtx,scaleMtx;
		transMtx.MakeTranslation(pos);
		rotMtx.MakeRotationFromQuaternion(rot);
		scaleMtx.MakeScaling(scale);

		worldMtx = scaleMtx * rotMtx * transMtx;
		return worldMtx;
	}


	//!} 
private:	
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	bool m_isInited = false; // 初期化されている
	Matrix m_worldMtx;		 // ワールド行列
	Mesh m_meshParts;		 // メッシュパーツ。
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	 
	//!} 
};