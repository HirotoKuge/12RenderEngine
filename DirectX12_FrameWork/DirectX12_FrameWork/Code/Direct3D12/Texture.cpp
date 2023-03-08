/*****************************************************************//**
 * \file   Texture.cpp
 * \brief  テクスチャクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//=============================================================================
// Includes.
//=============================================================================
#include "../stdafx.h"
#include "Texture.h"


//=============================================================================
// コンストラクタ
//=============================================================================
Texture::Texture(const wchar_t* filePath){
	InitFromDDSFile(filePath);
}

//=============================================================================
// デストラクタ
//=============================================================================
Texture::~Texture(){
	if (m_pTexture) {
		m_pTexture->Release();
	}
}

//=============================================================================
// DDSファイルから初期化
//=============================================================================
void Texture::InitFromDDSFile(const wchar_t* filePath){
	//DDSファイルからテクスチャをロード
	LoadTextureFromDDSFile(filePath);
}

//=============================================================================
// メモリからテクスチャを初期化
//=============================================================================
void Texture::InitFromMemory(const char* memory, unsigned int size){
	// メモリからテクスチャをロード
	LoadTextureFromMemory(memory, size);
}

//=============================================================================
// D3Dリソースからテクスチャを初期化
//=============================================================================
void Texture::InitFromD3DResource(ID3D12Resource* texture){
	if (m_pTexture) {
		m_pTexture->Release();
	}
	m_pTexture = texture;
	m_pTexture->AddRef();
	m_textureDesc = m_pTexture->GetDesc();
}

//=============================================================================
// シェーダーリソースビューに登録
//=============================================================================
void Texture::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo){
	if (m_pTexture) {
		auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
		pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, descriptorHandle);
	}
}

//=============================================================================
// DDSファイルからテクスチャをロード
//=============================================================================
void Texture::LoadTextureFromDDSFile(const wchar_t* filePath){
	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();
	
	DirectX::ResourceUploadBatch re(pDevice);
	re.Begin();
	ID3D12Resource* texture;
	auto hr = DirectX::CreateDDSTextureFromFileEx(
		pDevice,
		re,
		filePath,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		0,
		&texture
	);
	re.End(GraphicsEngine::GetInstance()->GetCommandQueue());

	if (FAILED(hr)) {
		//テクスチャの作成に失敗しました
		return;
	}

	m_pTexture = texture;
	m_textureDesc = m_pTexture->GetDesc();
	
}

//=============================================================================
// メモリからテクスチャをロード
//=============================================================================
void Texture::LoadTextureFromMemory(const char* memory, unsigned int size){
	auto pDevice =GraphicsEngine::GetInstance()->GetDevice();

	DirectX::ResourceUploadBatch re(pDevice);
	re.Begin();
	ID3D12Resource* texture;
	auto hr = DirectX::CreateDDSTextureFromMemoryEx(
		pDevice,
		re,
		(const uint8_t*)memory,
		size,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		0,
		&texture
	);
	re.End(GraphicsEngine::GetInstance()->GetCommandQueue());

	if (FAILED(hr)) {
		//テクスチャの作成に失敗しました
		return;
	}

	m_pTexture = texture;
	m_textureDesc = m_pTexture->GetDesc();
}
