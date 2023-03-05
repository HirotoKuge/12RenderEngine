/*****************************************************************//**
 * \file   Sprite.h
 * \brief  スプライト
 * 
 * \author Hiroto Kuge
 * \date   January 2023
 *********************************************************************/
#pragma once

//=============================================================================
// Includes
//=============================================================================
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"


class Texture;

//スプライトに設定できる最大テクスチャ数
constexpr uint32_t MAX_TEXTURE = 32;

//拡張SRVが設定されるレジスタの開始番号。
constexpr uint32_t EXPAND_SRV_REG_START_NO = 10;

class ShaderResource;

//=============================================================================
// アルファブレンディングモード
//=============================================================================
enum AlphaBlendMode {
	AlphaBlendMode_None,	//アルファブレンディングなし(上書き)
	AlphaBlendMode_Trans,	//半透明合成
	AlphaBlendMode_Add,		//加算合成
};

//=============================================================================
// InitData Sturuct
//=============================================================================
struct SpriteInitData {
	uint32_t width = 0;		// スプライトの幅
	uint32_t height = 0;	// スプライトの高さ

	std::array<Texture*, MAX_TEXTURE>		pTextures = { nullptr };	// 外部指定のテクスチャ(GBuffer)	
	std::array<const wchar_t*, MAX_TEXTURE> ddsFilePath = { nullptr };	// DDSファイルのファイルパス
	void*			expandConstantBuffer = nullptr;						// ユーザー拡張の定数バッファ
	uint32_t		expandConstantBufferSize = 0;						// ユーザー拡張の定数バッファのサイズ
	ShaderResource* pExpandShaderResoruceView = nullptr;				// ユーザー拡張のシェーダーリソース。
	AlphaBlendMode	alphaBlendMode = AlphaBlendMode_None;				// アルファブレンディングモード
	D3D12_FILTER	samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// サンプラのフィルター
	
	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormats = {
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	DXGI_FORMAT_UNKNOWN,
	};	//レンダリングするカラーバッファのフォーマット。

};


//=============================================================================
// Sprite Class
//=============================================================================
class Sprite {
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

	static const Vector2 DEFAULT_PIVOT;	//ピボット

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{

	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~Sprite();

	/*****************************************************************//**
	 * \brief 初期化処理
	 *
	 * \param	initData 初期化データ
	 *********************************************************************/
	void Init(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief 更新処理
	 * 
	 * \param pos	座標
	 * \param rot	回転
	 * \param scale	サイズ
	 * \param pivot	ピボット
	 *********************************************************************/
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot = DEFAULT_PIVOT);

	/*****************************************************************//**
	 * \brief 描画
	 * 
	 * \param renderContext　レンダリングコンテキスト
	 *********************************************************************/
	void Draw(RenderContext& renderContext);


	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{

	VertexBuffer	m_vertexBuffer;		// 頂点バッファ
	IndexBuffer		m_indexBuffer;		// インデックスバッファ

	uint32_t m_numTexture = 0;	// テクスチャの数
	Texture  m_textures[MAX_TEXTURE];										 // 描画に使用するテクスチャ(各種GBufferのレンダリング結果が入るバターンもあるよ)
	std::shared_ptr<Texture> m_pTextureExternals[MAX_TEXTURE] = { nullptr }; // 外部から指定されたテクスチャ
	
	Vector3		m_position;		// 座標
	Vector2		m_size;			// サイズ
	Quaternion	m_quaternion;	// 回転
	Matrix		m_worldMtx;		// ワールド変換行列

	RootSignature	m_rootSignature;	// ルートシグネチャ
	PipelineState	m_pipelineState;	// パイプラインステート
	DescriptorHeap	m_descriptorHeap;	// ディスクリプタヒープ

	// スプライトで使うローカル定数バッファ
	struct SpriteConstantBuffer {
		Matrix mvp;
		Vector4 mulColor;
		Vector4 screenParam;
	};
	SpriteConstantBuffer	m_constantBufferCPU;					 // スプライト用定数バッファ
	ConstantBuffer			m_constantBufferGPU;					 // GPU側の定数バッファ
	void*					m_userExpandConstantBufferCPU = nullptr; // ユーザー拡張の定数バッファ(CPU側)
	ConstantBuffer			m_userExpandConstantBufferGPU;			 // ユーザー拡張の定数バッファ(GPU側)

	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{

	/*****************************************************************//**
	 * \brief テクスチャの初期化
	 *  
	 * \param initData 初期化データ
	 *********************************************************************/
	void InitTextures(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief 頂点バッファと定数バッファを初期化
	 *  
	 * \param initData 初期化データ
	 *********************************************************************/
	void InitVertexBufferAndIndexBuffer(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief 定数バッファの初期化
	 *  
	 * \param initData 初期化データ
	 *********************************************************************/
	void InitConstantBuffer(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief パイプラインステートの初期化
	 *  
	 * \param initData 初期化データ
	 *********************************************************************/
	void InitPipelineState(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief ディスクリプタヒープを初期化
	 * 
	 * \param initData 初期化データ
	 *********************************************************************/
	void InitDescriptorHeap(const SpriteInitData& initData);

	//!} 
};
