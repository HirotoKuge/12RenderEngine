/*****************************************************************//**
 * \file   Material.cpp
 * \brief  マテリアルクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "Material.h"

//=============================================================================
// static member
//=============================================================================
const wchar_t* Material::DummyTag = L""; // ダミーテクスチャ用

//=============================================================================
// マテリアルの初期化
//=============================================================================
bool Material::Init(
	const ResMaterialPBR& resMaterial, 
	const wchar_t* p_vsFilePath, 
	const wchar_t* p_psFilePath, 
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat, 
	uint32_t numSrv, 
	uint32_t numCbv, 
	uint32_t offsetInDescriptorsFromTableStartCB, 
	uint32_t offsetInDescriptorsFromTableStartSRV, 
	D3D12_FILTER samplerFilter
){

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	auto pCommandQueue =GraphicsEngine::GetInstance()->GetCommandQueue();


	// テクスチャを初期化
	InitTexture(pDevice,pCommandQueue,resMaterial);

	// 定数バッファを作成
	PBR_Param param;
	param.Metallic = resMaterial.Matallic;
	param.Roughness = resMaterial.Roughness;
	m_materialCB.Init(sizeof(PBR_Param),&param);

	//ルートシグネチャを初期化
	D3D12_STATIC_SAMPLER_DESC samplerDescArray[2];
	
	//デフォルトのサンプラ-
	samplerDescArray[0].Filter = samplerFilter;
	samplerDescArray[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDescArray[0].MipLODBias = 0;
	samplerDescArray[0].MaxAnisotropy = 0;
	samplerDescArray[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDescArray[0].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	samplerDescArray[0].MinLOD = 0.0f;
	samplerDescArray[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDescArray[0].ShaderRegister = 0;
	samplerDescArray[0].RegisterSpace = 0;
	samplerDescArray[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	
	//シャドウマップ用のサンプラ-
	samplerDescArray[1] = samplerDescArray[0];
	
	//比較対象の値が小さければ０、大きければ１を返す比較関数を設定する
	samplerDescArray[1].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDescArray[1].ComparisonFunc = D3D12_COMPARISON_FUNC_GREATER;
	samplerDescArray[1].MaxAnisotropy = 1;
	samplerDescArray[1].ShaderRegister = 1;

	m_rootSignature.Init(
		samplerDescArray,
		2,
		numCbv,
		numSrv,
		8,
		offsetInDescriptorsFromTableStartCB,
		offsetInDescriptorsFromTableStartSRV
	);

	return true;
}

//=============================================================================
// 描画前処理
//=============================================================================
void Material::BeginRender(RenderContext& rc){
	rc.SetRootSignature(m_rootSignature);
	rc.SetPipelineState(m_pipelineState);
}

//=============================================================================
// パイプラインステートの初期化
//=============================================================================
void Material::InitPipelineState(const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats){
	// グラフィックスパイプラインステートを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
	
	desc.InputLayout		= MeshVertex::InputLayout;
	desc.pRootSignature		= m_rootSignature.Get();
	desc.VS					= D3D12_SHADER_BYTECODE(m_pVSShader->GetCompiledBlob());
	desc.PS					= D3D12_SHADER_BYTECODE(m_pPSShader->GetCompiledBlob());
	
	desc.RasterizerState			= CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);	// ラスタライザーはデフォルト
	desc.RasterizerState.CullMode	= D3D12_CULL_MODE_NONE;				// カリングはなし
	desc.BlendState					= CD3DX12_BLEND_DESC(D3D12_DEFAULT);				// ブレンドステートもデフォルト
	
	desc.DepthStencilState					= CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);	// 深度ステンシルはデフォルトを使う
	desc.DepthStencilState.DepthEnable		= TRUE;
	desc.DepthStencilState.DepthWriteMask	= D3D12_DEPTH_WRITE_MASK_ALL;
	desc.DepthStencilState.DepthFunc		= D3D12_COMPARISON_FUNC_LESS_EQUAL;
	desc.DepthStencilState.StencilEnable	= FALSE;
	
	desc.SampleMask = UINT_MAX;
	
	desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;// 三角形を描画

	int numRenderTarget = 0;
	for (auto& format : colorBufferFormats) {
		if (format == DXGI_FORMAT_UNKNOWN) {
			//フォーマットが指定されていない場所が来たら終わり
			break;
		}
		desc.RTVFormats[numRenderTarget] = colorBufferFormats[numRenderTarget];
		numRenderTarget++;
	}
	desc.NumRenderTargets = numRenderTarget;
	desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;

	m_pipelineState.Init(desc);

}

//=============================================================================
// シェーダーを初期化
//=============================================================================
void Material::InitShader(const wchar_t* pVSShaderPath, const wchar_t* pPSShaderPath){
	m_pVSShader = new Shader();
	m_pVSShader->LoadVScso(pVSShaderPath);

	m_pPSShader = new Shader();
	m_pPSShader->LoadPScso(pPSShaderPath);

}
void Material::InitShaders(
	const char* fxFilePath,
	const char* vsEntryPointFunc,
	const char* vsSkinEntriyPointFunc,
	const char* psEntryPointFunc
){
	//シェーダーをロードする
	m_pVSShader->LoadVS("Assets/shader/sample.fx", "VSMain");
	m_pPSShader->LoadPS("Assets/shader/sample.fx", "PSMain");
}

//=============================================================================
// テクスチャ初期化
//=============================================================================
void Material::InitTexture(
	ID3D12Device5* pDevice,
	ID3D12CommandQueue* pCommandQueue,
	const ResMaterialPBR& resMaterial){


	//アルベドマップ
	{
		// 該当ファイルが存在しているなら
		if (resMaterial.AlbedMapFileName != L"") {
			// テクスチャをロードする
			// TODO:.dds以外にも対応させたい
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO]->
				InitFromDDSFile(resMaterial.AlbedMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO] = nullptr;
		}
	}	
	// メタルネスマップ
	{
		// 該当ファイルが存在しているなら
		if (resMaterial.MatalnessMapFileName != L"") {
			// テクスチャをロードする
			// TODO:.dds以外にも対応させたい
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC]->
				InitFromDDSFile(resMaterial.MatalnessMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::METALLIC] = nullptr;
		}
	}
	// ラフネスマップ
	{
		// 該当ファイルが存在しているなら
		if (resMaterial.RoughnessMapFileName != L"") {
			// テクスチャをロードする
			// TODO:.dds以外にも対応させたい
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS]->
				InitFromDDSFile(resMaterial.RoughnessMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::ROUGHNESS ] = nullptr;
		}
	}
	// ノーマルマップ
	{
		// 該当ファイルが存在しているなら
		if (resMaterial.NormalMapFileName != L"") {
			// テクスチャをロードする
			// TODO:.dds以外にも対応させたい
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL]->
				InitFromDDSFile(resMaterial.NormalMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL] = nullptr;
		}
	}
	// ハイトマップ
	{
		// 該当ファイルが存在しているなら
		if (resMaterial.HeightMapFileName != L"") {
			// テクスチャをロードする
			// TODO:.dds以外にも対応させたい
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT] = new Texture();
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT]->
				InitFromDDSFile(resMaterial.HeightMapFileName.c_str());
		}
		else {
			m_pTexturesPBR[TEXTURE_USAGE_PBR::HEIGHT] = nullptr;
		}
	}
	
}
