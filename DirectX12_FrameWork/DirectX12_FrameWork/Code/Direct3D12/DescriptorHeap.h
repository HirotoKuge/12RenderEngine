/*****************************************************************//**
 * \file   DescriptorHeap.h
 * \brief  �f�B�X�N���v�^�q�[�v�N���X
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 * 
 * \memo   ���\�[�X�n(CBV_SRV_UAV)���T���v���[�p
 *********************************************************************/
#pragma once

//=============================================================================
// DescriptorHeap Class
//=============================================================================
class DescriptorHeap{
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
	 * \brief �R���X�g���N�^
	 *********************************************************************/
	DescriptorHeap();
	
	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~DescriptorHeap();
	
	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�q�[�v�{�̂��擾
	 *  
	 * \return �f�B�X�N���v�^�q�[�v
	 *********************************************************************/
	ID3D12DescriptorHeap* Get() const;

	/*****************************************************************//**
	 * \brief �V�F�[�_�[���\�[�X(�e�N�X�`��)��o�^
	 *  
	 * \param registerNo ���W�X�^�ԍ�
	 * \param sr		 �V�F�[�_�[���\�[�X
	 * 
	 * \memo ���W�X�^�ԍ���-1���w�肷��Ǝ����Ŏ��ԍ��̃��W�X�^���g�p�����
	 *********************************************************************/
	void RegistShaderResource(int registerNo, ShaderResource& sr){
		RegistResource(
			registerNo,
			&sr,
			&m_shaderResources.front(),
			m_numShaderResource,
			MAX_SHADER_RESOURCE,
			L"DescriptorHeap::RegistShaderResource() ���W�X�^�ԍ����͈͊O�ł�"
		);
	}

	/*****************************************************************//**
	 * \brief �A���I�[�_�[�A�N�Z�X���\�[�X��o�^
	 *  
	 * \param registerNo ���W�X�^�ԍ�
	 * \param ur		 �A���I�[�_�[���\�[�X
	 * 
	 * \memo ���W�X�^�ԍ���-1���w�肷��Ǝ����Ŏ��ԍ��̃��W�X�^���g�p�����
	 *********************************************************************/
	void RegistUnorderAccessResource(int registerNo, UnorderAccessResrouce& ur){
		RegistResource(
			registerNo,
			&ur,
			&m_uavResources.front(),
			m_numUavResource,
			MAX_SHADER_RESOURCE,
			L"DescriptorHeap::RegistUnorderAccessResource() ���W�X�^�ԍ����͈͊O�ł�"
		);
	}
	
	/*****************************************************************//**
	 * \brief �萔�o�b�t�@���f�B�X�N���v�^�q�[�v�ɓo�^
	 *  
	 * \param registerNo ���W�X�^�ԍ�
	 * \param cb		 �萔�o�b�t�@
	 * 
	 * \memo ���W�X�^�ԍ���-1���w�肷��Ǝ����Ŏ��ԍ��̃��W�X�^���g�p�����
	 *********************************************************************/
	void RegistConstantBuffer(int registerNo, ConstantBuffer& cb){
		RegistResource(
			registerNo,
			&cb,
			&m_constantBuffers.front(),
			m_numConstantBuffer,
			MAX_CONSTANT_BUFFER,
			L"DescriptorHeap::RegistConstantBuffer() ���W�X�^�ԍ����͈͊O�ł�"
		);
	}

	/*****************************************************************//**
	 * \brief �T���v���[��`���f�B�X�N���v�^�q�[�v�ɒǉ�
	 *  
	 * \param registerNo ���W�X�^�ԍ�
	 * \param desc		 �T���v���[��`
	 * 
	 * \memo ���W�X�^�ԍ���-1���w�肷��Ǝ����Ŏ��ԍ��̃��W�X�^���g�p�����
	 *********************************************************************/
	void RegistSamplerDesc(int registerNo, const D3D12_SAMPLER_DESC& desc){
		RegistResource(
			registerNo,
			desc,
			m_samplerDescs,
			m_numSamplerDesc,
			MAX_SAMPLER_STATE,
			L"DescriptorHeap::RegistSamplerDesc() ���W�X�^�ԍ����͈͊O�ł�"
		);
	}
	
	/*****************************************************************//**
	 * \brief �q�[�v�ւ̓o�^���m��
	 *********************************************************************/
	void Commit();

	/*****************************************************************//**
	 * \brief �T���v���X�e�[�g�p�̃f�B�X�N���v�^�q�[�v�ւ̓o�^
	 ********************************************************************/
	void CommitSamplerHeap();

	/*****************************************************************//**
	 * \brief �萔�o�b�t�@�̃f�B�X�N���v�^�̊J�n�n���h�����擾
	 *  
	 * \return �萔�o�b�t�@�̃f�B�X�N���v�^�̊J�n�n���h��
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetConstantBufferGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief �V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�����擾
	 *  
	 * \return �V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�� 
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetShaderResourceGpuDescriptorStartHandle() const;

	/*****************************************************************//**
	 * \brief �A���I�[�_�[�h�A�N�Z�X���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�����擾
	 *  
	 * \return �A���I�[�_�[�h�A�N�Z�X���\�[�X�̃f�B�X�N���v�^�̊J�n�n���h�� 
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetUavResourceGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief �T���v���[�̃f�B�X�N���v�^�̊J�n�n���h�����擾
	 *  
	 * \return �T���v���[�̃f�B�X�N���v�^�̊J�n�n���h��
	 *********************************************************************/
	D3D12_GPU_DESCRIPTOR_HANDLE GetSamplerResourceGpuDescriptorStartHandle() const;
	
	/*****************************************************************//**
	 * \brief �V�F�[�_�[���\�[�X��1�ł��o�^����Ă邩�m�F
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistShaderResource() const{
		return m_numShaderResource != 0;
	}
	
	/*****************************************************************//**
	 * \brief �萔�o�b�t�@��1�ł��o�^����Ă��邩����
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistConstantBuffer() const{
		return m_numConstantBuffer != 0;
	}
	
	/*****************************************************************//**
	 * \brief UAV���\�[�X����ł��o�^����Ă��邩����
	 *  
	 * \return 
	 *********************************************************************/
	bool IsRegistUavResource() const{
		return m_numUavResource != 0;
	}
	
	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	enum {
		MAX_SHADER_RESOURCE = 1024 * 10,	//�V�F�[�_�[���\�[�X�̍ő吔
		MAX_CONSTANT_BUFFER = 1024 * 10,	//�萔�o�b�t�@�̍ő吔
		MAX_SAMPLER_STATE = 16,				//�T���v���X�e�[�g�̍ő吔
	};

	int m_numShaderResource = 0;	//�V�F�[�_�[���\�[�X�̐�
	int m_numConstantBuffer = 0;	//�萔�o�b�t�@�̐�
	int m_numUavResource = 0;		//�A���I�[�_�[�A�N�Z�X���\�[�X�̐�
	int m_numSamplerDesc = 0;		//�T���v���̐�

	ID3D12DescriptorHeap*				m_descriptorHeap[2] = { nullptr };	//�f�B�X�N���v�^�q�[�v

	std::vector<ShaderResource*>		m_shaderResources;					//�V�F�[�_�[���\�[�X
	std::vector<UnorderAccessResrouce*> m_uavResources;						//UAV���\�[�X
	std::vector<ConstantBuffer*>		m_constantBuffers;					//�萔�o�b�t�@
	
	D3D12_SAMPLER_DESC					m_samplerDescs[MAX_SAMPLER_STATE];	//�T���v���X�e�[�g
	D3D12_GPU_DESCRIPTOR_HANDLE			m_cbGpuDescriptorStart[2];			//�萔�o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE			m_srGpuDescriptorStart[2];			//�V�F�[�_�[���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE			m_uavGpuDescriptorStart[2];			//UAV���\�[�X�̃f�B�X�N���v�^�q�[�v�̊J�n�n���h��
	D3D12_GPU_DESCRIPTOR_HANDLE			m_samplerGpuDescriptorStart[2];		//Sampler�̂ŃX�N���v�^�q�[�v�̊J�n�n���h��
	
	uint32_t m_cbrSrvDescriptorSize = 0;						//CBR_SRV�̃f�B�X�N���v�^�̃T�C�Y
	uint32_t m_samplerDescriptorSize = 0;
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief ���\�[�X���f�B�X�N���v�^�q�[�v�ɓo�^
	 *  
	 * \param registerNo	�o�^�ԍ�
	 * \param res			�o�^���郊�\�[�X
	 * \param resTbl		���\�[�X�e�[�u��
	 * \param numRes		�o�^����Ă��郊�\�[�X��
	 * \param MAX_RESOURCE	���\�[�X�̍ő吔
	 * \param errorMessage	�G���[���b�Z�[�W
	 *********************************************************************/
	template<class T>
	void RegistResource(
		int registerNo,
		T res,
		T resTbl[],
		int& numRes,
		const int MAX_RESOURCE,
		const wchar_t* errorMessage
	)
	{
		if (registerNo == -1) {
			//-1���w�肳��Ă�����A���ݓo�^����Ă��閖���̃��\�[�X�̎��ɓo�^�����
			registerNo = numRes;
		}
		if (registerNo < MAX_RESOURCE) {
			resTbl[registerNo] = res;
			if (numRes < registerNo + 1) {
				numRes = registerNo + 1;
			}
		}
		else {
			MessageBox(nullptr, errorMessage, L"�G���[", MB_OK);
			std::abort();
		}
	}
	//!} 


};

