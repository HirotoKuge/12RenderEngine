/*****************************************************************//**
 * \file   Mesh.h
 * \brief  ���b�V���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/
#pragma once


class RenderContext;
class Material;
class ShaderResource;
class DescriptorHeap;

const int MAX_MODEL_EXPAND_SRV = 6;

//=============================================================================
// �P�ʃ��b�V��
//=============================================================================
struct UnitMesh {
	VertexBuffer				vertexBuffer;		//���_�o�b�t�@
	std::vector<IndexBuffer*>	pIndexBufferArray;	//�C���f�b�N�X�o�b�t�@
	std::vector<Material*>		pMaterials;			//�}�e���A��
};

 //-----------------------------------------------------------------------------
 // Mesh Class
 //-----------------------------------------------------------------------------
class Mesh{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
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
	~Mesh();	

	/*****************************************************************//**
	 * \brief �t�@�C�����烂�f���f�[�^��������
	 * 
	 * \param pMDFilePath					MD�t�@�C���p�X
	 * \param pVSShaderPath					���_�V�F�[�_�[�̃t�@�C���p�X
	 * \param pPSShaderPath					�s�N�Z���V�F�[�_�[�̃t�@�C���p�X
	 * \param pExpandData					�g���f�[�^
	 * \param expandDataSize				�g���f�[�^�T�C�Y
	 * \param pExpandShaderResourceViews	�g���V�F�[�_�[���\�[�X�̔z��̎Q��
	 * \param colorBufferFormats			�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g
	 * \param samplerFilter					�T���v���[�t�B���^�[
	 *********************************************************************/
	void InitFromFile(
		const wchar_t* pMDFilePath,
		const wchar_t* pVSShaderPath,
		const wchar_t* pPSShaderPath,
		void* pExpandData,
		int expandDataSize,
		const std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV>& pExpandShaderResourceViews,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);

	/*****************************************************************//**
	 * \brief �`��
	  * 
	 * \param rc		�����_�����O�R���e�L�X�g
	 * \param worldMtx	���[���h�ϊ��s��
	 * \param viewMtx	�r���[�ϊ��s��
	 * \param projMtx	�v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	void Draw(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
	
	/*****************************************************************//**
	 * \brief �C���X�^���V���O�`��
	  * 
	 * \param rc		�����_�����O�R���e�L�X�g
	 * \param worldMtx	���[���h�ϊ��s��
	 * \param viewMtx	�r���[�ϊ��s��
	 * \param projMtx	�v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	void DrawInstancing(RenderContext& rc, uint32_t numInstance,const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
	
	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�q�[�v���쐬
	 *********************************************************************/
	void CreateDescriptorHeaps();



	//!}
private: 
	//-----------------------------------------------------------------------------
	// private variables.
	//----------------------------------------------------------------------------- 
	//!{

	
	//�g��SRV���ݒ肳��郌�W�X�^�̊J�n�ԍ�
	const int EXPAND_SRV_REG__START_NO = 10;
	//�P�̃}�e���A���Ŏg�p�����SRV�̐�
	const int NUM_SRV_ONE_MATERIAL = EXPAND_SRV_REG__START_NO + MAX_MODEL_EXPAND_SRV;
	//�P�̃}�e���A���Ŏg�p�����CBV�̐�
	const int NUM_CBV_ONE_MATERIAL = 2;
	
	//=============================================================================
	// ���f���p�̃��[�J���萔�o�b�t�@
	//=============================================================================
	struct LocalConstantBuffer {
		Matrix mWorld;		//���[���h�s��
		Matrix mView;		//�r���[�s��
		Matrix mProj;		//�v���W�F�N�V�����s��
	};

	ConstantBuffer m_commonConstantBuffer;					//���b�V�����ʂ̒萔�o�b�t�@
	ConstantBuffer m_expandConstantBuffer;					//���[�U�[�g���p�̒萔�o�b�t�@
	std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV> m_pExpandShaderResourceViews = { nullptr };	//���[�U�[�g���V�F�[�_�[���\�[�X�r���[
	std::vector<UnitMesh*> m_pMeshs;						//���b�V��
	DescriptorHeap m_descriptorHeap;					//�f�B�X�N���v�^�q�[�v
	void* m_expandData = nullptr;						//���[�U�[�g���f�[�^

	static MDLoader	m_loader; // ���\�[�X���[�_�[

	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief ���\�[�X���烁�b�V�����쐬
	 * 
	 * \param meshRes						���b�V�����\�[�X
	 * \param meshNo						���b�V���ԍ�
	 * \param materialNum					�}�e���A����
	 * \param pVSShaderPath					���_�V�F�[�_�[�̃t�@�C���p�X			
	 * \param pPSShaderPath					�s�N�Z���V�F�[�_�[�̃t�@�C���p�X
	 * \param pExpandData					�g���f�[�^
	 * \param expandDataSize				�g���f�[�^�T�C�Y
	 * \param colorBufferFormats			�����_�[�^�[�Q�b�g�̃t�H�[�}�b�g
	 * \param samplerFilter					�T���v���[�t�B���^�[
	 *********************************************************************/
	void CreateMeshFromRes(
		const ResMesh& meshRes,
		uint32_t meshNo,
		uint32_t& materialNum,
		const wchar_t* pVSShaderPath,
		const wchar_t* pPSShaderPath,
		void* pExpandData,
		int expandDataSize,
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats,
		D3D12_FILTER samplerFilter);

	/*****************************************************************//**
	 * \brief �`��̋��ʏ���
	 * 
	 * \param rc		�����_�����O�R���e�L�X�g
	 * \param worldMtx	���[���h�ϊ��s��
	 * \param viewMtx	�r���[�ϊ��s��
	 * \param projMtx	�v���W�F�N�V�����ϊ��s��
	 *********************************************************************/
	void DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx);
						
	//!}   
};

