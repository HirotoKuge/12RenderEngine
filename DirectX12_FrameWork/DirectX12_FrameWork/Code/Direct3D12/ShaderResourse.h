/*****************************************************************//**
 * \file   ShaderResourse.h
 * \brief  �V�F�[�_�[���\�[�X�̊��N���X
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once
#include <d3dx12.h>

 //=============================================================================
 // ShaderResourse Class
 //=============================================================================
class ShaderResource {
public:
	virtual ~ShaderResource()
	{
	}
	
	/*****************************************************************//**
	 * \brief �V�F�[�_�[���\�[�X�r���[�ɓo�^
	 *  
	 * \param descriptorHandle CPU�f�B�X�N���v�^�n���h��
	 * \param bufferNo		   �o�b�t�@�ԍ�
	 *********************************************************************/
	virtual void RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo) = 0;

};