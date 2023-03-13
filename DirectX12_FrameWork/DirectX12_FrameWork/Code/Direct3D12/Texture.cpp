/*****************************************************************//**
 * \file   Texture.cpp
 * \brief  �e�N�X�`���N���X
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
// �R���X�g���N�^
//=============================================================================
Texture::Texture(const wchar_t* filePath){
	InitFromDDSFile(filePath);
}

//=============================================================================
// �f�X�g���N�^
//=============================================================================
Texture::~Texture(){
	if (m_pTexture) {
		m_pTexture->Release();
	}
}

//=============================================================================
// DDS�t�@�C�����珉����
//=============================================================================
void Texture::InitFromDDSFile(const wchar_t* filePath){
	//DDS�t�@�C������e�N�X�`�������[�h
	LoadTextureFromDDSFile(filePath);
}

//=============================================================================
// .png�t�@�C�����珉����
//=============================================================================
void Texture::InitFromPNGFile(const wchar_t* filePath){
	//.png�t�@�C������e�N�X�`�������[�h
	LoadTextureFromWIC(filePath);
}

//=============================================================================
// ����������e�N�X�`����������
//=============================================================================
void Texture::InitFromMemory(const char* memory, unsigned int size){
	// ����������e�N�X�`�������[�h
	LoadTextureFromMemory(memory, size);
}

//=============================================================================
// D3D���\�[�X����e�N�X�`����������
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
// �V�F�[�_�[���\�[�X�r���[�ɓo�^
//=============================================================================
void Texture::RegistShaderResourceView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle, int bufferNo){
	
	if (m_pTexture) {
		
		auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = m_textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = m_textureDesc.MipLevels;
		pDevice->CreateShaderResourceView(m_pTexture, &srvDesc, descriptorHandle);
		
		if (m_isLoadFromWIC) {
			GraphicsEngine::GetInstance()->GetCommandList()->DiscardResource(m_pTextureUploadHeap, nullptr);
			CoUninitialize();
		}
		
	}
}

//=============================================================================
// DDS�t�@�C������e�N�X�`�������[�h
//=============================================================================
void Texture::LoadTextureFromDDSFile(const wchar_t* filePath){
	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	
	DirectX::ResourceUploadBatch re(pDevice);
	re.Begin();
	ID3D12Resource* pTexture;
	auto hr = DirectX::CreateDDSTextureFromFileEx(
		pDevice,
		re,
		filePath,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		0,
		&pTexture
	);
	re.End(GraphicsEngine::GetInstance()->GetCommandQueue());

	if (FAILED(hr)) {
		//�e�N�X�`���̍쐬�Ɏ��s���܂���
		return;
	}

	m_pTexture = pTexture;
	m_textureDesc = m_pTexture->GetDesc();
}

//=============================================================================
// WIC���g�p���ăe�N�X�`����ǂݍ���:BMP,JPEG,PNG,TIFF,GIF�ɑΉ����Ă��� ���܂������Ȃ��Ȃ��?????
//=============================================================================
void Texture::LoadTextureFromWIC(const wchar_t* filePath){
	
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	auto pDevice = GraphicsEngine::GetInstance()->GetDevice();
	auto pCommandList = GraphicsEngine::GetInstance()->GetCommandList();

	ID3D12Resource* pTexture;
	std::unique_ptr<uint8_t[]> wicData;
	D3D12_SUBRESOURCE_DATA subresouceData;

	auto hr = DirectX::LoadWICTextureFromFile(
		pDevice,
		filePath,
		&pTexture,
		wicData,
		subresouceData);

	if (FAILED(hr)) {
		//�e�N�X�`���̓ǂݍ��݂Ɏ��s
		DLOG("�ǂݍ��߂ĂȂ���I")
		return;
	}

	ID3D12Resource* pTextureUploadHeap;
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(pTexture, 0, 1);
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto rDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

	pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&rDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&pTextureUploadHeap));

	UpdateSubresources(
		pCommandList,
		pTexture,
		pTextureUploadHeap,
		0,
		0,
		1,
		&subresouceData);

	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
		pTexture,
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	pCommandList->ResourceBarrier(1,&transition);


	m_isLoadFromWIC = true;
	m_pTexture = pTexture;
	m_pTextureUploadHeap = pTextureUploadHeap;
	m_textureDesc = m_pTexture->GetDesc();
	
}

//=============================================================================
// ����������e�N�X�`�������[�h
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
		//�e�N�X�`���̍쐬�Ɏ��s���܂���
		return;
	}

	m_pTexture = texture;
	m_textureDesc = m_pTexture->GetDesc();
}

