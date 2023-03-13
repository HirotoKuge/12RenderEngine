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
// int�^�͌���uint32_t�^�Ŏg���@->�@int�^�̓T�C�Y�������n�ɂ���ĕς��ꍇ�����邱�Ƃ̑΍�
// 
// ���f���͕����̃��b�V���ō\������Ă���
// ���b�V���͕����̒��_�ō\������Ă���
// ���b�V���͕����̒��_,�}�e���A��,�C���f�b�N�X�ō\������Ă���
// ���_�͍��W,�@��,UV���W
//=============================================================================

//=============================================================================
// md�t�@�C���t�H�[�}�b�g
//=============================================================================
namespace mdFileFormat {
	// ���݂�md�t�@�C���̃o�[�W����
	std::uint16_t VERSION = 1;
	
	// �S�̂̃w�b�_�[
	struct MD_Header {
		std::uint8_t  version;	 //�o�[�W����
		std::uint16_t numMeshes; //���b�V����
	};
	
	// �P�ʃ��b�V���̃w�b�_�[
	struct MD_UnitMeshHeader {
		std::uint32_t numMaterial;	//�}�e���A���̐�
		std::uint32_t numVertex;	//���_��
		std::uint8_t indexSize;		//�C���f�b�N�X�̃T�C�Y:�T�C�Y��2��4
		std::uint8_t pad[3];		//�p�f�B���O
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
	
	// �A���x�h�̃t�@�C���������[�h
	mdFMat.albedoMapFileName = LoadTextureFileName(fp);

	// �@���}�b�v�̃t�@�C���������[�h
	mdFMat.normalMapFileName = LoadTextureFileName(fp);

	// �X�y�L�����}�b�v�̃t�@�C���������[�h
	mdFMat.specularMapFileName = LoadTextureFileName(fp);

	// ���t���N�V�����}�b�v�̃t�@�C���������[�h
	mdFMat.reflectionMapFileName = LoadTextureFileName(fp);

	// ���܃}�b�v�̃t�@�C���������[�h
	mdFMat.refractionMapFileName = LoadTextureFileName(fp);

	std::string texFilePath = filePath;
	auto loadTexture = [&](
		std::string& texFileName,
		std::unique_ptr<char[]>& ddsFileMemory,
		unsigned int& fileSize,
		std::string& texFilePathDst
		) {
			int filePathLength = static_cast<int>(texFilePath.length());
			if (texFileName.length() > 0) {
				// ���f���̃t�@�C���p�X���烉�X�g�̃t�H���_��؂��T��
				auto replaseStartPos = texFilePath.find_last_of('/');
				if (replaseStartPos == std::string::npos) {
					replaseStartPos = texFilePath.find_last_of('\\');
				}
				replaseStartPos += 1;
				auto replaceLen = filePathLength - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, texFileName);
				
				// �g���q��dds�ɕύX����
				replaseStartPos = texFilePath.find_last_of('.') + 1;
				replaceLen = texFilePath.length() - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, "dds");
				
				// �e�N�X�`���t�@�C���p�X���L�����Ă���
				texFilePathDst = texFilePath;

				// �e�N�X�`�������[�h
				FILE* texFileFp = fopen(texFilePath.c_str(), "rb");
				if (texFileFp != nullptr) {
					//�t�@�C���T�C�Y���擾
					fseek(texFileFp, 0L, SEEK_END);
					fileSize = ftell(texFileFp);
					fseek(texFileFp, 0L, SEEK_SET);

					ddsFileMemory = std::make_unique<char[]>(fileSize);
					fread(ddsFileMemory.get(), fileSize, 1, texFileFp);
					fclose(texFileFp);
				}
				else {

					MessageBoxA(nullptr, "�e�N�X�`���̃��[�h�Ɏ��s���܂���", "�G���[", MB_OK);
					std::abort();
				}
			}
	};

	// �e�N�X�`�������[�h
	loadTexture(
		mdFMat.albedoMapFileName,
		mdFMat.albedoMap,
		mdFMat.albedoMapSize,
		mdFMat.albedoMapFilePath
	);
	loadTexture(
		mdFMat.normalMapFileName,
		mdFMat.normalMap,
		mdFMat.normalMapSize,
		mdFMat.normalMapFilePath
	);
	loadTexture(
		mdFMat.specularMapFileName,
		mdFMat.specularMap,
		mdFMat.specularMapSize,
		mdFMat.specularMapFilePath
	);
	loadTexture(
		mdFMat.reflectionMapFileName,
		mdFMat.reflectionMap,
		mdFMat.reflectionMapSize,
		mdFMat.reflectionMapFilePath
	);
	loadTexture(
		mdFMat.refractionMapFileName,
		mdFMat.refractionMap,
		mdFMat.refractionMapSize,
		mdFMat.refractionMapFilePath
	);
}

