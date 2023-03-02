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
#include "Mesh.h"
#include "SharedStruct.h"

//=============================================================================
// デストラクタ
//=============================================================================
Mesh::~Mesh(){ 
		
	for (auto& mesh : m_meshs){ 
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
	m_meshs.resize(meshResource.size());


	uint32_t meshNo = 0;
	uint32_t materianNo = 0;

	for (auto& res : meshResource) {
		// メッシュリソースから初期化
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
// 共通の描画処理
//=============================================================================
void Mesh::DrawCommon(RenderContext& rc, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){
	//メッシュごとにドロー
	//プリミティブのトポロジーはトライアングルリストのみ。
	rc.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//定数バッファを更新する。
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

	
	for (auto& mesh : m_meshs) {
		// 頂点バッファを設定
		rc.SetVertexBuffer(mesh->vertexBuffer);
		
		// マテリアルデータ設定
		mesh->material.BeginRender(rc);

		// ヒープを設定
		rc.SetDescriptorHeap(m_descriptorHeap);

		// インデックスバッファを設定
		rc.SetIndexBuffer(mesh->indexBuffer);

		// ドローコールを実行
		rc.DrawIndexed(mesh->indexBuffer.GetCount());
	}
}

//=============================================================================
// インスタンシング描画
//=============================================================================
void Mesh::DrawInstancing(RenderContext& rc, uint32_t numInstance, const Matrix& worldMtx, const Matrix& viewMtx, const Matrix& projMtx){

	//定数バッファの設定、更新など描画の共通処理を実行する
	DrawCommon(rc, worldMtx, viewMtx, projMtx);


	for (auto& mesh : m_meshs) {
		// 頂点バッファを設定
		rc.SetVertexBuffer(mesh->vertexBuffer);

		// マテリアルデータ設定
		mesh->material.BeginRender(rc);

		// ヒープを設定
		rc.SetDescriptorHeap(m_descriptorHeap);

		// インデックスバッファを設定
		rc.SetIndexBuffer(mesh->indexBuffer);

		// ドローコールを実行
		rc.DrawIndexedInstanced(mesh->indexBuffer.GetCount(),numInstance);
	}
}

//=============================================================================
// ディスクリプタヒープの作成
//=============================================================================
void Mesh::CreateDescriptorHeaps(){
	//ディスクリプタヒープを構築していく
	int srvNo = 0;
	int cbNo = 0;

	for (auto& mesh : m_meshs) {
		
		//ディスクリプタヒープにディスクリプタを登録していく。
		m_descriptorHeap.RegistShaderResource(srvNo, mesh->material.GetAlbedoMap());			//アルベドマップ
		m_descriptorHeap.RegistShaderResource(srvNo + 1, mesh->material.GetNormalMap());		//法線マップ
		
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

