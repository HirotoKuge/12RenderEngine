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
#include "../stdafx.h"
#include "Model.h"
#include "Material.h"

//=============================================================================
// ����������
//=============================================================================
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

//=============================================================================
// ���[���h�ϊ��s����X�V
//=============================================================================
void Model::UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale){
	m_worldMtx = CalcWorldMatrix(pos, rot, scale);
}

//=============================================================================
// �`�揈��
//=============================================================================
//!{
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

void Model::Draw(RenderContext& rc, Matrix& worldMtx){
	//3D�J�����擾
	auto pCamara3D = GraphicsEngine::GetInstance()->GetCamera3D();

	m_meshParts.Draw(
		rc,
		worldMtx,
		pCamara3D->GetViewMatrix(),
		pCamara3D->GetProjectionMatrix()
	);
}

void Model::Draw(RenderContext& rc, const Matrix& viewMatrix, const Matrix& projMatrix){
	m_meshParts.Draw(
		rc,
		m_worldMtx,
		viewMatrix,
		projMatrix
	);
}

//!} 

//=============================================================================
// �C���X�^���V���O�`��
//=============================================================================
void Model::DrawInstancing(RenderContext& rc, int numInstance){
	//3D�J�����擾
	auto pCamara3D =GraphicsEngine::GetInstance()->GetCamera3D();

	// �C���X�^���X�̐���0�ȏ�Ȃ�`��
	m_meshParts.DrawInstancing(
		rc,
		numInstance,
		m_worldMtx,
		pCamara3D->GetViewMatrix(),
		pCamara3D->GetProjectionMatrix()
	);
}