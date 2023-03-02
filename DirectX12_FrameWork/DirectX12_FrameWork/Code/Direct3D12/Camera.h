/*****************************************************************//**
 * \file   Camera.h
 * \brief  �J�����N���X
 * 
 * \author Hiroto Kuge
 * \date   December 2022
 *********************************************************************/
#pragma once

//=============================================================================
// Includes.
//=============================================================================
#include "../Util/Matrix.h"
#include "../Util/Vector.h"
#include <memory>

//=============================================================================
// CameraClass
//=============================================================================
class Camera {
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
	//-----------------------------------------------------------------------------	
	//!{
			/* Noting*/
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
public:
	/// <summary>
	/// �ˉe�s��̍X�V���@ 
	/// </summary>
	
	//=============================================================================
	// �v���W�F�N�V�����s��̍X�V���@
	//=============================================================================
	enum EnUpdateProjMatrixFunc {
		enUpdateProjMatrixFunc_Perspective,		// �����ˉe�s��F���ߖ@���������G����肽���Ȃ炱����
		enUpdateProjMatrixFunc_Ortho,			// ���s���e�F�Q�c�I�ȕ\�����������Ȃ炱����
	};
	
	/*****************************************************************//**
	 * \brief �X�V����
	 *********************************************************************/
	void Update();
	
	/*****************************************************************//**
	 * \brief �����_�����_�Ƃ��ăJ��������]
	 * 
	 * \param qRot ��]�������N�I�[�^�j�I��
	 *********************************************************************/
	void RotateOriginTarget(const Quaternion& qRot);

	/*****************************************************************//**
	 * \brief �J�����𓮂���
	 * 
	 * \param move �ړ���
	 *********************************************************************/
	void Move(const Vector3& move){
		m_position += move;
		m_target += move;
		m_isDirty = true;
	}

	/*****************************************************************//**
	 * \brief �����X�𓮂���
	 * 
	 * \param move �ړ���
	 *********************************************************************/
	void MoveTarget(const Vector3& move){
		m_target += move;
		m_isDirty = true;
	}
	
	/*****************************************************************//**
	 * \brief �J�����̍��W�𓮂���
	 * 
	 * \param move �ړ���
	 *********************************************************************/
	void MovePosition(const Vector3& move){
		m_position += move;
		m_isDirty = true;
	}
	
	/*****************************************************************//**
	 * \brief �J�����̑O�����Ɉړ�
	 * 
	 * \param moveForward �ړ���
	 *********************************************************************/
	void MoveForward(float moveForward){
		Move(m_forward * moveForward);
	}
	
	/*****************************************************************//**
	 * \brief �J�����̉E�����Ɉړ�
	 * \param moveRight
	 *********************************************************************/
	void MoveRight(float moveRight){
		Move(m_right * moveRight);
	}
	
	/*****************************************************************//**
	 * \brief �J�����̏�����Ɉړ�
	 * 
	 * \param moveUp �ړ���
	 *********************************************************************/
	void MoveUp(float moveUp){
		Move(m_up * moveUp);
	}
	
	/*****************************************************************//**
	 * \brief �J�����̍��W��ύX
	 * 
	 * \param pos �ݒ���W
	 *********************************************************************/
	void SetPosition(const Vector3& pos){
		m_position = pos;
		m_isDirty = true;
	}
	void SetPosition(float x, float y, float z){
		SetPosition({ x, y, z });
	}

	/*****************************************************************//**
	 * \brief �J�������W���擾
	 * 
	 * \return ���W 
	 *********************************************************************/
	const Vector3& GetPosition() const{
		return m_position;
	}
	
	/*****************************************************************//**
	 * \brief �����_��ݒ�
	 * 
	 * \param target �����X
	 *********************************************************************/
	void SetTarget(const Vector3& target){
		m_target = target;
		m_isDirty = true;
	}
	void SetTarget(float x, float y, float z){
		SetTarget({ x, y, z });
	}

	/*****************************************************************//**
	 * \brief �����_���擾
	 *  
	 * \return �����_
	 *********************************************************************/
	const Vector3& GetTarget() const{
		return m_target;
	}

	/*****************************************************************//**
	 * \brief �J�����̏������ݒ�
	 * \param up �ݒ�l
	 *********************************************************************/
	void SetUp(const Vector3& up){
		m_up = up;
		m_up.Normalize();
	}
	void SetUp(float x, float y, float z)
	{
		SetUp({ x, y, z });
	}

	/*****************************************************************//**
	 * \brief �J�����̏�������擾
	 * 
	 * \return �J�����̏����
	 *********************************************************************/
	const Vector3& GetUp() const{
		return m_up;
	}

