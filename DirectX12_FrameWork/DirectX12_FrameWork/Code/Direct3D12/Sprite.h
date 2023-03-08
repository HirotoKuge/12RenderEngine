/*****************************************************************//**
 * \file   Sprite.h
 * \brief  �X�v���C�g
 * 
 * \author Hiroto Kuge
 * \date   January 2023
 *********************************************************************/
#pragma once

//=============================================================================
// Includes
//=============================================================================
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"


class Texture;

//�X�v���C�g�ɐݒ�ł���ő�e�N�X�`����
constexpr uint32_t MAX_TEXTURE = 32;

//�g��SRV���ݒ肳��郌�W�X�^�̊J�n�ԍ�
constexpr uint32_t EXPAND_SRV_REG_START_NO = 10;

class ShaderResource;

//=============================================================================
// �A���t�@�u�����f�B���O���[�h
//=============================================================================
enum AlphaBlendMode {
	AlphaBlendMode_None,	//�A���t�@�u�����f�B���O�Ȃ�(�㏑��)
	AlphaBlendMode_Trans,	//����������
	AlphaBlendMode_Add,		//���Z����
};

//=============================================================================
// InitData Sturuct
//=============================================================================
struct SpriteInitData {
	// ��ʃT�C�Y
	uint32_t width = 0;		// �X�v���C�g�̕�
	uint32_t height = 0;	// �X�v���C�g�̍���
	
	// �V�F�[�_�[�֘A
	const char* m_vsEntryPointFunc = "VSMain";						// ���_�V�F�[�_�[�̃G���g���[�|�C���g
	const char* m_psEntryPointFunc = "PSMain";						// �s�N�Z���V�F�[�_�[�̃G���g���[�|�C���g
	const char* m_fxFilePath = nullptr;								// .fx�t�@�C���̃t�@�C���p�X
	
	// ���\�[�X�֘A
	std::array<Texture*, MAX_TEXTURE>		pTextures = { nullptr };	// �O���w��̃e�N�X�`��(GBuffer)	
	std::array<const wchar_t*, MAX_TEXTURE> ddsFilePath = { nullptr };	// DDS�t�@�C���̃t�@�C���p�X
	void*			pExpandConstantBuffer = nullptr;					// ���[�U�[�g���̒萔�o�b�t�@
	uint32_t		expandConstantBufferSize = 0;						// ���[�U�[�g���̒萔�o�b�t�@�̃T�C�Y
	ShaderResource* pExpandShaderResoruceView = nullptr;				// ���[�U�[�g���̃V�F�[�_�[���\�[�X
	AlphaBlendMode	alphaBlendMode = AlphaBlendMode_None;				// �A���t�@�u�����f�B���O���[�h
	D3D12_FILTER	samplerFilter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;	// �T���v���̃t�B���^�[
	
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
// Sprite Class
//=============================================================================
class Sprite {
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

	static const Vector2 DEFAULT_PIVOT;	//�s�{�b�g

	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	virtual ~Sprite();

	/*****************************************************************//**
	 * \brief ����������
	 *
	 * \param	initData �������f�[�^
	 *********************************************************************/
	void Init(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief �X�V����
	 * 
	 * \param pos	���W
	 * \param rot	��]
	 * \param scale	�T�C�Y
	 * \param pivot	�s�{�b�g
	 *********************************************************************/
	void Update(const Vector3& pos, const Quaternion& rot, const Vector3& scale, const Vector2& pivot = DEFAULT_PIVOT);

	/*****************************************************************//**
	 * \brief �`��
	 * 
	 * \param renderContext�@�����_�����O�R���e�L�X�g
	 *********************************************************************/
	void Draw(RenderContext& renderContext);


	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{

	VertexBuffer	m_vertexBuffer;		// ���_�o�b�t�@
	IndexBuffer		m_indexBuffer;		// �C���f�b�N�X�o�b�t�@

	uint32_t m_numTexture = 0;								// �e�N�X�`���̐�
	Texture  m_textures[MAX_TEXTURE];						// �`��Ɏg�p����e�N�X�`��(�e��GBuffer�̃����_�����O���ʂ�����o�^�[���������)
	Texture* m_pTextureExternals[MAX_TEXTURE] = { nullptr }; // �O������w�肳�ꂽ�e�N�X�`��
	
	Vector3		m_position;		// ���W
	Vector2		m_size;			// �T�C�Y
	Quaternion	m_quaternion;	// ��]
	Matrix		m_worldMtx;		// ���[���h�ϊ��s��

	// �X�v���C�g�Ŏg�����[�J���萔�o�b�t�@
	struct SpriteConstantBuffer {
		Matrix mvp;
		Vector4 mulColor;
		Vector4 screenParam;
	};
	SpriteConstantBuffer	m_constantBufferCPU;					 // �X�v���C�g�p�萔�o�b�t�@
	ConstantBuffer			m_constantBufferGPU;					 // GPU���̒萔�o�b�t�@
	ConstantBuffer			m_userExpandConstantBufferGPU;			 // ���[�U�[�g���̒萔�o�b�t�@(GPU��)
	void*					m_userExpandConstantBufferCPU = nullptr; // ���[�U�[�g���̒萔�o�b�t�@(CPU��)
	
	DescriptorHeap	m_descriptorHeap;	// �f�B�X�N���v�^�q�[�v
	RootSignature	m_rootSignature;	// ���[�g�V�O�l�`��
	PipelineState	m_pipelineState;	// �p�C�v���C���X�e�[�g
	Shader			m_vs;				// ���_�V�F�[�_�[
	Shader			m_ps;				// �s�N�Z���V�F�[�_�[


	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{

	/*****************************************************************//**
	 * \brief �e�N�X�`���̏�����
	 *  
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitTextures(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief �V�F�[�_�[��������
	 * 
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitShader(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief ���_�o�b�t�@�ƒ萔�o�b�t�@��������
	 *  
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitVertexBufferAndIndexBuffer(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̏�����
	 *  
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitConstantBuffer(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief �p�C�v���C���X�e�[�g�̏�����
	 *  
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitPipelineState(const SpriteInitData& initData);

	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�q�[�v��������
	 * 
	 * \param initData �������f�[�^
	 *********************************************************************/
	void InitDescriptorHeap(const SpriteInitData& initData);

	//!} 
};
