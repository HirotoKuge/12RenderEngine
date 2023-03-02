/*****************************************************************//**
 * \file   Model.cpp
 * \brief  ���f���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "Model.h"
#include <d3dcompiler.h>
#include "GraphicsEngine.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "../Util/FileUtil.h"
#include "../Util/Logger.h"


#pragma comment(lib, "d3dcompiler.lib")

void Model::Init(const ModelInitData& initData){
	
	m_meshParts.InitFromFile(
		initData.pPathMD,
		initData.pVSShaderPath,
		initData.pPSShaderPath,
		initData.pExpandConstantBuffer,
		initData.expandConstantBufferSize,
		initData.pExpandShaderResoruceViews,
		initData.colorBufferFormats,
		initData.m_samplerFilter
	);

	UpdateWorldMatrix(g_vec3Zero, g_quatIdentity, g_vec3One);

	m_isInited = true;

}


void Model::UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale){
	m_worldMtx = CalcWorldMatrix(pos, rot, scale);
}

void Model::Draw(RenderContext& rc){
	//3D�J�����擾
	auto pCamara3D = GraphicsEngine::GetInstance()->GetCamera3D();
	
	m_meshParts.Draw(
		rc,
		m_worldMtx,
		pCamara3D->GetViewMatrix(),
		pCamara3D->GetProjectionMatrix()
	);
}
void Model::Draw(RenderContext& rc, Camera& camera){
	Draw(rc, camera.GetViewMatrix(), camera.GetProjectionMatrix());
}

void Model::Draw(RenderContext& rc, const Matrix& viewMatrix, const Matrix& projMatrix){
	m_meshParts.Draw(
		rc,
		m_worldMtx,
		viewMatrix,
		projMatrix
	);
}

void Model::DrawInstancing(RenderContext& rc, int numInstance){
	//3D�J�����擾
	auto pCamara3D = GraphicsEngine::GetInstance()->GetCamera3D();

	// �C���X�^���X�̐���0�ȏ�Ȃ�`��B
	m_meshParts.DrawInstancing(
		rc,
		numInstance,
		m_worldMtx,
		pCamara3D->GetViewMatrix(),
		pCamara3D->GetProjectionMatrix()
	);
}