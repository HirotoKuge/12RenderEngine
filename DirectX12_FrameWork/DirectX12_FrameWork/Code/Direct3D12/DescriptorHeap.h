/*****************************************************************//**
 * \file   DescriptorHeap.h
 * \brief  ディスクリプタヒープクラス
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 * 
 * \memo   リソース系(CBV_SRV_UAV)かサンプラー用
 *********************************************************************/
#pragma once

//=============================================================================
// DescriptorHeap Class
//=============================================================================
class DescriptorHeap{
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
	DescriptorHeap();
	
	/*****************************************************************//**
	 * \brief デストラクタ
	 *********************************************************************/
	~DescriptorHeap();
	
	/*****************************************************************//**
	 * \brief ディスクリプタヒープ本体を取得
	 *  
	 * \return ディスクリプタヒープ
	 *********************************************************************/
	ID3D12DescriptorHeap* Get() const;

	/*****************************************************************//**
	 * \brief シェーダーリソース(テクスチャ)を登録
	 *  
	 * \param registerNo レジスタ番号
	 * \param sr		 シェーダーリソース
	 * 
	 * \memo レジスタ番号に-1を指定すると自動で次番号のレジスタが使用される
	 *********************************************************************/
	void RegistShaderResource(int registerNo, ShaderResource& sr){
		RegistResource(
			registerNo,
			&sr,
			&m_shaderResources.front(),
			m_numShaderResource,
			MAX_SHADER_RESOURCE,
			L"DescriptorHeap::RegistShaderResource() レジスタ番号が範囲外です"
		);
	}

	/*****************************************************************//**
	 * \brief アンオーダーアクセスリソースを登録
	 *  
	 * \param registerNo レジスタ番号
	 * \param ur		 アンオーダーリソース
	 * 
	 * \memo レジスタ番号に-1を指定すると自動で次番号のレジスタが使用される
	 *********************************************************************/
	void RegistUnorderAccessResource(int registerNo, UnorderAccessResrouce& ur){
		RegistResource(
			registerNo,
			&ur,
			&m_uavResources.front(),
			m_numUavResource,
			MAX_SHADER_RESOURCE,
			L"DescriptorHeap::RegistUnorderAccessResource() レジスタ番号が範囲外です"
		);
	}
	
	/*****************************************************************//**
	 * \brief 定数バッファをディスクリプタヒープに登録
	 *  
	 * \param registerNo レジスタ番号
	 * \param cb		 定数バッファ
	 * 
	 * \memo レジスタ番号に-1を指定すると自動で次番号のレジスタが使用される
	 *********************************************************************/
	void RegistConstantBuffer(int registerNo, ConstantBuffer& cb){
		RegistResource(
			registerNo,
			&cb,
			&m_constantBuffers.front(),
			m_numConstantBuffer,
			MAX_CONSTANT_BUFFER,
			L"DescriptorHeap::RegistConstantBuffer() レジスタ番号が範囲外です"
		);
	}

	/*****************************************************************//**
	 * \brief サンプラー定義をディスクリプタヒープに追加
	 *  
	 * \param registerNo レジスタ番号
	 * \param desc		 サンプラー定義
	 * 
	 * \memo レジスタ番号に-1を指定すると自動で次番号のレジスタが使用される
	 *********************************************************************/
	void RegistSamplerDesc(int registerNo, const D3D12_SAMPLER_DESC& desc){
		RegistResource(
			registerNo,
			desc,
			m_samplerDescs,
			m_numSamplerDesc,
			MAX_SAMPLER_STATE,
			L"DescriptorHeap::RegistSamplerDesc() レジスタ番号が範囲外です"
		);
	}
	
	/*****************************************************************//**
	 * \brief ヒープへの登録を確定
	 *********************************************************************/
	void Commit();

	/*****************************************************************//**
	 * \brief サンプラステート用のディスクリプタヒープへの登録
	 ********************************************************************/
	void CommitSamplerHeap();

