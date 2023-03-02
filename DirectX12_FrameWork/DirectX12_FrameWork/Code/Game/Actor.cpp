/*****************************************************************//**
 * \file   Actor.cpp
 * \brief  アクタークラス
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "Actor.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Game::Object::Actor::Actor()
{/* Do Nothing */ }

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Game::Object::Actor::~Actor()
{ Term(); }

//-----------------------------------------------------------------------------
// 初期化処理
//-----------------------------------------------------------------------------
bool Game::Object::Actor::Init() {
	return true;
}

//-----------------------------------------------------------------------------
// 更新処理
//-----------------------------------------------------------------------------
void Game::Object::Actor::Update(float dt) {
}

//-----------------------------------------------------------------------------
// 終了処理
//-----------------------------------------------------------------------------
void Game::Object::Actor::Term() {
	m_pSceneDI.reset();
}

//-----------------------------------------------------------------------------
// ワールド変換行列を更新
//-----------------------------------------------------------------------------
void Game::Object::Actor::UpdateWorldMtx() {
	Matrix scaleMtx;	//拡大縮小行列
	Matrix rotMtx;		//回転行列
	Matrix transMtx;	//平行移動行列

	scaleMtx.MakeScaling(m_scale);
	rotMtx.MakeRotationFromQuaternion(m_quaternion);
	transMtx.MakeTranslation(m_position);

	m_worldMtx.Multiply(m_worldMtx.Identity,scaleMtx);
	m_worldMtx *= rotMtx;
	m_worldMtx *= transMtx;
}


