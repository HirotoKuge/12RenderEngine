/*****************************************************************//**
 * \file   mdFile.h
 * \brief  .md�t�@�C���N���X
 *
 * \author Hiroto Kuge
 * \date   March 2023
 *
 * \memo .md�͓Ǝ��K�i��3D���f���t�H�[�}�b�g
 *********************************************************************/
#include "../stdafx.h"
#include "mdFile.h"


//=============================================================================
// .md����
// 
// �K���Ƃ��ĉ����̃T�C�Y�������o���ۂ�size_t�^
// int�^�͌���uint32_t�^�Ŏg��
// �� �T�C�Y�������n�ɂ���ĕς��ꍇ�����邱�Ƃ̑΍�
// 
// ���f���͕����̃��b�V���ō\������Ă���
// ���b�V���͕����̒��_�ō\������Ă���
// ���b�V���͕����̒��_,�}�e���A��,�C���f�b�N�X�ō\������Ă���
// ���_�͍��W,�@��,UV���W,�ڃx�N�g��,�]�x�N�g���ō\������Ă���
// �}�e���A���f�[�^�͊e��}�b�v�e�N�X�`���̃t�@�C���p�X���i�[���Ă���
// �C���f�b�N�X�͂��̂܂܃C���f�b�N�X�ł�
//=============================================================================

//=============================================================================
// md�t�@�C���t�H�[�}�b�g
//=============================================================================
namespace mdFileFormat {
	// ���݂�md�t�@�C���̃o�[�W����
	std::uint16_t VERSION = 1;

	// �}�e���A���̐�

	// �S�̂̃w�b�_�[
	struct MD_Header {
		std::uint16_t version;	 //�o�[�W����
		std::uint16_t numMeshes; //���b�V����
	};

	// �P�ʃ��b�V���̃w�b�_�[
	struct MD_UnitMeshHeader {
		std::uint32_t numMaterial;	//�}�e���A���̐�
		std::uint32_t numVertex;	//���_��
		std::uint32_t numIndex;		//�C���f�b�N�X�̐�
	};

	// �ǂݍ��񂾒��_
	struct MD_LoadVertex {
		float pos[3];		// ���_���W
		float normal[3];	// �@��
		float tangent[3];	// �ڃx�N�g��
		float binormal[3];	// �]�x�N�g��
		float uv[2];		// UV���W
	};
};

//=============================================================================
// �e�N�X�`���������[�h
//=============================================================================
std::string mdFile::LoadTextureFileName(FILE* fp){

	std::string fileName;

	std::size_t fileNameLen;
	fread(&fileNameLen, sizeof(fileNameLen), 1, fp);

	if (fileNameLen > 0) {
		// �ꕶ�����ǂ݂���
		for (size_t i = 0; i < fileNameLen; i++) {
			char tmp;
			fread(&tmp, sizeof(char), fileNameLen, fp);
			fileName += tmp;
		}
	}

	return fileName;
}

//=============================================================================
// �C���f�b�N�X�f�[�^�����[�h
//=============================================================================
template<class T>
void mdFile::LoadIndex(std::vector<T>& indices, int numIndex, FILE* fp){
	indices.resize(numIndex);
	for (auto indexNo = 0; indexNo < numIndex; indexNo++) {
		T index;
		fread(&index, sizeof(index), 1, fp);
		indices[indexNo] = index - 1;	
	}
}

//=============================================================================
// �}�e���A�����\�z
//=============================================================================
void mdFile::BuildMaterial(MD_Material& mdFMat, FILE* fp, const char* filePath){
	
	
}

