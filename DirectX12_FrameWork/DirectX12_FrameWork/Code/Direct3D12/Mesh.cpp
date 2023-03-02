/*****************************************************************//**
 * \file   Mesh.cpp
 * \brief  ���b�V���N���X
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "Mesh.h"
#include "SharedStruct.h"

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Mesh::~Mesh(){ 
		
	for (auto& mesh : m_meshs){ 
		//���b�V�����폜
		delete mesh;
	}

}

//=============================================================================
// �t�@�C�����珉����
//=============================================================================
void Mesh::InitFromFile(
	const wchar_t* pMDFilePath, 
	const wchar_t* pVSShaderPath, 
	const wchar_t* pPSShaderPath, 
	void* pExpandData, 
	int expandDataSize, 
	const std::array<ShaderResource*, MAX_MODEL_EXPAND_SRV>& pExpandShaderResourceViews, 
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats, 
	D3D12_FILTER samplerFilter
){
	// ���\�[�X�t�@�C���ǂݍ���
	// THINK:�ǂݍ��݋@�\���O�ɏo���Ĉꊇ�ǂݍ��݂ɂ���̂�����
	// ��������΃��f���̃^�O�t�����y�ɂȂ�
	// ���̏ꍇ�̓��f���f�[�^���N���X�����ĊǗ�����K�v������
	std::vector<ResMesh> meshResource;
	if (!m_loader.LoadMD(meshResource, pMDFilePath)) {
		// �ǂݍ��݂Ɏ��s
	}
	m_meshs.resize(meshResource.size());


	uint32_t meshNo = 0;
	uint32_t materianNo = 0;

	for (auto& res : meshResource) {
		// ���b�V�����\�[�X���珉����
		CreateMeshFormRes(
			res,
			meshNo,
			materianNo,
			pVSShaderPath, 
			pPSShaderPath, 
			pExpandData, 
			expandDataSize, 
			colorBufferFormats,
			samplerFilter
		);
	}

	// ���ʒ萔�o�b�t�@�̍쐬
	m_commonConstantBuffer.Init(sizeof(LocalConstantBuffer), nullptr);

	// ���[�U�[�g���p�̒萔�o�b�t�@���쐬
	if (pExpandData) {
		m_expandConstantBuffer.Init(expandDataSize, nullptr);
		m_expandData = pExpandData;
	}
	for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
		m_pExpandShaderResourceViews[i] = pExpandShaderResourceViews[i];
	}

	// �f�B�X�N���v�^�q�[�v���쐬
	CreateDescriptorHeaps();
}

//=============================================================================
// ���ʂ̕`�揈��
//=============================================================================
void Mesh::DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//���b�V�����ƂɃh���[
	//�v���~�e�B�u�̃g�|���W�[�̓g���C�A���O�����X�g�̂݁B
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�萔�o�b�t�@���X�V����B
	LocalConstantBuffer cb;
	cb.mWorld = worldMtx;
	cb.mView = viewMtx;
	cb.mProj = projMtx;
	m_commonConstantBuffer.CopyToVRAM(cb);

	if (m_expandData) {
		m_expandConstantBuffer.CopyToVRAM(m_expandData);
	}
	
}
	

//=============================================================================
// �`�揈��
//=============================================================================
void Mesh::Draw(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//�萔�o�b�t�@�̐ݒ�A�X�V�ȂǕ`��̋��ʏ��������s����
	DrawCommon(rc, worldMtx, viewMtx, projMtx);

	
	for (auto& mesh : m_meshs) {
		// ���_�o�b�t�@��ݒ�
		rc.SetVertexBuffer(mesh->vertexBuffer);
		
		// �}�e���A���f�[�^�ݒ�
		mesh->material.BeginRender(rc);

		// �q�[�v��ݒ�
		rc.SetDescriptorHeap(m_descriptorHeap);

		// �C���f�b�N�X�o�b�t�@��ݒ�
		rc.SetIndexBuffer(mesh->indexBuffer);

		// �h���[�R�[�������s
		rc.DrawIndexed(mesh->indexBuffer.GetCount());
	}
}

//=============================================================================
// �C���X�^���V���O�`��
//=============================================================================
void Mesh::DrawInstancing(RenderContext& rc, uint32_t numInstance, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){

	//�萔�o�b�t�@�̐ݒ�A�X�V�ȂǕ`��̋��ʏ��������s����
	DrawCommon(rc, worldMtx, viewMtx, projMtx);


	for (auto& mesh : m_meshs) {
		// ���_�o�b�t�@��ݒ�
		rc.SetVertexBuffer(mesh->vertexBuffer);

		// �}�e���A���f�[�^�ݒ�
		mesh->material.BeginRender(rc);

		// �q�[�v��ݒ�
		rc.SetDescriptorHeap(m_descriptorHeap);

		// �C���f�b�N�X�o�b�t�@��ݒ�
		rc.SetIndexBuffer(mesh->indexBuffer);

		// �h���[�R�[�������s
		rc.DrawIndexedInstanced(mesh->indexBuffer.GetCount(),numInstance);
	}
}

//=============================================================================
// �f�B�X�N���v�^�q�[�v�̍쐬
//=============================================================================
void Mesh::CreateDescriptorHeaps(){
	//�f�B�X�N���v�^�q�[�v���\�z���Ă���
	int srvNo = 0;
	int cbNo = 0;

	for (auto& mesh : m_meshs) {
		
		//�f�B�X�N���v�^�q�[�v�Ƀf�B�X�N���v�^��o�^���Ă����B
		m_descriptorHeap.RegistShaderResource(srvNo, mesh->material.GetAlbedoMap());			//�A���x�h�}�b�v
		m_descriptorHeap.RegistShaderResource(srvNo + 1, mesh->material.GetNormalMap());		//�@���}�b�v
		
		for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
			if (m_pExpandShaderResourceViews[i]) {
				m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_pExpandShaderResourceViews[i]);
			}
		}
		srvNo += NUM_SRV_ONE_MATERIAL;
		m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);

		if (mesh->material.GetConstantBuffer().IsValid()) {
			m_descriptorHeap.RegistConstantBuffer(cbNo + 1, mesh->material.GetConstantBuffer());
		}
		
		if (m_expandConstantBuffer.IsValid()) {
			m_descriptorHeap.RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer);
		}
		
		cbNo += NUM_CBV_ONE_MATERIAL;
	
	}
	m_descriptorHeap.Commit();
}

