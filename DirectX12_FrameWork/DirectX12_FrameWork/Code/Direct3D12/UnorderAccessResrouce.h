/*****************************************************************//**
 * \file   UnorderAccessResrouce.h
 * \brief  �A���I�[�_�[�h�A�N�Z�X���\�[�X
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once
#include "ShaderResourse.h"


class UnorderAccessResrouce : public ShaderResource {
public:
	virtual ~UnorderAccessResrouce() {};
	
	/*****************************************************************//**
	 * \brief UAV�ɓo�^
	 *  
	 * \param CPU�f�B�X�N���v�^�n���h��
	 * \param bufferNo �o�b�t�@�ԍ�
	 *********************************************************************/
	virtual void RegistUnorderAccessView(D3D12_CPU_DESCRIPTOR_HANDLE, int bufferNo) = 0;
};