//=============================================================================
// �ǂݍ��ݏ���
//=============================================================================
void mdFile::Load(const char* filePath){
	// �t�@�C�����J�����
	FILE* fp = fopen(filePath, "rb");
	if (fp == nullptr) {
		MessageBoxA(nullptr, "md�t�@�C�����J���܂���", "�G���[", MB_OK);
		return;
	}

	// md�t�@�C���̃w�b�_�[��ǂݍ���
	mdFileFormat::MD_Header header;
	fread(&header, sizeof(header), 1, fp);
	if (header.version != mdFileFormat::VERSION) {
		//md�t�@�C���̃o�[�W�������Ⴄ
		MessageBoxA(nullptr, "md�t�@�C���̃o�[�W�������قȂ��Ă��܂�", "�G���[", MB_OK);
	}

	// == ���b�V���������[�h���Ă��� == //

	// ���b�V���T�C�Y�����m��
	m_meshes.resize(header.numMeshes);

	// ���b�V���P�ʂŃ��[�h�������܂킷
	for (int meshPartsNo = 0; meshPartsNo < header.numMeshes; meshPartsNo++) {
		// �i�[��̐ݒ�
		auto& meshParts = m_meshes[meshPartsNo];
		
		// �P�ʃ��b�V���̃w�b�_�[��ǂ݂���
		mdFileFormat::MD_UnitMeshHeader meshPartsHeader;
		fread(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);

		// �}�e���A�������L�^�ł���̈���m��
		meshParts.materials.resize(meshPartsHeader.numMaterial);

		// �}�e���A�������\�z
		for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
			auto& material = meshParts.materials[materialNo];
			BuildMaterial(material, fp, filePath);
		}

		// ���_�f�[�^�ǂݍ���
		meshParts.vertices.resize(meshPartsHeader.numVertex);
		for (unsigned int vertNo = 0; vertNo < meshPartsHeader.numVertex; vertNo++) {
			// 1���_���Ƃɓǂ݂���
			mdFileFormat::MD_LoadVertex vertexTmp;
			fread(&vertexTmp, sizeof(vertexTmp), 1, fp);

			// �i�[
			auto& vertex = meshParts.vertices[vertNo];
			vertex.pos.Set(vertexTmp.pos[0], vertexTmp.pos[1], vertexTmp.pos[2]);
			vertex.normal.Set(vertexTmp.normal[0], vertexTmp.normal[1], vertexTmp.normal[2]);
			vertex.tangent.Set(vertexTmp.tangent[0], vertexTmp.tangent[1], vertexTmp.tangent[2]);
			vertex.binormal.Set(vertexTmp.binormal[0], vertexTmp.binormal[1], vertexTmp.binormal[2]);
			vertex.uv.Set(vertexTmp.uv[0], vertexTmp.uv[1]);
		}

		// �����ăC���f�b�N�X
		// �����n�ɂ���ă������T�C�Y�ς�鎞������̂ŃT�C�Y���w�肷��
		if (meshPartsHeader.indexSize == 2) {
			//16bit�̃C���f�b�N�X�o�b�t�@
			meshParts.indecies16.resize(meshPartsHeader.numMaterial);
		}
		else {
			//32bit�̃C���f�b�N�X�o�b�t�@
			meshParts.indecies32.resize(meshPartsHeader.numMaterial);
		}

		// �C���f�b�N�X�����[�h
		for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
			

		}
	}

	fclose(fp);

}