	/*****************************************************************//**
	 * \brief 定数バッファのディスクリプタの開始ハンドルを取得
	 *  
	 * \return 定数バッファのディスクリプタの開始ハンドル
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief シェーダーリソースのディスクリプタの開始ハンドルを取得
	 *  
	 * \return シェーダーリソースのディスクリプタの開始ハンドル 
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceGpuDescriptorStartHandle() const;

	/*****************************************************************//**
	 * \brief アンオーダードアクセスリソースのディスクリプタの開始ハンドルを取得
	 *  
	 * \return アンオーダードアクセスリソースのディスクリプタの開始ハンドル 
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavResourceGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief サンプラーのディスクリプタの開始ハンドルを取得
	 *  
	 * \return サンプラーのディスクリプタの開始ハンドル
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerResourceGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief シェーダーリソースが1つでも登録されてるか確認
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistShaderResource() const{
		return m_numShaderResource != 0;
	}
	
	/*****************************************************************//**
	 * \brief 定数バッファが1つでも登録されているか判定
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistConstantBuffer() const{
		return m_numConstantBuffer != 0;
	}
	
	/*****************************************************************//**
	 * \brief UAVリソースが一つでも登録されているか判定
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistUavResource() const{
		return m_numUavResource != 0;
	}
	
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	enum {
		MAX_SHADER_RESOURCE = 1024 * 10,	//シェーダーリソースの最大数
		MAX_CONSTANT_BUFFER = 1024 * 10,	//定数バッファの最大数
		MAX_SAMPLER_STATE = 16,				//サンプラステートの最大数
	};

	int m_numShaderResource = 0;	//シェーダーリソースの数
	int m_numConstantBuffer = 0;	//定数バッファの数
	int m_numUavResource = 0;		//アンオーダーアクセスリソースの数
	int m_numSamplerDesc = 0;		//サンプラの数

	ID3D12DescriptorHeap*				m_descriptorHeap[2] = { nullptr };	//ディスクリプタヒープ

	std::vector<ShaderResource*>		m_shaderResources;					//シェーダーリソース
	std::vector<UnorderAccessResrouce*> m_uavResources;						//UAVリソース
	std::vector<ConstantBuffer*>		m_constantBuffers;					//定数バッファ
	
	D3D12_SAMPLER_DESC					m_samplerDescs[MAX_SAMPLER_STATE];	//サンプラステート
	D3D12_GPU_DESCRIPTOR_HANDLE			m_cbGpuDescriptorStart[2];			//定数バッファのディスクリプタヒープの開始ハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE			m_srGpuDescriptorStart[2];			//シェーダーリソースのディスクリプタヒープの開始ハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE			m_uavGpuDescriptorStart[2];			//UAVリソースのディスクリプタヒープの開始ハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE			m_samplerGpuDescriptorStart[2];		//Samplerのでスクリプタヒープの開始ハンドル
	
	uint32_t m_cbrSrvDescriptorSize = 0;						//CBR_SRVのディスクリプタのサイズ
	uint32_t m_samplerDescriptorSize = 0;
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief リソースをディスクリプタヒープに登録
	 *  
	 * \param registerNo	登録番号
	 * \param res			登録するリソース
	 * \param resTbl		リソーステーブル
	 * \param numRes		登録されているリソース数
	 * \param MAX_RESOURCE	リソースの最大数
	 * \param errorMessage	エラーメッセージ
	 *********************************************************************/
	template<class T>
	void RegistResource(
		int registerNo,
		T res,
		T resTbl[],
		int& numRes,
		const int MAX_RESOURCE,
		const wchar_t* errorMessage
	)
	{
		if (registerNo == -1) {
			//-1が指定されていたら、現在登録されている末尾のリソースの次に登録される
			registerNo = numRes;
		}
		if (registerNo < MAX_RESOURCE) {
			resTbl[registerNo] = res;
			if (numRes < registerNo + 1) {
				numRes = registerNo + 1;
			}
		}
		else {
			MessageBox(nullptr, errorMessage, L"エラー", MB_OK);
			std::abort();
		}
	}
	//!} 


};

