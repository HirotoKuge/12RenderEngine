/*****************************************************************//**
 * \file   PipelineState.cpp
 * \brief  �O���t�B�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�N���X
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
// �f�X�g���N�^
//=============================================================================
PipelineState::~PipelineState(){ 
	m_pPipelineState->Release();
};


//=============================================================================
// �������ݒ肩�珉����
//=============================================================================
void PipelineState::Init(D3D12_GRAPHICS_PIPELINE_STATE_DESC desc){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto hr = pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr)) {
		MessageBoxA(nullptr, "�p�C�v���C���X�e�[�g�̍쐬�Ɏ��s���܂���\n", "�G���[", MB_OK);
		std::abort();
	}
}
void PipelineState::Init(D3D12_COMPUTE_PIPELINE_STATE_DESC desc){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto hr = pDevice->CreateComputePipelineState(&desc, IID_PPV_ARGS(&m_pPipelineState));
	if (FAILED(hr)) {
		MessageBoxA(nullptr, "�p�C�v���C���X�e�[�g�̍쐬�Ɏ��s���܂���\n", "�G���[", MB_OK);
		std::abort();
	}
}
