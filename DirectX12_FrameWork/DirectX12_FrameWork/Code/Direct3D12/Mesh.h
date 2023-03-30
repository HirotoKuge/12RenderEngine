/*****************************************************************//**
 * \file   Mesh.h
 * \brief  メッシュクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once


class RenderContext;
class Material;
class ShaderResource;
class DescriptorHeap;

const int MAX_MODEL_EXPAND_SRV = 6;

//=============================================================================
// 単位メッシュ
//=============================================================================
struct UnitMesh {
	VertexBuffer				vertexBuffer;		//頂点バッファ
	std::vector<IndexBuffer*>	pIndexBufferArray;	//インデックスバッファ
	std::vector<Material*>		pMaterials;			//マテリアル
};

//=============================================================================
// Mesh Class
//=============================================================================
class Mesh {
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
	 * \brief デストラクタ
	 *********************************************************************/
	~Mesh();

	/*****************************************************************//**
	 * \brief ファイルからモデルデータを初期化
	 *
	 * \param pMDFilePath					MDファイルパス
	 * \param pVSShaderPath					頂点シェーダーのファイルパス
	 * \param pPSShaderPath					ピクセルシェーダーのファイルパス
	 * \param pExpandData					拡張データ
	 * \param expandDataSize				拡張データサイズ
	 * \param pExpandShaderResourceViews	拡張シェーダーリソースの配列の参照
	 * \param colorBufferFormats			レンダーターゲットのフォーマット
	 * \param samplerFilter					サンプラーフィルター
	 *********************************************************************/
	void InitFromFile(
		const wchar_t* pMDFilePath,
		const wchar_t* pVSShaderPath,
		const wchar_t* pPSShaderPath,
		void* pExpandData,
		int expandDataSize,
		const std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV>& pExpandShaderResourceViews,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);


	/*****************************************************************//**
	 * \brief mdファイルからモデルデータを初期化
	 * 
	 * \param mdFile						.mdファイルの実体
	 * \param pMDFilePath					.mdファイルパス
	 * \param pVSShaderPath					頂点シェーダー
	 * \param pPSShaderPath
	 * \param pExpandData
	 * \param expandDataSize
	 * \param pExpandShaderResourceViews
	 * \param colorBufferFormats
	 * \param samplerFilter
	 *********************************************************************/
	void InitFromMDFile(
		const mdFile& mdFile,
		const wchar_t* pMDFilePath,
		const wchar_t* pVSShaderPath,
		const wchar_t* pPSShaderPath,
		void* pExpandData,
		int expandDataSize,
		const std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV>& pExpandShaderResourceViews,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);
	
	/*****************************************************************//**
	 * \brief MDファイルからモデルデータを初期化
	 * 
	 * \param mdFile						.mdファイルの実体
	 * \param pShaderFilePath				シェーダーファイルのパス
	 * \param pVSEntryPointFunc				頂点シェーダーのエントリポイント
	 * \param pPSEntryPointFunc				ピクセルシェーダーのエントリポイント
	 * \param pExpandData					拡張データ
	 * \param expandDataSize				拡張データのサイズ
	 * \param pExpandShaderResourceViews	拡張シェーダーリソースの配列
	 * \param colorBufferFormats			レンダリング先のカラーバッファのフォーマット
	 * \param samplerFilter					サンプラーフィルター
	 *********************************************************************/
	void InitFromMDFile(
		const mdFile& mdFile,
		const char* pShaderFilePath,
		const char* pVSEntryPointFunc,
		const char* pPSEntryPointFunc,
		void* pExpandData,
		int expandDataSize,
		const std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV>& pExpandShaderResourceViews,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);



	/*****************************************************************//**
	 * \brief 描画
	  * 
	 * \param rc		レンダリングコンテキスト
	 * \param worldMtx	ワールド変換行列
	 * \param viewMtx	ビュー変換行列
	 * \param projMtx	プロジェクション変換行列
	 *********************************************************************/
	void Draw(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
	
	/*****************************************************************//**
	 * \brief インスタンシング描画
	  * 
	 * \param rc		レンダリングコンテキスト
	 * \param worldMtx	ワールド変換行列
	 * \param viewMtx	ビュー変換行列
	 * \param projMtx	プロジェクション変換行列
	 *********************************************************************/
	void DrawInstancing(RenderContext& rc, uint32_t numInstance,const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
	
	/*****************************************************************//**
	 * \brief ディスクリプタヒープを作成
	 *********************************************************************/
	void CreateDescriptorHeaps();



	//!}
private: 
	//-----------------------------------------------------------------------------
	// private variables.
	//----------------------------------------------------------------------------- 
	//!{

	
	//拡張SRVが設定されるレジスタの開始番号
	const int EXPAND_SRV_REG__START_NO = 10;
	//１つのマテリアルで使用されるSRVの数
	const int NUM_SRV_ONE_MATERIAL = EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV;
	//１つのマテリアルで使用されるCBVの数
	const int NUM_CBV_ONE_MATERIAL = 2;
	
	//=============================================================================
	// モデル用のローカル定数バッファ
	//=============================================================================
	struct LocalConstantBuffer {
		Matrix mWorld;		//ワールド行列
		Matrix mView;		//ビュー行列
		Matrix mProj;		//プロジェクション行列
	};

	ConstantBuffer m_commonConstantBuffer;					//メッシュ共通の定数バッファ
	ConstantBuffer m_expandConstantBuffer;					//ユーザー拡張用の定数バッファ
	std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV> m_pExpandShaderResourceViews = { nullptr };	//ユーザー拡張シェーダーリソースビュー
	std::vector<UnitMesh*> m_pMeshs;						//メッシュ
	DescriptorHeap m_descriptorHeap;					//ディスクリプタヒープ
	void* m_pExpandData = nullptr;						//ユーザー拡張データ

	static MDLoader	m_loader; // リソースローダー

	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief リソースからメッシュを作成
	 * 
	 * \param meshRes						メッシュリソース
	 * \param meshNo						メッシュ番号
	 * \param materialNum					マテリアル数
	 * \param pVSShaderPath					頂点シェーダーのファイルパス			
	 * \param pPSShaderPath					ピクセルシェーダーのファイルパス
	 * \param pExpandData					拡張データ
	 * \param expandDataSize				拡張データサイズ
	 * \param colorBufferFormats			レンダーターゲットのフォーマット
	 * \param samplerFilter					サンプラーフィルター
	 *********************************************************************/
	void CreateMeshFromRes(
		const ResMesh& meshRes,
		uint32_t meshNo,
		uint32_t& materialNum,
		const wchar_t* pVSShaderPath,
		const wchar_t* pPSShaderPath,
		void* pExpandData,
		int expandDataSize,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);

	/*****************************************************************//**
	 * \brief MDメッシュからメッシュを作成1
	 * 
	 * \param mdUnitMesh			対象のメッシュ
	 * \param meshNo				メッシュ番号
	 * \param materialNo			マテリアル番号
	 * \param pShaderFilePath		シェーダーファイルパス
	 * \param pVSEntryPointFunc		頂点シェーダーのエントリーポイント
	 * \param pPSEntryPointFunc		ピクセルシェーダーのエントリーポイント
	 * \param colorBufferFormats	カラーターゲットのフォーマット
	 * \param samplerFilter			サンプラーフィルター
	 *********************************************************************/
	void CreateMeshFromMDMesh(
		const mdFile::MD_UnitMesh& mdUnitMesh,
		int meshNo,
		int& materialNo,
		const char* pShaderFilePath,
		const char* pVSEntryPointFunc,
		const char* pPSEntryPointFunc,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);

	/*****************************************************************//**
	 * \brief 描画の共通処理
	 * 
	 * \param rc		レンダリングコンテキスト
	 * \param worldMtx	ワールド変換行列
	 * \param viewMtx	ビュー変換行列
	 * \param projMtx	プロジェクション変換行列
	 *********************************************************************/
	void DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
						
	//!}   
};

