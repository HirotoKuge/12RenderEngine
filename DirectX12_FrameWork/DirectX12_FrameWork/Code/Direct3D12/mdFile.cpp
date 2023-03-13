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
// int型は原則uint32_t型で使う　->　int型はサイズが処理系によって変わる場合があることの対策
// 
// モデルは複数のメッシュで構成されている
// メッシュは複数の頂点で構成されている
// メッシュは複数の頂点,マテリアル,インデックスで構成されている
// 頂点は座標,法線,UV座標
//=============================================================================

//=============================================================================
// mdファイルフォーマット
//=============================================================================
namespace mdFileFormat {
	// 現在のmdファイルのバージョン
	std::uint16_t VERSION = 1;
	
	// 全体のヘッダー
	struct MD_Header {
		std::uint8_t  version;	 //バージョン
		std::uint16_t numMeshes; //メッシュ数
	};
	
	// 単位メッシュのヘッダー
	struct MD_UnitMeshHeader {
		std::uint32_t numMaterial;	//マテリアルの数
		std::uint32_t numVertex;	//頂点数
		std::uint8_t indexSize;		//インデックスのサイズ:サイズは2か4
		std::uint8_t pad[3];		//パディング
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
	
	// アルベドのファイル名をロード
	mdFMat.albedoMapFileName = LoadTextureFileName(fp);

	// 法線マップのファイル名をロード
	mdFMat.normalMapFileName = LoadTextureFileName(fp);

	// スペキュラマップのファイル名をロード
	mdFMat.specularMapFileName = LoadTextureFileName(fp);

	// リフレクションマップのファイル名をロード
	mdFMat.reflectionMapFileName = LoadTextureFileName(fp);

	// 屈折マップのファイル名をロード
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
				// モデルのファイルパスからラストのフォルダ区切りを探す
				auto replaseStartPos = texFilePath.find_last_of('/');
				if (replaseStartPos == std::string::npos) {
					replaseStartPos = texFilePath.find_last_of('\\');
				}
				replaseStartPos += 1;
				auto replaceLen = filePathLength - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, texFileName);
				
				// 拡張子をddsに変更する
				replaseStartPos = texFilePath.find_last_of('.') + 1;
				replaceLen = texFilePath.length() - replaseStartPos;
				texFilePath.replace(replaseStartPos, replaceLen, "dds");
				
				// テクスチャファイルパスを記憶しておく
				texFilePathDst = texFilePath;

				// テクスチャをロード
				FILE* texFileFp = fopen(texFilePath.c_str(), "rb");
				if (texFileFp != nullptr) {
					//ファイルサイズを取得
					fseek(texFileFp, 0L, SEEK_END);
					fileSize = ftell(texFileFp);
					fseek(texFileFp, 0L, SEEK_SET);

					ddsFileMemory = std::make_unique<char[]>(fileSize);
					fread(ddsFileMemory.get(), fileSize, 1, texFileFp);
					fclose(texFileFp);
				}
				else {

					MessageBoxA(nullptr, "テクスチャのロードに失敗しました", "エラー", MB_OK);
					std::abort();
				}
			}
	};

	// テクスチャをロード
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
// 読み込み処理
//=============================================================================
void mdFile::Load(const char* filePath){
	// ファイルを開けるで
	FILE* fp = fopen(filePath, "rb");
	if (fp == nullptr) {
		MessageBoxA(nullptr, "mdファイルが開けません", "エラー", MB_OK);
		return;
	}

	// mdファイルのヘッダーを読み込み
	mdFileFormat::MD_Header header;
	fread(&header, sizeof(header), 1, fp);
	if (header.version != mdFileFormat::VERSION) {
		//mdファイルのバージョンが違う
		MessageBoxA(nullptr, "mdファイルのバージョンが異なっています", "エラー", MB_OK);
	}

	// == メッシュ情報をロードしていく == //

	// メッシュサイズ分を確保
	m_meshes.resize(header.numMeshes);

	// メッシュ単位でロード処理をまわす
	for (int meshPartsNo = 0; meshPartsNo < header.numMeshes; meshPartsNo++) {
		// 格納先の設定
		auto& meshParts = m_meshes[meshPartsNo];
		
		// 単位メッシュのヘッダーを読みだす
		mdFileFormat::MD_UnitMeshHeader meshPartsHeader;
		fread(&meshPartsHeader, sizeof(meshPartsHeader), 1, fp);

		// マテリアル情報を記録できる領域を確保
		meshParts.materials.resize(meshPartsHeader.numMaterial);

		// マテリアル情報を構築
		for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
			auto& material = meshParts.materials[materialNo];
			BuildMaterial(material, fp, filePath);
		}

		// 頂点データ読み込み
		meshParts.vertices.resize(meshPartsHeader.numVertex);
		for (unsigned int vertNo = 0; vertNo < meshPartsHeader.numVertex; vertNo++) {
			// 1頂点ごとに読みだす
			mdFileFormat::MD_LoadVertex vertexTmp;
			fread(&vertexTmp, sizeof(vertexTmp), 1, fp);

			// 格納
			auto& vertex = meshParts.vertices[vertNo];
			vertex.pos.Set(vertexTmp.pos[0], vertexTmp.pos[1], vertexTmp.pos[2]);
			vertex.normal.Set(vertexTmp.normal[0], vertexTmp.normal[1], vertexTmp.normal[2]);
			vertex.tangent.Set(vertexTmp.tangent[0], vertexTmp.tangent[1], vertexTmp.tangent[2]);
			vertex.binormal.Set(vertexTmp.binormal[0], vertexTmp.binormal[1], vertexTmp.binormal[2]);
			vertex.uv.Set(vertexTmp.uv[0], vertexTmp.uv[1]);
		}

		// 続いてインデックス
		// 処理系によってメモリサイズ変わる時があるのでサイズを指定する
		if (meshPartsHeader.indexSize == 2) {
			//16bitのインデックスバッファ
			meshParts.indecies16.resize(meshPartsHeader.numMaterial);
		}
		else {
			//32bitのインデックスバッファ
			meshParts.indecies32.resize(meshPartsHeader.numMaterial);
		}

		// インデックスをロード
		for (unsigned int materialNo = 0; materialNo < meshPartsHeader.numMaterial; materialNo++) {
			

		}
	}

	fclose(fp);

}
