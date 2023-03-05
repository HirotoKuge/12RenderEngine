/*****************************************************************//**
 * \file   PipelineState.cpp
 * \brief  グラフィクスパイプラインステートオブジェクトクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes.
//=============================================================================
#include "../stdafx.h"
#include "PipelineState.h"

//=============================================================================
// デストラクタ
//=============================================================================
PipelineState::~PipelineState(){ 
	m_pPipelineState->Release();
};

//=============================================================================
// 初期化処理
//=============================================================================
void PipelineState::Init(
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc,
	const wchar_t* pVsPath,
	const wchar_t* pPsPath,
	ID3D12RootSignature* pRootSignature
){
	//引数チェック
	if (pVsPath == nullptr || pPsPath == nullptr||pRootSignature == nullptr) {
		ELOG("Erorr : Invalid Argument ");
	}

	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	
	std::wstring vsPath;
	std::wstring psPath;

	// 頂点シェーダを検索.
	if (!SearchFilePath(pVsPath, vsPath)) {
		ELOG("Error : Vertex Shader Not Found.");
	}

	// ピクセルシェーダを検索.
	if (!SearchFilePath(pPsPath, psPath)) {
		ELOG("Error : Pixel Shader Node Found.");
	}

	ComPtr<ID3DBlob> pVSBlob;
	ComPtr<ID3DBlob> pPSBlob;

	// 頂点シェーダを読み込む.
	auto hr = D3DReadFileToBlob(vsPath.c_str(), pVSBlob.GetAddressOf());
	if (FAILED(hr)) {
		ELOG("Error : D3DReadFiledToBlob() Failed. path = %ls", vsPath.c_str());
	}

	// ピクセルシェーダを読み込む.
	hr = D3DReadFileToBlob(psPath.c_str(), pPSBlob.GetAddressOf());
	if (FAILED(hr)) {
		ELOG("Error : D3DReadFileToBlob() Failed. path = %ls", psPath.c_str());
	}

	// 各種設定
	desc.pRootSignature = pRootSignature;
	desc.VS = CD3DX12_SHADER_BYTECODE(pVSBlob.Get());
	desc.PS = CD3DX12_SHADER_BYTECODE(pPSBlob.Get());
	
	// パイプラインステートを生成.
	hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_pPipelineState.GetAddressOf()));
	if (FAILED(hr)){
		ELOG("Error : ID3D12Device::CreateGraphicsPipelineState() Failed. retcode = 0x%x", hr);
	}
}
