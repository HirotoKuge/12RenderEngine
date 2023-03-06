/*****************************************************************//**
 * \file   Material.h
 * \brief  �}�e���A���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//!} 
//-----------------------------------------------------------------------------
// Struct 
//-----------------------------------------------------------------------------
//!{

struct PBR_Param {
	float Metallic;
	float Roughness;
};
 

//=============================================================================
// �������f�[�^
//=============================================================================
struct MaterialInitData {
	const char* p_psFilePath;		// �s�N�Z���V�F�[�_�[�̃t�@�C���p�X
	const char* p_vsFilePath;		// ���_�V�F�[�_�[�̃t�@�C���p�X
	const char* p_directryPath;		// ���\�[�X�f�[�^�̃f�B���N�g���p�X
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat;	// �J���[�^�[�Q�b�g�̃t�H�[�}�b�g
	uint32_t numSrv;				// �V�F�[�_�[���\�[�X�̐�
	uint32_t numCbv;				// �萔�o�b�t�@�̐�
	uint32_t offsetInDescriptorsFromTableStartCB;	
	uint32_t offsetInDescriptorsFromTableStartSRV;
	D3D12_FILTER samplerFilter;
	ResMaterialPBR& resMaterial;	// �}�e���A�����\�[�X
};

//!} 
//-----------------------------------------------------------------------------
// Enum
//-----------------------------------------------------------------------------
//!{

//=============================================================================
// �e�N�X�`���̎g�p�p�r
//=============================================================================
enum TEXTURE_USAGE_PBR {
	ALBEDO = 0,		// �A���x�h�}�b�v
	METALLIC,		// ���^���b�N�}�b�v
	ROUGHNESS,		// ���t�l�X�}�b�v
	NORMAL,			// �m�[�}���}�b�v
	HEIGHT,			// �n�C�g�}�b�v
	MAX
};


//!} 
//=============================================================================
// Material Class
//=============================================================================
class Material{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
			/* Nothing */
	//!} 
public:
	//=============================================================================
	// enum
	//=============================================================================
	// �e�N�X�`���̗p�r
	enum TEXTURE_USAGE{
		TEXTURE_USAGE_DIFFUSE = 0,	// �f�B�t���[�Y�}�b�v
		TEXTURE_USAGE_SPECULAR,		// �X�y�L�����}�b�v
		TEXTURE_USAGE_SHININESS,	// �V���C�l�X�}�b�v
		TEXTURE_USAGE_NORMAL,		// �@���}�b�v

		TEXTURE_USAGE_BASECOLOR,	// �x�[�X�J���[�}�b�v
		TEXTURE_USAGE_METALLIC,		// ���^���b�N�}�b�v
		TEXTURE_USAGE_ROUGTHNESS,	// ���t�l�X�}�b�v

		TEXTURE_USAGE_COUNT			// ��`��
	};

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
	
	Material(){};

	/*****************************************************************//**
	 * \brief ������
	 *  
	 * \param resMaterial		�}�e���A�����\�[�X�f�[�^
	 * \param p_vsFilePath		���_�V�F�[�_�[�t�@�C���p�X
	 * \param p_psFilePath		�s�N�Z���V�F�[�_�[�t�@�C���p�X
	 * \param colorBufferFormat	�J���[�o�b�t�@�̃t�H�[�}�b�g
	 * \param numSrv			�V�F�[�_�[���\�[�X�̐�
	 * \param numCbv			�萔�o�b�t�@�̐�
	 * \param offsetInDescriptorsFromTableStartCB	�I�t�Z�b�g
	 * \param offsetInDescriptorsFromTableStartSRV	�I�t�Z�b�g
	 * \param samplerFilter		�T���v���[�t�B���^�[
	 * \return 
	 *********************************************************************/
	bool Init(
		const ResMaterialPBR& resMaterial,	
		const wchar_t* p_vsFilePath,			
		const wchar_t* p_psFilePath,			
		const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormat,
		uint32_t numSrv,
		uint32_t numCbv,
		uint32_t offsetInDescriptorsFromTableStartCB,
		uint32_t offsetInDescriptorsFromTableStartSRV,
		D3D12_FILTER samplerFilter);

	
	/*****************************************************************//**
	 * \brief �`��O����
	 * 
	 * \param rc �����_�����O�R���e�L�X�g
	 *********************************************************************/
	void BeginRender(RenderContext& rc);

	/*****************************************************************//**
	 * \brief �e�N�X�`�����擾
	 * 
	 * \return �e�N�X�`��
	 *********************************************************************/
	Texture& GetAlbedoMap(){
		return *m_pTexturesPBR[TEXTURE_USAGE_PBR::ALBEDO];
	}
	Texture& GetNormalMap(){
		return *m_pTexturesPBR[TEXTURE_USAGE_PBR::NORMAL];
	}
	
	/*****************************************************************//**
	 * \brief �萔�o�b�t�@���擾
	 * 
	 * \return �萔�o�b�t�@ 
	 *********************************************************************/
	ConstantBuffer& GetConstantBuffer()	{
		return m_constantBuffer;
	}
	//!}

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̃|�C���^���擾
	 *  
	 * \param index	�}�e���A���ԍ�
	 * \return �w�肳�ꂽ�}�e���A���ԍ��ɑΉ�����萔�o�b�t�@�̃|�C���^
	 *		   ��v������̂��Ȃ��ꍇ�� nullptr
	 *********************************************************************/
	void* GetBufferPtr(size_t index)const;

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̃|�C���^���擾
	 *
	 * \param index	�}�e���A���ԍ�
	 * \return �w�肳�ꂽ�}�e���A���ԍ��ɑΉ�����萔�o�b�t�@�̃|�C���^
	 *		   ��v������̂��Ȃ��ꍇ�� nullptr
	 *********************************************************************/
	template<typename T>
	T* GetBufferPtr(size_t index)const 
	{ return reinterpret_cast<T*>(GetBufferPtr(index)); }