//=============================================================================
// �ǂݍ��ݏ���
//=============================================================================
void mdFile::Load(const char* filePath){
	
	// �t�@�C�����J�����
	FILE* pLoadFile = fopen(filePath, "rb");
	if (pLoadFile == nullptr) {
		MessageBoxA(nullptr, "md�t�@�C�����J���܂���", "�G���[", MB_OK);
		return;
	}

	//----------------------------
	// md�t�@�C���̃w�b�_�[��ǂݍ���
	//----------------------------
	//!{
	mdFileFormat::MD_Header header;
	fread(&header, sizeof(header), 1, pLoadFile);
	if (header.version != mdFileFormat::VERSION) {
		//md�t�@�C���̃o�[�W�������Ⴄ
		MessageBoxA(nullptr, "md�t�@�C���̃o�[�W�������قȂ��Ă��܂�", "�G���[", MB_OK);
	}
	//!}
	//----------------------------
	// ���b�V���P�ʂœǂ݂���
	//----------------------------
	//!{

	// ���b�V���T�C�Y���m�� 
	m_meshes.resize(header.numMeshes);

	// ���b�V���P�ʂŃ��[�v
	for (auto i = 0; i < header.numMeshes; i++) {
		// �i�[��̐ݒ�
		auto& mesh = m_meshes[i];

		//----------------------------
		// �P�ʃ��b�V���̃w�b�_�[�ǂݏo��
		//----------------------------
		//!{
		mdFileFormat::MD_UnitMeshHeader meshHeader;
		fread(&meshHeader, sizeof(mdFileFormat::MD_UnitMeshHeader), 1, pLoadFile);
		//!} 
		//----------------------------
		// �}�e���A���f�[�^�̓ǂݏo��
		//----------------------------
		//!{

		// �}�e���A���̐����̈���m��
		mesh.materials.resize(meshHeader.numMaterial);

		// �ǂݏo��
		for (auto j = 0; j < meshHeader.numMaterial; j++) {
			
			// �����o����̐ݒ�
			auto& material = mesh.materials[j];

			/*****************************************************************//**
			 * \brief �}�e���A���ǂݏo���֐�
			 *
			 * \param fp �t�@�C���|�C���^
			 * \param dst �i�[��
			 *********************************************************************/
			auto LoadTextureFileName = [&](FILE* fp, std::string& dst) {
				// �i�[����N���A
				dst.clear();

				// ������̒������擾
				size_t fileNameLength = 0;
				fread(&fileNameLength, sizeof(size_t), 1, pLoadFile);

				// �P�������ǂ݂���
				for (auto k = 0; k < fileNameLength; k++) {
					char tmp;
					fread(&tmp, sizeof(char), 1, fp);
					dst += tmp;
				}
			};

			// �ǂ݂���
			LoadTextureFileName(pLoadFile, material.AlbedMapFileName);
			LoadTextureFileName(pLoadFile, material.MetalnessMapFileName);
			LoadTextureFileName(pLoadFile, material.RoughnessMapFileName);
			LoadTextureFileName(pLoadFile, material.NormalMapFileName);
			LoadTextureFileName(pLoadFile, material.HeightMapFileName);

		}

		//!} 
		//----------------------------
		// ���_�f�[�^�̓ǂݏo��
		//----------------------------
		//!{

		// �̈�m��
		mesh.vertices.resize(meshHeader.numVertex);

		for (auto j = 0; j < meshHeader.numVertex; j++) {
			// 1���_���Ƃɓǂ݂���
			mdFileFormat::MD_LoadVertex vertexTmp;
			fread(&vertexTmp, sizeof(vertexTmp), 1, pLoadFile);

			// �i�[
			auto& vertex = mesh.vertices[j];
			vertex.pos.Set(vertexTmp.pos[0], vertexTmp.pos[1], vertexTmp.pos[2]);
			vertex.normal.Set(vertexTmp.normal[0], vertexTmp.normal[1], vertexTmp.normal[2]);
			vertex.uv.Set(vertexTmp.uv[0], vertexTmp.uv[1]);
			vertex.tangent.Set(vertexTmp.tangent[0], vertexTmp.tangent[1], vertexTmp.tangent[2]);
			vertex.binormal.Set(vertexTmp.binormal[0], vertexTmp.binormal[1], vertexTmp.binormal[2]);
		}

		//!}
		//----------------------------
		// �C���f�b�N�X�̓ǂݏo��
		//----------------------------
		//!{ 

		// �̈�m��
		mesh.indecies.resize(meshHeader.numIndex);

		// �ǂݏo��
		for (auto j = 0; j < meshHeader.numIndex; j++) {
			uint32_t index;
			fread(&index, sizeof(uint32_t), 1, pLoadFile);
			mesh.indecies[j] = index;
		}

		//!}  
	}
	//!} 


	fclose(pLoadFile);


}