	/*****************************************************************//**
	 * \brief �r���[�ϊ��s��̋t�s����擾
	 * 
	 * \return �r���[�ϊ��s��̋t�s��
	 *********************************************************************/
	const Matrix& GetViewMatrixInv(){
		if (m_isDirty) {
			//�X�V����K�v������
			Update();
		}
		return m_viewMatrixInv;
	}

	/*****************************************************************//**
	 * \brief �r���[�ϊ��s����擾
	 *  
	 * \return �r���[�ϊ��s��
	 *********************************************************************/
	const Matrix& GetViewMatrix(){
		if (m_isDirty) {
			//�X�V����K�v������
			Update();
		}
		return m_viewMatrix;
	}

	/*****************************************************************//**
	 * \brief �v���W�F�N�V�����ϊ��s����擾
	 * 
	 * \return �v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	const Matrix& GetProjectionMatrix(){
		if (m_isDirty) {
			//�X�V����K�v������
			Update();
		}
		return m_projectionMatrix;
	}

	/*****************************************************************//**
	 * \brief �r���[�~�v���W�F�N�V�����ϊ��s����擾
	 * 
	 * \return �r���[�~�v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	const Matrix& GetViewProjectionMatrix()	{
		if (m_isDirty) {
			//�X�V����K�v������
			Update();
		}
		return m_viewProjectionMatrix;
	}
	
	/*****************************************************************//**
	 * \brief �J�����̕ϊ��s����擾
	 * 
	 * \return �J�����̕ϊ��s��
	 *********************************************************************/
	const Matrix& GetCameraRotation(){
		if (m_isDirty) {
			//�X�V����K�v������
			Update();
		}
		return m_cameraRotation;
	}
	
	/*****************************************************************//**
	 * \brief �t�@�[�N���b�v(�����ʂ܂ł̋���)��ݒ�
	 * 
	 * \param fFar �ݒ�l
	 *********************************************************************/
	void SetFar(float fFar){
		m_far = fFar;
		m_isDirty = true;
	}
	
	/*****************************************************************//**
	 * \brief �j�A�N���b�v(�ߕ��ʂ܂ł̋���)��ݒ�
	 * 
	 * \param fNear �ݒ�l
	 *********************************************************************/
	void SetNear(float fNear){
		m_near = fNear;
		m_isDirty = true;
	}

	/*****************************************************************//**
	 * \brief �t�@�[�N���b�v(�����ʂ܂ł̋���)���擾
	 * 
	 * \return �t�@�[�N���b�v 
	 *********************************************************************/
	float GetFar() const{
		return m_far;
	}

	/*****************************************************************//**
	 * \brief �j�A�N���b�v(�ߕ��ʂ܂ł̋���)���擾
	 * 
	 * \return �j�A�N���b�v 
	 *********************************************************************/
	float GetNear() const{
		return m_near;
	}

	/*****************************************************************//**
	 * \brief �v���W�F�N�V�����̕���ݒ�
	 * 
	 * \param w �ݒ�l
	 * \memo �v���W�F�N�V�����s��̍X�V���@�ŕ��s���e���ݒ肳��Ă���Ƃ��Ɏg�p�����
	 *********************************************************************/
	void SetWidth(float w){
		m_width = w;
		m_isDirty = true;
	}
	
	/*****************************************************************//**
	 * \brief �v���W�F�N�V�����ϊ��s��̍�����ݒ�
	 * 
	 * \param h �ݒ�l
	 * \memo �v���W�F�N�V�����s��̍X�V���@�ŕ��s���e���ݒ肳��Ă���Ƃ��Ɏg�p�����
	 *********************************************************************/
	void SetHeight(float h){
		m_height = h;
		m_isDirty = true;
	}

	/*****************************************************************//**
	 * \brief ���s���e�̕����擾
	 * 
	 * \return ���s���e�̕�
	 *********************************************************************/
	float GetWidth() const{
		return m_width;
	}

	/*****************************************************************//**
	 * \brief ���s���e�̍������擾
	 * 
	 * \return ���s���e�̍���
	 *********************************************************************/
	float GetHeight() const{
		return m_height;
	}
	
	/*****************************************************************//**
	 * \brief �v���W�F�N�V�����ϊ��̍X�V���@��ݒ�
	 * 
	 * \param func �X�V���@
	 *********************************************************************/
	void SetUpdateProjMatrixFunc(EnUpdateProjMatrixFunc func){
		m_updateProjMatrixFunc = func;
		m_isDirty = true;
	}

