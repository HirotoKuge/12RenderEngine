/*****************************************************************//**
 * \file   Texture.cpp
 * \brief  テクスチャクラス
 * 
 * \author Hiroto Kuge
 * \date   October 2022
 *********************************************************************/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "Texture.h"
#include <DirectXTex.h>
#include "GraphicsEngine.h"
#include "SharedStruct.h"
#include "ConstantBuffer.h"
#include "../Util/Logger.h"
#include "../Util/FileUtil.h"
#include "../../../DirectXTK/Inc/ResourceUploadBatch.h"
#include "../../../DirectXTK/Inc/DDSTextureLoader.h"


#pragma comment(lib, "DirectXTex.lib")


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
	//DDSファイルからテクスチャをロード。
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
		auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
		pDevice->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, descriptorHandle);
	}
}

//=============================================================================
// DDSファイルからテクスチャをロード
//=============================================================================
void Texture::LoadTextureFromDDSFile(const wchar_t* filePath){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	
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
		//テクスチャの作成に失敗しました。
		return;
	}

	m_pTexture = texture;
	m_textureDesc = m_pTexture->GetDesc();
	
}

//=============================================================================
// メモリからテクスチャをロード
//=============================================================================
void Texture::LoadTextureFromMemory(const char* memory, unsigned int size){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();

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

//=============================================================================
// シェーダーリソースビューの設定を取得
//=============================================================================
D3D12_SHADER_RESOURCE_VIEW_DESC Texture::GetViewDesc(bool isCube) {
	auto desc = m_pTexture->GetDesc();
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	
	viewDesc.Format					 = desc.Format;
	viewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (desc.Dimension)
	{
	case D3D12_RESOURCE_DIMENSION_BUFFER:
	{
		// バッファは対象外
		abort(); // アプリを止める.
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
	{
		if (desc.DepthOrArraySize > 1)
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;

			viewDesc.Texture1DArray.MostDetailedMip = 0;
			viewDesc.Texture1DArray.MipLevels = desc.MipLevels;
			viewDesc.Texture1DArray.FirstArraySlice = 0;
			viewDesc.Texture1DArray.ArraySize = desc.DepthOrArraySize;
			viewDesc.Texture1DArray.ResourceMinLODClamp = 0.0f;
		}
		else
		{
			viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;

			viewDesc.Texture1D.MostDetailedMip = 0;
			viewDesc.Texture1D.MipLevels = desc.MipLevels;
			viewDesc.Texture1D.ResourceMinLODClamp = 0.0f;
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
	{
		if (isCube)
		{
			if (desc.DepthOrArraySize > 6)
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;

				viewDesc.TextureCubeArray.MostDetailedMip = 0;
				viewDesc.TextureCubeArray.MipLevels = desc.MipLevels;
				viewDesc.TextureCubeArray.First2DArrayFace = 0;
				viewDesc.TextureCubeArray.NumCubes = (desc.DepthOrArraySize / 6);
				viewDesc.TextureCubeArray.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

				viewDesc.TextureCube.MostDetailedMip = 0;
				viewDesc.TextureCube.MipLevels = desc.MipLevels;
				viewDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
		}
		else
		{
			if (desc.DepthOrArraySize > 1)
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;

					viewDesc.Texture2DMSArray.FirstArraySlice = 0;
					viewDesc.Texture2DMSArray.ArraySize = desc.DepthOrArraySize;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;

					viewDesc.Texture2DArray.MostDetailedMip = 0;
					viewDesc.Texture2DArray.MipLevels = desc.MipLevels;
					viewDesc.Texture2DArray.FirstArraySlice = 0;
					viewDesc.Texture2DArray.ArraySize = desc.DepthOrArraySize;
					viewDesc.Texture2DArray.PlaneSlice = 0;
					viewDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
				}
			}
			else
			{
				if (desc.MipLevels > 1)
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
				}
				else
				{
					viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

					viewDesc.Texture2D.MostDetailedMip = 0;
					viewDesc.Texture2D.MipLevels = desc.MipLevels;
					viewDesc.Texture2D.PlaneSlice = 0;
					viewDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				}
			}
		}
	}
	break;

	case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
	{
		viewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;

		viewDesc.Texture3D.MostDetailedMip = 0;
		viewDesc.Texture3D.MipLevels = desc.MipLevels;
		viewDesc.Texture3D.ResourceMinLODClamp = 0.0f;
	}
	break;

	default:
	{
		// 想定外
		abort(); // アプリを止める.
	}
	break;
	}

	return viewDesc;

}