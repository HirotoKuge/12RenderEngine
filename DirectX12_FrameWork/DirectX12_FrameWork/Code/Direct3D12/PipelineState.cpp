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
// 初期化設定から初期化
//=============================================================================
void PipelineState::Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr)) {
		MessageBoxA(nullptr, "パイプラインステートの作成に失敗しました\n", "エラー", MB_OK);
		std::abort();
	}
}
void PipelineState::Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto hr = pDevice->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr)) {
		MessageBoxA(nullptr, "パイプラインステートの作成に失敗しました\n", "エラー", MB_OK);
		std::abort();
	}
}
