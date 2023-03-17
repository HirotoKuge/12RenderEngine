/*****************************************************************//**
 * \file   mdFile.h
 * \brief  .md�t�@�C���N���X
 * 
 * \author Hiroto Kuge
 * \date   March 2023
 * 
 * \memo .md�͓Ǝ��K�i��3D���f���t�H�[�}�b�g
 *********************************************************************/

#pragma once

//=============================================================================
// .mdFile Class
//=============================================================================
class mdFile{
	//-----------------------------------------------------------------------------
	// list of friends classes and methods.
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
	//=============================================================================
	// MD_Material:�}�e���A���f�[�^�̍\����
	//=============================================================================
	struct MD_Material {
		std::string albedoMapFileName;			//�A���x�h�}�b�v�̃t�@�C����
		std::string normalMapFileName;			//�@���}�b�v�̃t�@�C����
		std::string specularMapFileName;		//�X�y�L�����}�b�v�̃t�@�C����
		std::string reflectionMapFileName;		//���t���N�V�����}�b�v�̃t�@�C����
		std::string refractionMapFileName;		//���܃}�b�v�̃t�@�C����
		std::unique_ptr<char[]>	albedoMap;		//���[�h���ꂽ�A���x�h�}�b�v�F.dds
		unsigned int albedoMapSize;				//�A���x�h�}�b�v�̃T�C�Y�F.dds
		std::unique_ptr<char[]>	normalMap;		//���[�h���ꂽ�@���}�b�v�F.dds
		unsigned int normalMapSize;				//�@���}�b�v�̃T�C�Y
		std::unique_ptr<char[]>	specularMap;	//���[�h���ꂽ�X�y�L�����}�b�v�F.dds
		unsigned int specularMapSize;			//�X�y�L�����}�b�v�̃T�C�Y�F.dds
		std::unique_ptr<char[]>	reflectionMap;	//���[�h���ꂽ���t���N�V�����}�b�v�F.dds
		unsigned int reflectionMapSize;			//���t���N�V�����}�b�v�̃T�C�Y�F.dds
		std::unique_ptr<char[]>	refractionMap;	//���[�h���ꂽ���܃}�b�v�F.dds
		unsigned int refractionMapSize;			//���܃}�b�v�̃T�C�Y�F.dds
		std::string albedoMapFilePath;			//�A���x�h�}�b�v�̃t�@�C���p�X
		std::string normalMapFilePath;			//�@���}�b�v�̃t�@�C���p�X
		std::string specularMapFilePath;		//�X�y�L�����}�b�v�̃t�@�C���p�X
		std::string reflectionMapFilePath;		//���t���N�V�����}�b�v�̃t�@�C���p�X
		std::string refractionMapFilePath;		//���܃}�b�v�̃t�@�C���p�X
	};

	//=============================================================================
	// MD_Vertex:���_�f�[�^�̍\����
	//=============================================================================
	struct MD_Vertex {
		Vector3 pos;			//���W
		Vector3 normal;			//�@��
		Vector3 tangent;		//�ڃx�N�g��
		Vector3 binormal;		//�]�x�N�g��
		Vector2 uv;				//UV���W
	};

	//=============================================================================
	// MD_IndexBuffer16:�C���f�b�N�X(16bit)
	//=============================================================================
	struct MD_Index16{
		std::vector<uint16_t> indices;
	};

	//=============================================================================
	// MD_IndexBuffer16:�C���f�b�N�X(32bit)
	//=============================================================================
	struct MD_Index32 {
		std::vector<uint32_t> indices;
	};

	//=============================================================================
	// MD_UnitMesh:�P�ʃ��b�V��
	//=============================================================================
	struct MD_UnitMesh{
		std::vector<MD_Material> materials;
		std::vector<MD_Vertex>	 vertices;
		std::vector<MD_Index16>  indecies16;
		std::vector<MD_Index32>  indecies32;
	};

	/*****************************************************************//**
	 * \brief 3D���f�������[�h
	 * 
	 * \param filePath .md�t�@�C���̃p�X
	 *********************************************************************/
	void Load(const char* filePath);
	void Load(const wchar_t* filePath);


	/*****************************************************************//**
	 * \brief �P�ʃ��b�V���ɑ΂��ăN�G�����s��
	 * 
	 * \param func �N�G���֐�
	 * \memo ���̊֐��͂��ׂẴ��b�V���ɑ΂��ē���������������Ƃ��Ɏg���܂�
	 *********************************************************************/
	void QueryUnitMesh(std::function<void(const MD_UnitMesh& mesh)> func) const{
		for (auto& mesh : m_meshes) {
			func(mesh);
		}
	}

	/*****************************************************************//**
	 * \brief ���b�V���̔z����擾
	 * 
	 * \return ���b�V���z��̐擪�A�h���X
	 *********************************************************************/
	const std::vector<MD_UnitMesh>& GetMeshes() {
		return m_meshes;
	}

	/*****************************************************************//**
	 * \brief ���b�V���̐����擾
	 * 
	 * \return ���b�V����
	 *********************************************************************/
	uint32_t GetMeshNum()const {
		return uint32_t(m_meshes.size());
	}

	//!} 
private:
	//-----------------------------------------------------------------------------
	// private variables.
	//-----------------------------------------------------------------------------
	//!{
	
	std::vector<MD_UnitMesh> m_meshes; //���b�V���p�[�c
	
	//!}
	//-----------------------------------------------------------------------------
	// private methods.
	//----------------------------------------------------------------------------- 
	//!{
	
	/*****************************************************************//**
	 * \brief �e�N�X�`���������[�h
	 * 
	 * \param fp �t�@�C���|�C���^
	 * \return �t�@�C����
	 *********************************************************************/
	std::string LoadTextureFileName(FILE* fp);
	
	/*****************************************************************//**
	 * \brief �C���f�b�N�X�f�[�^�����[�h
	 * 
	 * \param indices
	 * \param numIndex
	 * \param fp
	 *********************************************************************/
	template<class T>
	void LoadIndex(std::vector<T>& indices, int numIndex, FILE* fp);

	/*****************************************************************//**
	 * \brief �}�e���A�����\�z
	 * 
	 * \param mdMat		�}�e���A���̍\�z��
	 * \param fp		�t�@�C���|�C���^
	 * \param filePath	�ǂݍ���ł�md�t�@�C���̃t�@�C���p�X
	 *********************************************************************/
	void BuildMaterial(MD_Material& mdMat, FILE* fp, const char* filePath);

	//!} 

};
