/*****************************************************************//**
 * \file   Model.h
 * \brief  ���f���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Include 
//=============================================================================
#include "Mesh.h"

//=============================================================================
// Forword Decolations 
//=============================================================================
class ShaderResource;

//=============================================================================
// Model InitData
//=============================================================================
struct ModelInitData {	
	const wchar_t* pPathMD;	// ���f���f�[�^�̃��\�[�X�t�@�C���p�X
	const wchar_t* pPathMT;	// �}�e���A���f�[�^�̃��\�[�X�t�@�C���p�X

	const wchar_t* pVSShaderPath;	// ���_�V�F�[�_�[�̃t�@�C���p�X
	const wchar_t* pPSShaderPath;	// �s�N�Z���V�F�[�_�[�̃t�@�C���p�X

	void* pExpandConstantBuffer = nullptr;	//�@���[�U�[�g���̒萔�o�b�t�@
	int expandConstantBufferSize = 0;		//�@���[�U�[�g���̒萔�o�b�t�@�̃T�C�Y
											//�@�C���X�^���V���O�`��̍��ق͂��̃p�����[�^�ɍő�̃C���X�^���X�����w��

	std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV> pExpandShaderResoruceViews = { nullptr };	//���[�U�[�g���̃V�F�[�_�[���\�[�X

	D3D12_FILTER m_samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	//�e�N�X�`���T���v�����O�̃t�B���^
	std::array<DXGI_FORMAT, MAX_RENDERING_TARGET> colorBufferFormats = {
		DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
		DXGI_FORMAT_UNKNOWN,
	};	//�����_�����O����J���[�o�b�t�@�̃t�H�[�}�b�g
};

//=============================================================================
// Model Class
//=============================================================================
class Model{
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
	//-----------------------------------------------------------------------------	
	//!{
			/* Nothing */
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
	 

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~Model(){};


	/*****************************************************************//**
	 * \brief ������
	 * 
	 * \param initData �������f�[�^
	 *********************************************************************/
	void Init( const ModelInitData& initData );
	
	
	/*****************************************************************//**
	 * \brief ���[���h�ϊ��s����X�V
	 * 
	 * \param pos	���W
	 * \param rot	��]
	 * \param scale	�T�C�Y
	 *********************************************************************/
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/*****************************************************************//**
	 * \brief �`��
	 * 
	 * \param renderContext �����_�����O�R���e�L�X�g
	 *********************************************************************/
	void Draw(RenderContext& renderContext);
	
	
	/*****************************************************************//**
	 * \brief �J�����w��`��
	 * 
	 * \param renderContext �����_�����O�R���e�L�X�g
	 * \param camera		�J����
	 *********************************************************************/
	void Draw(RenderContext& renderContext, Camera& camera);
	
	/*****************************************************************//**
	 * \brief �J�����s��w��`��
	 * 
	 * \param renderContext	�����_�����O�R���e�L�X�g
	 * \param viewMatrix	�r���[�ϊ��s��
	 * \param projMatrix	�v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	void Draw(RenderContext& renderContext, const Matrix& viewMatrix, const Matrix& projMatrix);
	
	/*****************************************************************//**
	 * \brief �C���X�^���V���O�`��
	 * 
	 * \param renderContext	�����_�����O�R���e�L�X�g
	 * \param numInstance	�C���X�^���X��
	 *********************************************************************/
	void DrawInstancing(RenderContext& renderContext, int numInstance);
	
	/*****************************************************************//**
	 * \brief ���[���h�ϊ��s����擾
	 * 
	 * \return ���[���h�ϊ��s��
	 *********************************************************************/
	const Matrix& GetWorldMatrix() const{
		return m_worldMtx;
	}
	
	/*****************************************************************//**
	 * \brief ����������Ă��邩���m�F
	 * 
	 * \return ����������Ă邩
	 *********************************************************************/
	bool IsInited() const{
		return m_isInited;
	}
	
	/*****************************************************************//**
	 * \brief ���[���h�ϊ��s����v�Z
	 * 
	 * \param pos	  ���W
	 * \param rot	  ��]
	 * \param scale	  �T�C�Y
	 * \return �v�Z����
	 *********************************************************************/
	
Matrix CalcWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale){
		Matrix worldMtx;
	
		Matrix transMtx, rotMtx,scaleMtx;
		transMtx.MakeTranslation(pos);
		rotMtx.MakeRotationFromQuaternion(rot);
		scaleMtx.MakeScaling(scale);

		worldMtx = scaleMtx * rotMtx * transMtx;
		return worldMtx;
	}


	//!} 
private:	
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	bool m_isInited = false; // ����������Ă��邩�ǂ���
	Matrix m_worldMtx;		 // ���[���h�s��
	Mesh m_meshParts;		 // ���b�V���p�[�c

	//TODO:����{�[�����Ƃ��ǉ��ł���Ƃ�����
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	 
	//!} 
};