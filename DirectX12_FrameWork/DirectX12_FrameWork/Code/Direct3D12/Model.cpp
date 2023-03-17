/*****************************************************************//**
 * \file   Model.cpp
 * \brief  モデルクラス
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
// 初期化処理
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
// ワールド変換行列を更新
//=============================================================================
void Model::UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale){
	m_worldMtx = CalcWorldMatrix(pos, rot, scale);
}

//=============================================================================
// 描画処理
//=============================================================================
//!{
void Model::Draw(RenderContext& rc){
	//3Dカメラ取得
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
	//3Dカメラ取得
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
// インスタンシング描画
//=============================================================================
void Model::DrawInstancing(RenderContext& rc, int numInstance){
	//3Dカメラ取得
	auto pCamara3D =GraphicsEngine::GetInstance()->GetCamera3D();

	// インスタンスの数が0以上なら描画
	m_meshParts.DrawInstancing(
		rc,
		numInstance,
		m_worldMtx,
		pCamara3D->GetViewMatrix(),
		pCamara3D->GetProjectionMatrix()
	);
}