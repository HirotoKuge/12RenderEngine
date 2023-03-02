/*****************************************************************//**
 * \file   Model.h
 * \brief  ���f���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Includes
//=============================================================================
#include "MDLoader.h"
#include "GraphicsEngine.h"
#include "DescriptorHeap.h"
#include "Mesh.h"
#include "Material.h"
#include "ShaderResourse.h"
#include "../Util/Matrix.h"


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
	~Model();


	/*****************************************************************//**
	 * \brief ������
	 * 
	 * \param initData �������f�[�^
	 *********************************************************************/
	void Init( const ModelInitData& initData );
	
	/// <summary>
	/// ���[���h�s����v�Z���āA�����o�ϐ���m_worldMatrix���������񂷂�B
	/// </summary>
	/// <param name="pos">���W</param>
	/// <param name="rot">��]</param>
	/// <param name="scale">�g�嗦</param>
	void UpdateWorldMatrix(Vector3 pos, Quaternion rot, Vector3 scale);

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g</param>
	void Draw(RenderContext& renderContext);
	
	/// <summary>
	/// �`��(�J�����w���)
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g</param>
	/// <param name="camera">�J����</param>
	void Draw(RenderContext& renderContext, Camera& camera);
	/// <summary>
	/// �`��(�J�����s��w���)
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g</param>
	/// <param name="viewMatrix">�r���[�s��</param>
	/// <param name="projMatrix">�v���W�F�N�V�����s��</param>
	void Draw(RenderContext& renderContext, const Matrix& viewMatrix, const Matrix& projMatrix);
	/// <summary>
	/// �C���X�^���V���O�`��
	/// </summary>
	/// <param name="renderContext">�����_�����O�R���e�L�X�g</param>
	/// <param name="numInstance">�C���X�^���X�̐�</param>
	void DrawInstancing(RenderContext& renderContext, int numInstance);
	/// <summary>
	/// ���[���h�s����擾�B
	/// </summary>
	/// <returns></returns>
	const Matrix& GetWorldMatrix() const
	{
		return m_worldMtx;
	}
	

	/// <summary>
	/// ����������Ă��邩����B
	/// </summary>
	/// <returns></returns>
	bool IsInited() const
	{
		return m_isInited;
	}
	/// <summary>
	/// ���[���h�s����v�Z����B
	/// </summary>
	/// <remark>
	/// ���̊֐���UpdateWorldMatrix�֐��̒�����g���Ă��܂��B
	/// Model�N���X�̎g�p�ɉ��������[���h�s��̌v�Z���s�������ꍇ�A
	/// �{�֐��𗘗p����ƌv�Z���邱�Ƃ��ł��܂��B
	/// </remark>
	/// <param name="pos">���W</param>
	/// <param name="rot">��]</param>
	/// <param name="scale">�g�嗦�B</param>
	/// <returns></returns>
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
	
	bool m_isInited = false; // ����������Ă���
	Matrix m_worldMtx;		 // ���[���h�s��
	Mesh m_meshParts;		 // ���b�V���p�[�c�B
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	 
	//!} 
};