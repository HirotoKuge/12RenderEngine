/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  �J�����N���X
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "Camera.h"
#include "GraphicsEngine.h"



//=============================================================================
// �X�V����
//=============================================================================
void Camera::Update(){
	//�A�X�y�N�g����v�Z���� 
	m_aspect = (float)GraphicsEngine::GetInstance()->GetFrameBufferWidth() / (float)GraphicsEngine::GetInstance()->GetFrameBufferHeight();
	if (m_isNeedUpdateProjectionMatrix) {
		if (m_updateProjMatrixFunc == enUpdateProjMatrixFunc_Perspective) {
			//�����ϊ��s����v�Z 
			m_projectionMatrix.MakeProjectionMatrix(
				m_viewAngle,
				m_aspect,
				m_near,
				m_far
			);
		}
		else {
			//���s���e�s����v�Z 
			m_projectionMatrix.MakeOrthoProjectionMatrix(m_width, m_height, m_near, m_far);
		}
	}
	//�r���[�s��̎Z�o
	m_viewMatrix.MakeLookAt(m_position, m_target, m_up);
	//�r���[�v���W�F�N�V�����s��̍쐬 
	m_viewProjectionMatrix = m_viewMatrix * m_projectionMatrix;
	//�r���[�s��̋t�s����v�Z 
	m_viewMatrixInv.Inverse(m_viewMatrix);

	m_forward.Set(m_viewMatrixInv.m[2][0], m_viewMatrixInv.m[2][1], m_viewMatrixInv.m[2][2]);
	m_right.Set(m_viewMatrixInv.m[0][0], m_viewMatrixInv.m[0][1], m_viewMatrixInv.m[0][2]);
	//�J�����̉�]�s����擾 
	m_cameraRotation = m_viewMatrixInv;
	m_cameraRotation.m[3][0] = 0.0f;
	m_cameraRotation.m[3][1] = 0.0f;
	m_cameraRotation.m[3][2] = 0.0f;
	m_cameraRotation.m[3][3] = 1.0f;

	Vector3 toPos;
	toPos.Subtract(m_position, m_target);
	m_targetToPositionLen = toPos.Length();

	m_isDirty = false;
}

//=============================================================================
// ���[���h���W����X�N���[�����W���v�Z
//=============================================================================
void Camera::CalcScreenPositionFromWorldPosition(Vector2& screenPos, const Vector3& worldPos) const{
	float half_w = (float)GraphicsEngine::GetInstance()->GetFrameBufferWidth() * 0.5f;
	float half_h = (float)GraphicsEngine::GetInstance()->GetFrameBufferHeight() * 0.5f;
	Vector4 _screenPos;
	_screenPos.Set(worldPos.x, worldPos.y, worldPos.z, 1.0f);
	m_viewProjectionMatrix.Apply(_screenPos);
	screenPos.x = (_screenPos.x / _screenPos.w) * half_w;
	screenPos.y = (_screenPos.y / _screenPos.w) * half_h;
}

//=============================================================================
// �����_�����_�Ƃ��ăJ��������]
//=============================================================================
void Camera::RotateOriginTarget(const Quaternion& qRot){
	Vector3 cameraPos = m_position;
	Vector3 cameraTarget = m_target;
	Vector3 toPos = cameraPos - cameraTarget;
	qRot.Apply(toPos);
	m_position = m_target + toPos;
	m_isDirty = true;
}