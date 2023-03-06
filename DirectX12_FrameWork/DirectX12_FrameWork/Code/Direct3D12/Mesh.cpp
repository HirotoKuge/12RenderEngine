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
#include "../stdafx.h"
#include "Mesh.h"
#include "Material.h"
#include "IndexBuffer.h"


MDLoader Mesh::m_loader;

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Mesh::~Mesh(){ 
		
	for (auto& mesh : m_pMeshs){
		//�C���f�b�N�X�o�b�t�@���폜
		for (auto& ib : mesh->pIndexBufferArray) {
			delete ib;
		}
		//�}�e���A�����폜
		for (auto& mat : mesh->pMaterials) {
			delete mat;
		}
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
	m_pMeshs.resize(meshResource.size());


	uint32_t meshNo = 0;
	uint32_t materianNo = 0;

	for (auto& res : meshResource) {
		// ���b�V�����\�[�X���珉����
		CreateMeshFromRes(
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
// ���\�[�X���烁�b�V�����쐬
//=============================================================================
void Mesh::CreateMeshFromRes(
	const ResMesh& meshRes, 
	uint32_t meshNo, 
	uint32_t& materialNum, 
	const wchar_t* pVSShaderPath, 
	const wchar_t* pPSShaderPath, 
	void* pExpandData, 
	int expandDataSize, 
	const std::array<DXGI_FORMAT, MAX_RENDERING_TARGET>& colorBufferFormats, 
	D3D12_FILTER samplerFilter
){
	// ���_�o�b�t�@���쐬
	int numVertex = meshRes.Vertices.size(); // ���_�����擾
	int vertexStride = sizeof(MeshVertex);	 // �P�ʒ��_�̃T�C�Y���v�Z
	auto mesh = new UnitMesh;
	mesh->vertexBuffer.Init(vertexStride * numVertex, vertexStride);
	auto vertices = meshRes.Vertices.data();
	mesh->vertexBuffer.Copy((void*)vertices);

	// �C���f�b�N�X�o�b�t�@���쐬
	auto ib = new IndexBuffer;
	ib->Init(static_cast<int>(meshRes.Indices.size()) * 4, 4);
	ib->Copy((uint32_t*)&meshRes.Indices[0]);
	mesh->pIndexBufferArray.push_back(ib);

	// �}�e���A�����쐬
	// TODO:�}�e���A���̓ǂݍ��݋@�\�����
	ResMaterialPBR resMat;
	resMat.AlbedMapFileName = L"assets/teapot/default.dds";
	resMat.NormalMapFileName = L"assets/teapot/normal.dds";
	resMat.Matallic = 0.5f;
	resMat.Roughness = 0.5f;

	auto mat = new Material;

	mat->Init(
		resMat,
		pVSShaderPath,
		pPSShaderPath,
		colorBufferFormats,
		NUM_SRV_ONE_MATERIAL,
		NUM_CBV_ONE_MATERIAL,
		NUM_CBV_ONE_MATERIAL * materialNum,
		NUM_SRV_ONE_MATERIAL * materialNum,
		samplerFilter
	);
	//�쐬�����}�e���A�������J�E���g����
	materialNum++;
	mesh->pMaterials.push_back(mat);
		
	m_pMeshs[meshNo] = mesh;
}

//=============================================================================
// ���ʂ̕`�揈��
//=============================================================================
void Mesh::DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//���b�V�����ƂɃh���[
	//�v���~�e�B�u�̃g�|���W�[�̓g���C�A���O�����X�g�̂�
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//�萔�o�b�t�@���X�V����
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

	int descriptorHeapNo = 0;
	for (auto& mesh : m_pMeshs) {
		// ���_�o�b�t�@��ݒ�
		rc.SetVertexBuffer(mesh->vertexBuffer);
		
		for (auto matNo = 0; matNo < mesh->pIndexBufferArray.size(); matNo++) {

			// �}�e���A���f�[�^�ݒ�
			mesh->pMaterials[matNo]->BeginRender(rc);

			// �q�[�v��ݒ�
			rc.SetDescriptorHeap(m_descriptorHeap);

			// �C���f�b�N�X�o�b�t�@��ݒ�
			auto& ib = mesh->pIndexBufferArray[matNo];
			rc.SetIndexBuffer(*ib);

			// �h���[�R�[�������s
			rc.DrawIndexed(ib->GetCount());
			descriptorHeapNo++;
		}
	}
}

//=============================================================================
// �C���X�^���V���O�`��
//=============================================================================
void Mesh::DrawInstancing(RenderContext& rc, uint32_t numInstance, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){

	//�萔�o�b�t�@�̐ݒ�A�X�V�ȂǕ`��̋��ʏ��������s����
	DrawCommon(rc, worldMtx, viewMtx, projMtx);

	int descriptorHeapNo = 0;
	for (auto& mesh : m_pMeshs) {
		// ���_�o�b�t�@��ݒ�
		rc.SetVertexBuffer(mesh->vertexBuffer);

		for (auto matNo = 0; matNo < mesh->pIndexBufferArray.size(); matNo++) {

			// �}�e���A���f�[�^�ݒ�
			mesh->pMaterials[matNo]->BeginRender(rc);

			// �q�[�v��ݒ�
			rc.SetDescriptorHeap(m_descriptorHeap);

			// �C���f�b�N�X�o�b�t�@��ݒ�
			auto& ib = mesh->pIndexBufferArray[matNo];
			rc.SetIndexBuffer(*ib);

			// �h���[�R�[�������s
			rc.DrawIndexedInstanced(ib->GetCount(),numInstance);
			descriptorHeapNo++;
		}
	}
}

//=============================================================================
// �f�B�X�N���v�^�q�[�v�̍쐬
//=============================================================================
void Mesh::CreateDescriptorHeaps(){
	//�f�B�X�N���v�^�q�[�v���\�z���Ă���
	int srvNo = 0;
	int cbNo = 0;

	for (auto& mesh : m_pMeshs) {
		for (int matNo = 0; matNo < mesh->pMaterials.size(); matNo++) {
			//�f�B�X�N���v�^�q�[�v�Ƀf�B�X�N���v�^��o�^���Ă���
			m_descriptorHeap.RegistShaderResource(srvNo, mesh->pMaterials[matNo]->GetAlbedoMap());			//�A���x�h�}�b�v
			m_descriptorHeap.RegistShaderResource(srvNo + 1,mesh->pMaterials[matNo]->GetNormalMap());		//�@���}�b�v
			m_descriptorHeap.RegistShaderResource(srvNo + 2,mesh->pMaterials[matNo]->GetNormalMap());		//�@���}�b�v
			//m_descriptorHeap.RegistShaderResource(srvNo + 3,mesh->pMaterials[matNo]->GetMetallicMap());		//�@���}�b�v
			//m_descriptorHeap.RegistShaderResource(srvNo + 3,mesh->pMaterials[matNo]->GetRouthnessMap());		//�@���}�b�v

			for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
				if (m_pExpandShaderResourceViews[i]) {
					m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_pExpandShaderResourceViews[i]);
				}
			}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);

			//TEST:�萔�o�b�t�@�Ŏ����ς���p
			if (mesh->pMaterials[matNo]->GetConstantBuffer().IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 1, mesh->pMaterials[matNo]->GetConstantBuffer());
			}

			// �g���萔�o�b�t�@����Ȃ�ǉ�
			if (m_expandConstantBuffer.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer);
			}

			cbNo += NUM_CBV_ONE_MATERIAL;
		}
	}
	m_descriptorHeap.Commit();
}

