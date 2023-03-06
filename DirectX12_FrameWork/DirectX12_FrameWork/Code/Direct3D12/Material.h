/*****************************************************************//**
 * \file   Material.h
 * \brief  マテリアルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//!} 
//-----------------------------------------------------------------------------
// Struct 
//-----------------------------------------------------------------------------
//!{

struct PBR_Param {
	float Metallic;
	float Roughness;
};
 

//=============================================================================
// 初期化データ
//=============================================================================
struct MaterialInitData {
	const char* p_psFilePath;		// ピクセルシェーダーのファイルパス
	const char* p_vsFilePath;		// 頂点シェーダーのファイルパス
	const char* p_directryPath;		// リソースデータのディレクトリパス
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat;	// カラーターゲットのフォーマット
	uint32_t numSrv;				// シェーダーリソースの数
	uint32_t numCbv;				// 定数バッファの数
	uint32_t offsetInDescriptorsFromTableStartCB;	
	uint32_t offsetInDescriptorsFromTableStartSRV;
	D3D12_FILTER samplerFilter;
	ResMaterialPBR& resMaterial;	// マテリアルリソース
};

//!} 
//-----------------------------------------------------------------------------
// Enum
//-----------------------------------------------------------------------------
//!{

//=============================================================================
// テクスチャの使用用途
//=============================================================================
enum TEXTURE_USAGE_PBR {
	ALBEDO = 0,		// アルベドマップ
	METALLIC,		// メタリックマップ
	ROUGHNESS,		// ラフネスマップ
	NORMAL,			// ノーマルマップ
	HEIGHT,			// ハイトマップ
	MAX
};


//!} 
//=============================================================================
// Material Class
//=============================================================================
class Material{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//=============================================================================
	// enum
	//=============================================================================
	// テクスチャの用途
	enum TEXTURE_USAGE{
		TEXTURE_USAGE_DIFFUSE = 0,	// ディフューズマップ
		TEXTURE_USAGE_SPECULAR,		// スペキュラマップ
		TEXTURE_USAGE_SHININESS,	// シャイネスマップ
		TEXTURE_USAGE_NORMAL,		// 法線マップ

		TEXTURE_USAGE_BASECOLOR,	// ベースカラーマップ
		TEXTURE_USAGE_METALLIC,		// メタリックマップ
		TEXTURE_USAGE_ROUGTHNESS,	// ラフネスマップ

		TEXTURE_USAGE_COUNT			// 定義数
	};

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
	
	Material(){};

	/*****************************************************************//**
	 * \brief 初期化
	 *  
	 * \param resMaterial		マテリアルリソースデータ
	 * \param p_vsFilePath		頂点シェーダーファイルパス
	 * \param p_psFilePath		ピクセルシェーダーファイルパス
	 * \param colorBufferFormat	カラーバッファのフォーマット
	 * \param numSrv			シェーダーリソースの数
	 * \param numCbv			定数バッファの数
	 * \param offsetInDescriptorsFromTableStartCB	オフセット
	 * \param offsetInDescriptorsFromTableStartSRV	オフセット
	 * \param samplerFilter		サンプラーフィルター
	 * \return 
	 *********************************************************************/
	bool Init(
		const ResMaterialPBR& resMaterial,	
		const wchar_t* p_vsFilePath,			
		const wchar_t* p_psFilePath,			
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		uint32_t numSrv,
		uint32_t numCbv,
		uint32_t offsetInDescriptorsFromTableStartCB,
		uint32_t offsetInDescriptorsFromTableStartSRV,
		D3D12_FILTER samplerFilter);

	
	/*****************************************************************//**
	 * \brief 描画前処理
	 * 
	 * \param rc レンダリングコンテキスト
	 *********************************************************************/
	void BeginRender(RenderContext& rc);

	/*****************************************************************//**
	 * \brief テクスチャを取得
	 * 
	 * \return テクスチャ
	 *********************************************************************/
	Texture& GetAlbedoMap(){
		return *m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO];
	}
	Texture& GetNormalMap(){
		return *m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL];
	}
	
	/*****************************************************************//**
	 * \brief 定数バッファを取得
	 * 
	 * \return 定数バッファ 
	 *********************************************************************/
	ConstantBuffer& GetConstantBuffer()	{
		return m_constantBuffer;
	}
	//!}

	/*****************************************************************//**
	 * \brief 定数バッファのポインタを取得
	 *  
	 * \param index	マテリアル番号
	 * \return 指定されたマテリアル番号に対応する定数バッファのポインタ
	 *		   一致するものがない場合は nullptr
	 *********************************************************************/
	void* GetBufferPtr(size_t index)const;

	/*****************************************************************//**
	 * \brief 定数バッファのポインタを取得
	 *
	 * \param index	マテリアル番号
	 * \return 指定されたマテリアル番号に対応する定数バッファのポインタ
	 *		   一致するものがない場合は nullptr
	 *********************************************************************/
	template<typename T>
	T* GetBufferPtr(size_t index)const 
	{ return reinterpret_cast<T*>(GetBufferPtr(index)); }

private:
	//=============================================================================
	// structures
	//=============================================================================
	// サブセット
	struct Subset {
		ConstantBuffer* pCostantBuffer;									// 定数バッファ
		D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle[TEXTURE_USAGE_COUNT];	// テクスチャハンドル
	};

	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	std::array<Texture*,TEXTURE_USAGE_PBR::MAX> m_pTexturesPBR;	// テクスチャ
	
	ConstantBuffer m_constantBuffer;	// 定数バッファ
	ConstantBuffer m_materialCB;		// 定数バッファ
	RootSignature m_rootSignature;		// ルートシグネチャ
	PipelineState m_pipelineState;		// パイプラインステート
	Shader* m_pVSShader;
	Shader* m_pPSShader;

	uint32_t m_numSr; // シェーダーリソースの数(テクスチャ枚数)
	uint32_t m_numCb; // 定数バッファの数

	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	
	/*****************************************************************//**
	 * \brief パイプラインステートの初期化
	 * 
	 * \param colorBufferFormats カラーターゲットのフォーマットの配列
	 *********************************************************************/
	void InitPipelineState(const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats);

	/*****************************************************************//**
	 * \brief シェーダーの初期化
	 * 
	 * \param pVSShaderPath
	 * \param pPSShaderPath
	 *********************************************************************/
	void InitShader(const wchar_t* pVSShaderPath, const wchar_t* pPSShaderPath);
	void InitShaders(
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntriyPointFunc,
		const char* psEntryPointFunc);


	/*****************************************************************//**
	 * \brief テクスチャの初期化
	 *  
	 * \param pDevice		デバイス
	 * \param pCommandQueue	コマンドキュー
	 * \param resMaterial	マテリアルリソース
	 *********************************************************************/
	void InitTexture(
		ID3D12Device5* pDevice,
		ID3D12CommandQueue* pCommandQueue, 
		const ResMaterialPBR& resMaterial);

	Material(const Material&) = delete;
	void operator = (const Material&) = delete;
	
	static const wchar_t* DummyTag;

	//!} 
};