	/*****************************************************************//**
	 * \brief ��p��ݒ�
	 * 
	 * \param viewAngle ��p(Radian)
	 *********************************************************************/
	void SetViewAngle(float viewAngle){
		m_viewAngle = viewAngle;
		m_isDirty = true;
	}
	
	/*****************************************************************//**
	 * \brief ��p���擾
	 * 
	 * \return ��p(Radian)
	 *********************************************************************/
	float GetViewAngle() const{
		return m_viewAngle;
	}
	
	/*****************************************************************//**
	 * \brief �����_�ƃJ�������W�̋������擾
	 * 
	 * \return ����
	 *********************************************************************/
	float GetTargetToPositionLength() const{
		return m_targetToPositionLen;
	}
	
	/*****************************************************************//**
	 * \brief �J�����̑O�������擾
	 * 
	 * \return �J�����̑O����
	 *********************************************************************/
	const Vector3& GetForward() const{
		return m_forward;
	}
	
	/*****************************************************************//**
	 * \brief �J�����̉E�������擾
	 * 
	 * \return �J�����̉E����
	 *********************************************************************/
	const Vector3& GetRight() const{
		return m_right;
	}

	/*****************************************************************//**
	 * \brief �A�X�y�N�g����擾
	 * 
	 * \return �A�X�y�N�g��
	 *********************************************************************/
	float GetAspect() const{
		return m_aspect;
	}

	/*****************************************************************//**
	 * \brief �J�������R�s�[����
	 * 
	 * \param dst �R�s�[��
	 *********************************************************************/
	void CopyTo(Camera& dst){
		memcpy(&dst, this, sizeof(dst));
		m_isDirty = true;
	}

	/*****************************************************************//**
	 * \brief ���[���h���W����X�N���[�����W���v�Z
	 * 
	 * \param screenPos �X�N���[�����W�̊i�[��
	 * \param worldPos  ���[���h���W
	 * 
	 * \memo �v�Z�����X�N���[�����W�͉�ʂ̒��S��{0,0}
	 *		 �����{��ʂ̕�*-0.5,��ʂ̍���*-0.5}
	 *		 �E����{ ��ʂ̕� * 0.5,��ʂ̍��� * 0.5 }�Ƃ�����W�n
	 *********************************************************************/
	void CalcScreenPositionFromWorldPosition(Vector2& screenPos, const Vector3& worldPos) const;
	//!} 
	//!{ 
protected:
	//-----------------------------------------------------------------------------
	// protected variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!}
	//-----------------------------------------------------------------------------
	// protected methods.
	//----------------------------------------------------------------------------- 
	//!{
	
	float		m_targetToPositionLen = 1.0f;			// �����_�Ǝ��_�܂ŋ��� 
	Vector3		m_position = { 0.0f, 0.0f, 1.0f };		// �J�����ʒu 
	Vector3		m_up = g_vec3Up;						// �J�����̏���� 
	Vector3		m_target;								// �J�����̒��~�_ 
	Matrix		m_viewMatrix;							// �r���[�s�� 
	Matrix		m_projectionMatrix;						// �v���W�F�N�V�����s�� 
	Matrix		m_viewProjectionMatrix;					// �r���[�v���W�F�N�V�����s�� 
	Matrix		m_viewMatrixInv;						// �r���[�s��̋t�s�� 
	Matrix		m_cameraRotation;						// �J�����̉�]�s�� 
	Vector3		m_forward = g_vec3Front;				// �J�����̑O�� 
	Vector3		m_right = g_vec3Right;					// �J�����̉E 
	float		m_near = 1.0f;							// �ߕ��� 
	float		m_far = 5000.0f;						// ������ 
	float		m_viewAngle = Math::DegToRad(60.0f);	// ��p(���W�A��) 
	float		m_aspect = 1.0f;						// �A�X�y�N�g�� 
	float		m_width = 1280.0f;						// ���s���e�s����쐬����Ƃ��Ɏg�p����镝 
	float		m_height = 720.0f;						// ���s���e�s����쐬����Ƃ��Ɏg�p����鍂�� 
	bool		m_isNeedUpdateProjectionMatrix = true;	// �v���W�F�N�V�����ϊ��s��̍X�V���K�v���ǂ���
	bool		m_isDirty = false;						// �_�[�e�B�t���O 

	EnUpdateProjMatrixFunc m_updateProjMatrixFunc = enUpdateProjMatrixFunc_Perspective;	//�v���W�F�N�V�����s��̍X�V���@
	
																						//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
			/* Nothing */
	//!} 
};