private:
	//=============================================================================
	// structures
	//=============================================================================
	// �T�u�Z�b�g
	struct Subset {
		ConstantBuffer* pCostantBuffer;									// �萔�o�b�t�@
		D3D12_GPU_DESCRIPTOR_HANDLE TextureHandle[TEXTURE_USAGE_COUNT];	// �e�N�X�`���n���h��
	};

	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	std::array<Texture*,TEXTURE_USAGE_PBR::MAX> m_pTexturesPBR;	// �e�N�X�`��
	
	ConstantBuffer m_constantBuffer;	// �萔�o�b�t�@
	ConstantBuffer m_materialCB;		// �萔�o�b�t�@
	RootSignature m_rootSignature;		// ���[�g�V�O�l�`��
	PipelineState m_pipelineState;		// �p�C�v���C���X�e�[�g
	Shader* m_pVSShader;
	Shader* m_pPSShader;

	uint32_t m_numSr; // �V�F�[�_�[���\�[�X�̐�(�e�N�X�`������)
	uint32_t m_numCb; // �萔�o�b�t�@�̐�

	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	
	/*****************************************************************//**
	 * \brief �p�C�v���C���X�e�[�g�̏�����
	 * 
	 * \param colorBufferFormats �J���[�^�[�Q�b�g�̃t�H�[�}�b�g�̔z��
	 *********************************************************************/
	void InitPipelineState(const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats);

	/*****************************************************************//**
	 * \brief �V�F�[�_�[�̏�����
	 * 
	 * \param pVSShaderPath
	 * \param pPSShaderPath
	 *********************************************************************/
	void InitShader(const wchar_t* pVSShaderPath, const wchar_t* pPSShaderPath);
	void InitShaders(
		const char* fxFilePath,
		const char* vsEntryPointFunc,
		const char* vsSkinEntriyPointFunc,
		const char* psEntryPointFunc);


	/*****************************************************************//**
	 * \brief �e�N�X�`���̏�����
	 *  
	 * \param pDevice		�f�o�C�X
	 * \param pCommandQueue	�R�}���h�L���[
	 * \param resMaterial	�}�e���A�����\�[�X
	 *********************************************************************/
	void InitTexture(
		ID3D12Device5* pDevice,
		ID3D12CommandQueue* pCommandQueue, 
		const ResMaterialPBR& resMaterial);

	Material(const Material&) = delete;
	void operator = (const Material&) = delete;
	
	static const wchar_t* DummyTag;

	//!} 
};

