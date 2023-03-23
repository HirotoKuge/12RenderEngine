/*****************************************************************//**
 * \file   mdFile.h
 * \brief  .mdファイルクラス
 *
 * \author Hiroto Kuge
 * \date   March 2023
 *
 * \memo .mdは独自規格の3Dモデルフォーマット
 *********************************************************************/
#include "../stdafx.h"
#include "mdFile.h"


//=============================================================================
// .mdメモ
// 
// 規則として何かのサイズを書き出す際はsize_t型
// int型は原則uint32_t型で使う
// → サイズが処理系によって変わる場合があることの対策
// 
// モデルは複数のメッシュで構成されている
// メッシュは複数の頂点で構成されている
// メッシュは複数の頂点,マテリアル,インデックスで構成されている
// 頂点は座標,法線,UV座標,接ベクトル,従ベクトルで構成されている
// マテリアルデータは各種マップテクスチャのファイルパスを格納している
// インデックスはそのままインデックスです
//=============================================================================

//=============================================================================
// mdファイルフォーマット
//=============================================================================
namespace mdFileFormat {
	// 現在のmdファイルのバージョン
	std::uint16_t VERSION = 1;

	// マテリアルの数

	// 全体のヘッダー
	struct MD_Header {
		std::uint16_t version;	 //バージョン
		std::uint16_t numMeshes; //メッシュ数
	};

	// 単位メッシュのヘッダー
	struct MD_UnitMeshHeader {
		std::uint32_t numMaterial;	//マテリアルの数
		std::uint32_t numVertex;	//頂点数
		std::uint32_t numIndex;		//インデックスの数
	};

	// 読み込んだ頂点
	struct MD_LoadVertex {
		float pos[3];		// 頂点座標
		float normal[3];	// 法線
		float tangent[3];	// 接ベクトル
		float binormal[3];	// 従ベクトル
		float uv[2];		// UV座標
	};
};

//=============================================================================
// テクスチャ名をロード
//=============================================================================
std::string mdFile::LoadTextureFileName(FILE* fp){

	std::string fileName;

	std::size_t fileNameLen;
	fread(&fileNameLen, sizeof(fileNameLen), 1, fp);

	if (fileNameLen > 0) {
		// 一文字ずつ読みだす
		for (size_t i = 0; i < fileNameLen; i++) {
			char tmp;
			fread(&tmp, sizeof(char), fileNameLen, fp);
			fileName += tmp;
		}
	}

	return fileName;
}

//=============================================================================
// インデックスデータをロード
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
// マテリアルを構築
//=============================================================================
void mdFile::BuildMaterial(MD_Material& mdFMat, FILE* fp, const char* filePath){
	
	
}

//=============================================================================
// 読み込み処理
//=============================================================================
void mdFile::Load(const char* filePath){
	
	// ファイルを開けるで
	FILE* pLoadFile = fopen(filePath, "rb");
	if (pLoadFile == nullptr) {
		MessageBoxA(nullptr, "mdファイルが開けません", "エラー", MB_OK);
		return;
	}

	//----------------------------
	// mdファイルのヘッダーを読み込み
	//----------------------------
	//!{
	mdFileFormat::MD_Header header;
	fread(&header, sizeof(header), 1, pLoadFile);
	if (header.version != mdFileFormat::VERSION) {
		//mdファイルのバージョンが違う
		MessageBoxA(nullptr, "mdファイルのバージョンが異なっています", "エラー", MB_OK);
	}
	//!}
	//----------------------------
	// メッシュ単位で読みだす
	//----------------------------
	//!{

	// メッシュサイズ分確保 
	m_meshes.resize(header.numMeshes);

	// メッシュ単位でループ
	for (auto i = 0; i < header.numMeshes; i++) {
		// 格納先の設定
		auto& mesh = m_meshes[i];

		//----------------------------
		// 単位メッシュのヘッダー読み出し
		//----------------------------
		//!{
		mdFileFormat::MD_UnitMeshHeader meshHeader;
		fread(&meshHeader, sizeof(mdFileFormat::MD_UnitMeshHeader), 1, pLoadFile);
		//!} 
		//----------------------------
		// マテリアルデータの読み出し
		//----------------------------
		//!{

		// マテリアルの数分領域を確保
		mesh.materials.resize(meshHeader.numMaterial);

		// 読み出し
		for (auto j = 0; j < meshHeader.numMaterial; j++) {
			
			// 書き出し先の設定
			auto& material = mesh.materials[j];

			/*****************************************************************//**
			 * \brief マテリアル読み出し関数
			 *
			 * \param fp ファイルポインタ
			 * \param dst 格納先
			 *********************************************************************/
			auto LoadTextureFileName = [&](FILE* fp, std::string& dst) {
				// 格納先をクリア
				dst.clear();

				// 文字列の長さを取得
				size_t fileNameLength = 0;
				fread(&fileNameLength, sizeof(size_t), 1, pLoadFile);

				// １文字ずつ読みだす
				for (auto k = 0; k < fileNameLength; k++) {
					char tmp;
					fread(&tmp, sizeof(char), 1, fp);
					dst += tmp;
				}
			};

			// 読みだす
			LoadTextureFileName(pLoadFile, material.AlbedMapFileName);
			LoadTextureFileName(pLoadFile, material.MetalnessMapFileName);
			LoadTextureFileName(pLoadFile, material.RoughnessMapFileName);
			LoadTextureFileName(pLoadFile, material.NormalMapFileName);
			LoadTextureFileName(pLoadFile, material.HeightMapFileName);

		}

		//!} 
		//----------------------------
		// 頂点データの読み出し
		//----------------------------
		//!{

		// 領域確保
		mesh.vertices.resize(meshHeader.numVertex);

		for (auto j = 0; j < meshHeader.numVertex; j++) {
			// 1頂点ごとに読みだす
			mdFileFormat::MD_LoadVertex vertexTmp;
			fread(&vertexTmp, sizeof(vertexTmp), 1, pLoadFile);

			// 格納
			auto& vertex = mesh.vertices[j];
			vertex.pos.Set(vertexTmp.pos[0], vertexTmp.pos[1], vertexTmp.pos[2]);
			vertex.normal.Set(vertexTmp.normal[0], vertexTmp.normal[1], vertexTmp.normal[2]);
			vertex.uv.Set(vertexTmp.uv[0], vertexTmp.uv[1]);
			vertex.tangent.Set(vertexTmp.tangent[0], vertexTmp.tangent[1], vertexTmp.tangent[2]);
			vertex.binormal.Set(vertexTmp.binormal[0], vertexTmp.binormal[1], vertexTmp.binormal[2]);
		}

		//!}
		//----------------------------
		// インデックスの読み出し
		//----------------------------
		//!{ 

		// 領域確保
		mesh.indecies.resize(meshHeader.numIndex);

		// 読み出し
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
