/*****************************************************************//**
 * \file   RenderContext.h
 * \brief  �����_�[�R���e�L�X�g
 * 
 * \author Hiroto Kuge
 * \date   February 2023
 *********************************************************************/
#pragma once


//=============================================================================
// Include
//=============================================================================
#include <array>
#include <vector>
#include <d3d12.h>
#include <d3dx12.h>
#include "ComPtr.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "PipelineState.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "DescriptorHeap.h"


//-----------------------------------------------------------------------------
// Forword Decolations
//-----------------------------------------------------------------------------
//!{
class ConstantBuffer;
class Texture;
class DescriptorHeap;
class RenderTarget;

//!} 
//-----------------------------------------------------------------------------
// Struct 
//-----------------------------------------------------------------------------
//!{

//!} 
//-----------------------------------------------------------------------------
// Enum 
//-----------------------------------------------------------------------------
//!{

//!} 

//=============================================================================
// RenderContext Class
//=============================================================================
class RenderContext{
	//-----------------------------------------------------------------------------
	// list of friend classes and methods.
	//-----------------------------------------------------------------------------
	//!{
	//!} 
public:
	//-----------------------------------------------------------------------------
	// public variables.
	//-----------------------------------------------------------------------------
	//!{
	//!} 
	//-----------------------------------------------------------------------------
	// public methods.
	//-----------------------------------------------------------------------------
	//!{
	
	RenderContext(){}

	/*****************************************************************//**
	 * \brief ������
	 *  
	 * \param commandList�@�R�}���h���X�g
	 *********************************************************************/
	void Init(ID3D12GraphicsCommandList4* commandList){
		m_pCommandList = commandList;
	}
	
	/*****************************************************************//**
	 * \brief ���_�o�b�t�@��ݒ�
	 *  
	 * \param vb ���_�o�b�t�@
	 *********************************************************************/
	void SetVertexBuffer(VertexBuffer& vb){
		m_pCommandList->IASetVertexBuffers(0, 1, &vb.GetView());
	}
	
