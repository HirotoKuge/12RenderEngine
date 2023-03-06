/*****************************************************************//**
 * \file   Model.h
 * \brief  モデルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Include 
//=============================================================================
#include "Mesh.h"

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
	~Model(){};


	/*****************************************************************//**
	 * \brief 初期化
	 * 
	 * \param initData 初期化データ
	 *********************************************************************/
	void Init( const ModelInitData& initData );
	
	
	/*****************************************************************//**
	 * \brief ワールド変換行列を更新
	 * 
	 * \param pos	座標
	 * \param rot	回転
	 * \param scale	サイズ
	 *********************************************************************/
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/*****************************************************************//**
	 * \brief 描画
	 * 
	 * \param renderContext レンダリングコンテキスト
	 *********************************************************************/
	void Draw(RenderContext& renderContext);
	
	
	/*****************************************************************//**
	 * \brief カメラ指定描画
	 * 
	 * \param renderContext レンダリングコンテキスト
	 * \param camera		カメラ
	 *********************************************************************/
	void Draw(RenderContext& renderContext, Camera& camera);
	
	/*****************************************************************//**
	 * \brief カメラ行列指定描画
	 * 
	 * \param renderContext	レンダリングコンテキスト
	 * \param viewMatrix	ビュー変換行列
	 * \param projMatrix	プロジェクション変換行列
	 *********************************************************************/
	void Draw(RenderContext& renderContext, const Matrix& viewMatrix, const Matrix& projMatrix);
	
	/*****************************************************************//**
	 * \brief インスタンシング描画
	 * 
	 * \param renderContext	レンダリングコンテキスト
	 * \param numInstance	インスタンス数
	 *********************************************************************/
	void DrawInstancing(RenderContext& renderContext, int numInstance);
	
	/*****************************************************************//**
	 * \brief ワールド変換行列を取得
	 * 
	 * \return ワールド変換行列
	 *********************************************************************/
	const Matrix& GetWorldMatrix() const{
		return m_worldMtx;
	}
	
	/*****************************************************************//**
	 * \brief 初期化されているかを確認
	 * 
	 * \return 初期化されてるか
	 *********************************************************************/
	bool IsInited() const{
		return m_isInited;
	}
	
	/*****************************************************************//**
	 * \brief ワールド変換行列を計算
	 * 
	 * \param pos	  座標
	 * \param rot	  回転
	 * \param scale	  サイズ
	 * \return 計算結果
	 *********************************************************************/
	
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
	
	bool m_isInited = false; // 初期化されているかどうか
	Matrix m_worldMtx;		 // ワールド行列
	Mesh m_meshParts;		 // メッシュパーツ

	//TODO:今後ボーン情報とか追加できるといいね
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	 
	//!} 
};