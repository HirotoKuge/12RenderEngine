/*****************************************************************//**
 * \file   VertexBuffer.h
 * \brief  ���_�o�b�t�@�N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

#pragma once

//=============================================================================
// Includes
//=============================================================================
#include <d3d12.h>
#include "ComPtr.h"
#include <cstdint>

//=============================================================================
// Vertex Buffer Class
//=============================================================================
class VertexBuffer{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods
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
	VertexBuffer();

	/*****************************************************************//**
	 * \brief �f�X�g���N�^
	 *********************************************************************/
	~VertexBuffer();

	/*****************************************************************//**
	 * \brief ������
	 * 
	 * \param size	 �o�b�t�@�T�C�Y
	 * \param stride �o�b�t�@�̃X�g���C�h:1���_�̃T�C�Y
	 *********************************************************************/
	void Init(uint32_t size, uint32_t stride);

	/*****************************************************************//**
	 * \brief �f�[�^���R�s�[
	 * 
	 * \param srcVertices�@�R�s�[���̃f�[�^
	 *********************************************************************/
	void Copy(void* srcVertices);

	/*****************************************************************//**
	 * \brief ���_�o�b�t�@�r���[���擾
	 * 
	 * \return ���_�o�b�t�@�r���[
	 *********************************************************************/
	const D3D12_VERTEX_BUFFER_VIEW& GetView() const{
		return m_vertexBufferView;
	}

	/*****************************************************************//**
	 * \brief ���_�o�b�t�@�̃X�g���C�h���擾
	 * 
	 * \return �X�g���C�h(�P�ʁF�o�C�g)
	 * \memo �X�g���C�h��1�v�f�̃T�C�Y
	 *		 �����ł�1���_�̃T�C�Y���擾����
	 *********************************************************************/
	UINT GetStrideInBytes() const{
		return m_vertexBufferView.StrideInBytes;
	}
	
	/*****************************************************************//**
	 * \brief ���\�[�X�̃A�h���X���擾
	 * 
	 * \return ���\�[�X 
	 *********************************************************************/
	ID3D12Resource* GetID3DResourceAddress() const{
		return m_pVertexBuffer.Get();
	}

	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	ComPtr<ID3D12Resource>	 m_pVertexBuffer = nullptr;	//���_�o�b�t�@
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		//���_�o�b�t�@�r���[

	//!} 
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	VertexBuffer(const VertexBuffer&) = delete;	 // �A�N�Z�X�֎~
	void operator = (const VertexBuffer&) = delete;	// �A�N�Z�X�֎~

	//!} 
};