	/*****************************************************************//**
	 * \brief �C���f�b�N�X�o�b�t�@��ݒ�
	 *  
	 * \param ib �C���f�b�N�X�o�b�t�@
	 *********************************************************************/
	void SetIndexBuffer(IndexBuffer& ib){
		m_pCommandList->IASetIndexBuffer(&ib.GetView());
	}

	
	/*****************************************************************//**
	 * \brief �v���~�e�B�u�g�|���W�[��ݒ�
	 *  
	 * \param topology �g�|���W�[
	 *********************************************************************/
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY topology){
		m_pCommandList->IASetPrimitiveTopology(topology);
	}
	
	/*****************************************************************//**
	 * \brief �R�}���h���X�g��ݒ�
	 *  
	 * \param commandList
	 *********************************************************************/
	void SetCommandList(ID3D12GraphicsCommandList4* commandList){
		m_pCommandList = commandList;
	}
	
	/*****************************************************************//**
	 * \brief �r���[�|�[�g�ƃV�U�����O��`��ݒ�
	 *  
	 * \param viewport �r���[�|�[�g
	 *********************************************************************/
	void SetViewportAndScissor(D3D12_VIEWPORT& viewport){
		//�V�U�����O��`���ݒ肷��B
		D3D12_RECT scissorRect;
		scissorRect.bottom = static_cast<LONG>(viewport.Height);
		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = static_cast<LONG>(viewport.Width);
		SetScissorRect(scissorRect);

		m_pCommandList->RSSetViewports(1, &viewport);
		m_currentViewport = viewport;
	}
	
	/*****************************************************************//**
	 * \brief �r���[�|�[�g���擾
	 *  
	 * \return �r���[�|�[�g
	 *********************************************************************/
	D3D12_VIEWPORT GetViewport() const{
		return m_currentViewport;
	}
	
	/*****************************************************************//**
	 * \brief �V�U�����O��`��ݒ�
	 *  
	 * \param rect ��`
	 *********************************************************************/
	void SetScissorRect(D3D12_RECT& rect){
		m_pCommandList->RSSetScissorRects(1, &rect);
	}

	/*****************************************************************//**
	 * \brief ���[�g�V�O�l�`����ݒ�
	 *  
	 * \param rootSignature ���[�g�V�O�l�`��
	 *********************************************************************/
	void SetRootSignature(ID3D12RootSignature* rootSignature){
		m_pCommandList->SetGraphicsRootSignature(rootSignature);
	}
	void SetRootSignature(RootSignature& rootSignature){
		m_pCommandList->SetGraphicsRootSignature(rootSignature.Get());
	}

	/*****************************************************************//**
	 * \brief ���[�g�V�O�l�`����ݒ�(�R���s���[�g�V�F�[�_�[�p)
	 *  
	 * \param rootSignature ���[�g�V�O�l�`��
	 *********************************************************************/
	void SetComputeRootSignature(ID3D12RootSignature* rootSignature){
		m_pCommandList->SetComputeRootSignature(rootSignature);
	}
	void SetComputeRootSignature(RootSignature& rootSignature){
		m_pCommandList->SetComputeRootSignature(rootSignature.Get());
	}
	
	/*****************************************************************//**
	 * \brief �p�C�v���C���X�e�[�g��ݒ�
	 *  
	 * \param pipelineState �p�C�v���C���X�e�[�g
	 *********************************************************************/
	void SetPipelineState(ID3D12PipelineState* pipelineState){
		m_pCommandList->SetPipelineState(pipelineState);
	}
	void SetPipelineState(PipelineState& pipelineState)	{
		m_pCommandList->SetPipelineState(pipelineState.Get());
	}

	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�q�[�v��ݒ�
	 *  
	 * \param descHeap �f�B�X�N���v�^�q�[�v
	 *********************************************************************/
	void SetDescriptorHeap(ID3D12DescriptorHeap* descHeap){
		m_descriptorHeaps[0] = descHeap;
		m_pCommandList->SetDescriptorHeaps(1, m_descriptorHeaps);
	}

	void SetDescriptorHeap(DescriptorHeap& descHeap);
	void SetComputeDescriptorHeap(DescriptorHeap& descHeap);
	
	/*****************************************************************//**
	 * \brief �����̃f�B�X�N���v�^�q�[�v��o�^
	 *  
	 * \param numDescriptorHeap �f�B�X�N���v�^�q�[�v�̐�
	 * \param descPools			�f�B�X�N���v�^�q�[�v�̔z��
	 *********************************************************************/
	void SetDescriptorHeaps(int numDescriptorHeap, const DescriptorHeap* descHeaps[]){
		for (int i = 0; i < numDescriptorHeap; i++) {
			m_descriptorHeaps[i] = descHeaps[i]->Get();
		}
		m_pCommandList->SetDescriptorHeaps(numDescriptorHeap, m_descriptorHeaps);
	}
	
	/*****************************************************************//**
	 * \brief �萔�o�b�t�@��ݒ�
	 *  
	 * \param registerNo �ݒ肷�郌�W�X�^�ԍ�
	 * \param cb		 �萔�o�b�t�@
	 *********************************************************************/
	void SetConstantBuffer(int registerNo, ConstantBuffer& cb){
		if (registerNo < MAX_CONSTANT_BUFFER) {
			m_constantBuffers[registerNo] = &cb;
		}
		else {
			//�͈͊O�A�N�Z�X�B
			std::abort();
		}
	}
	
	/*****************************************************************//**
	 * \brief �V�F�[�_�[���\�[�X(�e�N�X�`��)��ݒ�
	 *  
	 * \param registerNo �ݒ肷�郌�W�X�^�ԍ�
	 * \param texture	 �e�N�X�`��
	 *********************************************************************/
	void SetShaderResource(int registerNo, Texture& texture){
		if (registerNo < MAX_SHADER_RESOURCE) {
			m_shaderResources[registerNo] = &texture;
		}
		else {
			//�͈͊O�A�N�Z�X�B
			std::abort();
		}
	}
	
	/*****************************************************************//**
	 * \brief �����̃����_�����O�^�[�Q�b�g��ݒ�(MRT�p)
	 *  
	 * \param numRT			�����_�����O�^�[�Q�b�g�̐�
	 * \param renderTargets �����_�����O�^�[�Q�b�g�̔z��
	*********************************************************************/
	void SetRenderTargets(UINT numRT, RenderTarget* renderTargets[]);

	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g��ݒ�
	 *  
	 * \param rtvHandle �J���[�^�[�Q�b�g�̃f�B�X�N���v�^�n���h��
	 * \param dsvHandle �[�x�^�[�Q�b�g�̃f�B�X�N���v�^�n���h��
	 *********************************************************************/
	void SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle){
		m_pCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	}

	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g���X���b�g0�ɐݒ�
	 *  
	 * \param renderTarget �����_�����O�^�[�Q�b�g
	 * \memo  �r���[�|�[�g�ݒ�̓��[�U�[���ōs��Ȃ��Ƃ����Ȃ�
	 *********************************************************************/
	void SetRenderTarget(RenderTarget& renderTarget){
		RenderTarget* rtArray[] = { &renderTarget };
		SetRenderTargets(1, rtArray);
	}

	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�ƃr���[�|�[�g�̐ݒ�
	 *  
	 * \param renderTarget �����_�����O�^�[�Q�b�g
	 * \memo  �����Ń����_�����O�^�[�Q�b�g�Ɠ������ƍ����̃r���[�|�[�g���ݒ肳���
	 *********************************************************************/
	void SetRenderTargetAndViewport(RenderTarget& renderTarget);
	
	/*****************************************************************//**
	 * \brief �������̃����_�����O�^�[�Q�b�g�ƃr���[�|�[�g�𓯎��ɐݒ�
	 *  
	 * \param numRT			�����_�����O�^�[�Q�b�g�̌�
	 * \param renderTargets �����_�����O�^�[�Q�b�g�̔z��
	 * \memo  �����Ń����_�����O�^�[�Q�b�g�Ɠ������ƍ����̃r���[�|�[�g���ݒ肳���
	 *********************************************************************/
	void SetRenderTargetsAndViewport(UINT numRT, RenderTarget* renderTargets[]);


	/*****************************************************************//**
	 * \brief �������̃����_�����O�^�[�Q�b�g���N���A
	 *  
	 * \param numRt			�����_�����O�^�[�Q�b�g�̌�
	 * \param renderTargets �����_�����O�^�[�Q�b�g�̔z��
	 * \memo �N���A�J���[�̓����_�����O�^�[�Q�b�g�ɐݒ肵�Ă����g��
	 *********************************************************************/
	void ClearRenderTargetViews(
		int numRt,
		RenderTarget* renderTargets[]
	);
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g���N���A
	 *  
	 * \param rtvHandle  �����_�[�^�[�Q�b�g��CPU�f�B�X�N���v�^�n���h��
	 * \param clearColor �N���A�J���[
	 *********************************************************************/
	void ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, const float* clearColor){
		m_pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�̃N���A
	 *  
	 * \param renderTarget �����_�����O�^�[�Q�b�g
	 *********************************************************************/
	void ClearRenderTargetView(RenderTarget& renderTarget){
		RenderTarget* rtArray[] = { &renderTarget };
		ClearRenderTargetViews(1, rtArray);
	}

	/*****************************************************************//**
	 * \brief �[�x�^�[�Q�b�g���N���A
	 *  
	 * \param dsvHandle	 �[�x�^�[�Q�b�g��CPU�f�B�X�N���v�^�n���h��
	 * \param clearValue �N���A�l
	 *********************************************************************/
	void ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle, float clearValue){
		m_pCommandList->ClearDepthStencilView(
			dsvHandle,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			clearValue,
			0,
			0,
			nullptr);
	}
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�ւ̏������ݑ҂�
	 *  
	 * \memo �����_�����O�^�[�Q�b�g���V�F�[�_�[���\�[�X(�e�N�X�`��)�Ƃ��Ďg���ɂ�
	 *		 ���̊֐����g���ď������݂̊�����҂K�v������
	 *********************************************************************/
	void WaitUntilFinishDrawingToRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilFinishDrawingToRenderTarget(RenderTarget& renderTarget);
	void WaitUntilFinishDrawingToRenderTarget(ID3D12Resource* renderTarget){
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			renderTarget,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief �����_�����O�^�[�Q�b�g�Ƃ��Ďg�p�\�ɂȂ�܂ő҂�
	 *  
	 * \memo �����_�����O�^�[�Q�b�g�Ƃ��Đݒ肵�����ꍇ��
	 *		 ���̊֐����g���Ďg�p�\�ɂȂ�܂ő҂K�v������
	 *********************************************************************/
	void WaitUntilToPossibleSetRenderTargets(int numRt, RenderTarget* renderTargets[]);
	void WaitUntilToPossibleSetRenderTarget(RenderTarget& renderTarget);
	void WaitUntilToPossibleSetRenderTarget(ID3D12Resource* renderTarget){
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}

	/*****************************************************************//**
	 * \brief ���\�[�X�o���A
	 *  
	 * \param barrier �o���A
	 *********************************************************************/
	void ResourceBarrier(D3D12_RESOURCE_BARRIER& barrier){
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief ���\�[�X�X�e�[�g�̑J��
	 *  
	 * \param resrouce	  �J�ڂ��������\�[�X
	 * \param beforeState �J�ڑO�̃X�e�[�g
	 * \param afterState  �J�ڌ�̃X�e�[�g
	 *********************************************************************/
	void TransitionResourceState(ID3D12Resource* resrouce, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState){
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(resrouce, beforeState, afterState);
		m_pCommandList->ResourceBarrier(1, &barrier);
	}
	
	/*****************************************************************//**
	 * \brief �R�}���h���X�g�����
	 *********************************************************************/
	void Close(){
		m_pCommandList->Close();
	}
	
	/*****************************************************************//**
	 * \brief �R�}���h���X�g�����Z�b�g
	 *  
	 * \param commandAllocator �R�}���h�A���P�[�^�[
	 * \param pipelineState	   �p�C�v���C���X�e�[�g
	 *********************************************************************/
	void Reset(ID3D12CommandAllocator* commandAllocator, ID3D12PipelineState* pipelineState){
		m_pCommandList->Reset(commandAllocator, pipelineState);
		//�X�N���b�`���\�[�X���N���A�B
		m_scratchResourceList.clear();
	}
	
	/*****************************************************************//**
	 * \brief �C���f�b�N�X�t���v���~�e�B�u��`��
	 *  
	 * \param indexCount �C���f�b�N�X��
	 *********************************************************************/
	void DrawIndexed(UINT indexCount){
		m_pCommandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
	}
	
	/*****************************************************************//**
	 * \brief �C���X�^���V���O�`��
	 *  
	 * \param indexCount  �C���f�b�N�X��
	 * \param numInstance �C���X�^���X��
	 *********************************************************************/
	void DrawIndexedInstanced(UINT indexCount, UINT numInstance){
		m_pCommandList->DrawIndexedInstanced(indexCount, numInstance, 0, 0, 0);
	}

	/*****************************************************************//**
	 * \brief �R���s���[�g�V�F�[�_�[���f�B�X�p�b�`
	 *  
	 * \param ThreadGroupCountX
	 * \param ThreadGroupCountY
	 * \param ThreadGroupCountZ
	 *********************************************************************/
	void Dispatch(
		UINT ThreadGroupCountX,
		UINT ThreadGroupCountY,
		UINT ThreadGroupCountZ
	){
		m_pCommandList->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
	}
	
	/*****************************************************************//**
	 * \brief ���\�[�X���R�s�[
	 *  
	 * \param pDst �R�s�[��̃��\�[�X
	 * \param pSrc �R�s�[���̃��\�[�X
	 *********************************************************************/
	void CopyResource(ID3D12Resource* pDst, ID3D12Resource* pSrc){
		m_pCommandList->CopyResource(pDst, pSrc);
	}

	//!}
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	enum { MAX_DESCRIPTOR_HEAP = 4 };	//�f�B�X�N���v�^�q�[�v�̍ő吔(Resourse,RTV,DSV,Sampler��4��)
	enum { MAX_CONSTANT_BUFFER = 8 };	//�萔�o�b�t�@�̍ő吔:�s�������瑝�₷
	enum { MAX_SHADER_RESOURCE = 16 };	//�V�F�[�_�[���\�[�X�̍ő吔:�s�������瑝�₷

	D3D12_VIEWPORT				m_currentViewport;										//���݂̃r���[�|�[�g
	ID3D12GraphicsCommandList4* m_pCommandList;											//�R�}���h���X�g
	ID3D12DescriptorHeap*		m_descriptorHeaps[MAX_DESCRIPTOR_HEAP];					//�f�B�X�N���v�^�q�[�v�̔z��
	ConstantBuffer*				m_constantBuffers[MAX_CONSTANT_BUFFER] = { nullptr };	//�萔�o�b�t�@�̔z��
	Texture*					m_shaderResources[MAX_SHADER_RESOURCE] = { nullptr };	//�V�F�[�_�[���\�[�X�̔z��

	std::vector<ComPtr<ID3D12Resource> > m_scratchResourceList;	//�X�N���b�`���\�[�X�̃��X�g
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//-----------------------------------------------------------------------------
	//!{
	
	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�e�[�u����ݒ�
	 *  
	 * \param RootParameterIndex ���[�g�p�����[�^�̃C���f�b�N�X
	 * \param BaseDescriptor	 �擪�̃f�B�X�N���v�^�n���h��
	 *********************************************************************/
	void SetGraphicsRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor){
		m_pCommandList->SetGraphicsRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}
	
	/*****************************************************************//**
	 * \brief �f�B�X�N���v�^�e�[�u����ݒ�
	 *  
	 * \param RootParameterIndex ���[�g�p�����[�^�̃C���f�b�N�X
	 * \param BaseDescriptor	 �擪�̃f�B�X�N���v�^�n���h��
	 *********************************************************************/
	void SetComputeRootDescriptorTable(
		UINT RootParameterIndex,
		D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor){
		m_pCommandList->SetComputeRootDescriptorTable(
			RootParameterIndex,
			BaseDescriptor
		);
	}

	//!} 	
};
