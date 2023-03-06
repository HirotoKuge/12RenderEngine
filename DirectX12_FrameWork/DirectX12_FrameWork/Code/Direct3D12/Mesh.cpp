/*****************************************************************//**
 * \file   Mesh.cpp
 * \brief  メッシュクラス
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
// デストラクタ
//=============================================================================
Mesh::~Mesh(){ 
		
	for (auto& mesh : m_pMeshs){
		//インデックスバッファを削除
		for (auto& ib : mesh->pIndexBufferArray) {
			delete ib;
		}
		//マテリアルを削除
		for (auto& mat : mesh->pMaterials) {
			delete mat;
		}
		//メッシュを削除
		delete mesh;
	}

}

//=============================================================================
// ファイルから初期化
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
	// リソースファイル読み込み
	// THINK:読み込み機構を外に出して一括読み込みにするのもあり
	// そうすればモデルのタグ付けが楽になる
	// その場合はモデルデータをクラス化して管理する必要がある
	std::vector<ResMesh> meshResource;
	if (!m_loader.LoadMD(meshResource, pMDFilePath)) {
		// 読み込みに失敗
	}
	m_pMeshs.resize(meshResource.size());


	uint32_t meshNo = 0;
	uint32_t materianNo = 0;

	for (auto& res : meshResource) {
		// メッシュリソースから初期化
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

	// 共通定数バッファの作成
	m_commonConstantBuffer.Init(sizeof(LocalConstantBuffer), nullptr);

	// ユーザー拡張用の定数バッファを作成
	if (pExpandData) {
		m_expandConstantBuffer.Init(expandDataSize, nullptr);
		m_expandData = pExpandData;
	}
	for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
		m_pExpandShaderResourceViews[i] = pExpandShaderResourceViews[i];
	}

	// ディスクリプタヒープを作成
	CreateDescriptorHeaps();
}

//=============================================================================
// リソースからメッシュを作成
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
	// 頂点バッファを作成
	int numVertex = meshRes.Vertices.size(); // 頂点数を取得
	int vertexStride = sizeof(MeshVertex);	 // 単位頂点のサイズを計算
	auto mesh = new UnitMesh;
	mesh->vertexBuffer.Init(vertexStride * numVertex, vertexStride);
	auto vertices = meshRes.Vertices.data();
	mesh->vertexBuffer.Copy((void*)vertices);

	// インデックスバッファを作成
	auto ib = new IndexBuffer;
	ib->Init(static_cast<int>(meshRes.Indices.size()) * 4, 4);
	ib->Copy((uint32_t*)&meshRes.Indices[0]);
	mesh->pIndexBufferArray.push_back(ib);

	// マテリアルを作成
	// TODO:マテリアルの読み込み機構を作る
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
	//作成したマテリアル数をカウントする
	materialNum++;
	mesh->pMaterials.push_back(mat);
		
	m_pMeshs[meshNo] = mesh;
}

//=============================================================================
// 共通の描画処理
//=============================================================================
void Mesh::DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//メッシュごとにドロー
	//プリミティブのトポロジーはトライアングルリストのみ
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//定数バッファを更新する
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
// 描画処理
//=============================================================================
void Mesh::Draw(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//定数バッファの設定、更新など描画の共通処理を実行する
	DrawCommon(rc, worldMtx, viewMtx, projMtx);

	int descriptorHeapNo = 0;
	for (auto& mesh : m_pMeshs) {
		// 頂点バッファを設定
		rc.SetVertexBuffer(mesh->vertexBuffer);
		
		for (auto matNo = 0; matNo < mesh->pIndexBufferArray.size(); matNo++) {

			// マテリアルデータ設定
			mesh->pMaterials[matNo]->BeginRender(rc);

			// ヒープを設定
			rc.SetDescriptorHeap(m_descriptorHeap);

			// インデックスバッファを設定
			auto& ib = mesh->pIndexBufferArray[matNo];
			rc.SetIndexBuffer(*ib);

			// ドローコールを実行
			rc.DrawIndexed(ib->GetCount());
			descriptorHeapNo++;
		}
	}
}

//=============================================================================
// インスタンシング描画
//=============================================================================
void Mesh::DrawInstancing(RenderContext& rc, uint32_t numInstance, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){

	//定数バッファの設定、更新など描画の共通処理を実行する
	DrawCommon(rc, worldMtx, viewMtx, projMtx);

	int descriptorHeapNo = 0;
	for (auto& mesh : m_pMeshs) {
		// 頂点バッファを設定
		rc.SetVertexBuffer(mesh->vertexBuffer);

		for (auto matNo = 0; matNo < mesh->pIndexBufferArray.size(); matNo++) {

			// マテリアルデータ設定
			mesh->pMaterials[matNo]->BeginRender(rc);

			// ヒープを設定
			rc.SetDescriptorHeap(m_descriptorHeap);

			// インデックスバッファを設定
			auto& ib = mesh->pIndexBufferArray[matNo];
			rc.SetIndexBuffer(*ib);

			// ドローコールを実行
			rc.DrawIndexedInstanced(ib->GetCount(),numInstance);
			descriptorHeapNo++;
		}
	}
}

//=============================================================================
// ディスクリプタヒープの作成
//=============================================================================
void Mesh::CreateDescriptorHeaps(){
	//ディスクリプタヒープを構築していく
	int srvNo = 0;
	int cbNo = 0;

	for (auto& mesh : m_pMeshs) {
		for (int matNo = 0; matNo < mesh->pMaterials.size(); matNo++) {
			//ディスクリプタヒープにディスクリプタを登録していく
			m_descriptorHeap.RegistShaderResource(srvNo, mesh->pMaterials[matNo]->GetAlbedoMap());			//アルベドマップ
			m_descriptorHeap.RegistShaderResource(srvNo + 1,mesh->pMaterials[matNo]->GetNormalMap());		//法線マップ
			m_descriptorHeap.RegistShaderResource(srvNo + 2,mesh->pMaterials[matNo]->GetNormalMap());		//法線マップ
			//m_descriptorHeap.RegistShaderResource(srvNo + 3,mesh->pMaterials[matNo]->GetMetallicMap());		//法線マップ
			//m_descriptorHeap.RegistShaderResource(srvNo + 3,mesh->pMaterials[matNo]->GetRouthnessMap());		//法線マップ

			for (int i = 0; i < MAX_MODEL_EXPAND_SRV; i++) {
				if (m_pExpandShaderResourceViews[i]) {
					m_descriptorHeap.RegistShaderResource(srvNo + EXPAND_SRV_REG__START_NO + i, *m_pExpandShaderResourceViews[i]);
				}
			}
			srvNo += NUM_SRV_ONE_MATERIAL;
			m_descriptorHeap.RegistConstantBuffer(cbNo, m_commonConstantBuffer);

			//TEST:定数バッファで質感変える用
			if (mesh->pMaterials[matNo]->GetConstantBuffer().IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 1, mesh->pMaterials[matNo]->GetConstantBuffer());
			}

			// 拡張定数バッファあるなら追加
			if (m_expandConstantBuffer.IsValid()) {
				m_descriptorHeap.RegistConstantBuffer(cbNo + 2, m_expandConstantBuffer);
			}

			cbNo += NUM_CBV_ONE_MATERIAL;
		}
	}
	m_descriptorHeap.Commit();
}

