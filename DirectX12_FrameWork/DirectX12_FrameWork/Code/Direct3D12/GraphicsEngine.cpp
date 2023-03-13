/*****************************************************************//**
 * \file	GraphicsEngine.cpp
 * \brief	�`��G���W��
 * 
 * \author	Hiroto Kuge
 * \date	October 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "GraphicsEngine.h"


//=============================================================================
// �f�o�b�O���C���[�L����
//=============================================================================
void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}


// �C���X�^���X�A�h���X
GraphicsEngine* GraphicsEngine::pInstance = nullptr;

//=============================================================================
// �C���X�^���X�擾
//=============================================================================
GraphicsEngine* GraphicsEngine::GetInstance() {
	if (pInstance == nullptr) {
		pInstance = new GraphicsEngine();
	}

	return pInstance;
}


//=============================================================================
// �f�X�g���N�^
//=============================================================================
GraphicsEngine::~GraphicsEngine(){
	// �`���҂�
	WaitDraw();

	//��n��
	if (m_pCommandQueue) {
		m_pCommandQueue->Release();
	}
	if (m_pSwapChain) {
		m_pSwapChain->Release();
	}
	if (m_pRTVHeap) {
		m_pRTVHeap->Release();
	}
	if (m_pDSVHeap) {
		m_pDSVHeap->Release();
	}
	if (m_pCommandAllocator) {
		m_pCommandAllocator->Release();
	}
	if (m_pCommandList) {
		m_pCommandList->Release();
	}
	if (m_pPipelineState) {
		m_pPipelineState->Release();
	}
	for (auto& rt : m_pRenderTargets) {
		if (rt) {
			rt->Release();
		}
	}
	if (m_pDepthStencilBuffer) {
		m_pDepthStencilBuffer->Release();
	}
	if (m_pFence) {
		m_pFence->Release();
	}

	if (m_pDevice) {
		m_pDevice->Release();
	}

	CloseHandle(m_fenceEvent);
}

//=============================================================================
// �`��I���҂�
//=============================================================================
void GraphicsEngine::WaitDraw(){
	const UINT64 fence = m_fenceValue;
	m_pCommandQueue->Signal(m_pFence, fence);
	m_fenceValue++;

	if (m_pFence->GetCompletedValue() < fence){
		m_pFence->SetEventOnCompletion(fence, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

//=============================================================================
// ����������
//=============================================================================
bool GraphicsEngine::Init(HWND hwnd, uint32_t frameBufferWidth, uint32_t frameBufferHeight){

	m_frameBufferWidth = frameBufferWidth;
	m_frameBufferHeight = frameBufferHeight;

	//�f�o�C�X�ɃA�N�Z�X���邽�߂̃C���^�[�t�F�[�X���쐬
	auto dxgiFactory = CreateDXGIFactory();
	
	//D3D�f�o�C�X�̍쐬
	if (!CreateD3DDevice(dxgiFactory)) {
		//D3D�f�o�C�X�̍쐬�Ɏ��s����
		MessageBox(hwnd, TEXT("D3D�f�o�C�X�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}
	//�R�}���h�L���[�̍쐬
	if (!CreateCommandQueue()) {
		//�R�}���h�L���[�̍쐬�Ɏ��s����
		MessageBox(hwnd, TEXT("�R�}���h�L���[�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}
	//�X���b�v�`�F�C�����쐬
	if (!CreateSwapChain(hwnd, frameBufferWidth, frameBufferHeight, dxgiFactory)) {
		//�X���b�v�`�F�C���̍쐬�Ɏ��s
		MessageBox(hwnd, TEXT("�X���b�v�`�F�C���̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//�t���[���o�b�t�@�p�̃f�B�X�N���v�^�q�[�v���쐬����
	if (!CreateDescriptorHeapForFrameBuffer()) {
		MessageBox(hwnd, TEXT("�t���[���o�b�t�@�p�̃f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//�t���[���o�b�t�@�p��RTV���쐬����
	if (!CreateRTVForFameBuffer()) {
		MessageBox(hwnd, TEXT("�t���[���o�b�t�@�p��RTV�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;

	}

	//�t���[���o�b�t�@�p��DSV���쐬����
	if (!CreateDSVForFrameBuffer(frameBufferWidth, frameBufferHeight)) {
		MessageBox(hwnd, TEXT("�t���[���o�b�t�@�p��DSV�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//�R�}���h�A���P�[�^�̍쐬
	m_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pCommandAllocator));

	if (!m_pCommandAllocator) {
		MessageBox(hwnd, TEXT("�R�}���h�A���P�[�^�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//�R�}���h���X�g�̍쐬
	if (!CreateCommandList()) {
		MessageBox(hwnd, TEXT("�R�}���h���X�g�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//GPU�Ɠ������Ƃ邽�߂̃I�u�W�F�N�g���쐬����
	if (!CreateSynchronizationWithGPUObject()) {
		MessageBox(hwnd, TEXT("GPU�Ɠ������Ƃ邽�߂̃I�u�W�F�N�g�̍쐬�Ɏ��s���܂���"), TEXT("�G���["), MB_OK);
		return false;
	}

	//�����_�����O�R���e�L�X�g�̍쐬
	m_renderContext.Init(m_pCommandList);

	//�r���[�|�[�g��������
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<FLOAT>(frameBufferWidth);
	m_viewport.Height = static_cast<FLOAT>(frameBufferHeight);
	m_viewport.MinDepth = D3D12_MIN_DEPTH;
	m_viewport.MaxDepth = D3D12_MAX_DEPTH;

	//�V�U�����O��`��������
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = frameBufferWidth;
	m_scissorRect.bottom = frameBufferHeight;

	//CBR_SVR�̃f�B�X�N���v�^�̃T�C�Y���擾
	m_cbrSrvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	//Sampler�̃f�B�X�N���v�^�̃T�C�Y���擾
	m_samplerDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	//���������I������̂�DXGI��j��
	dxgiFactory->Release();

	//�J����������������
	m_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	m_camera2D.SetWidth(static_cast<float>(m_frameBufferWidth));
	m_camera2D.SetHeight(static_cast<float>(m_frameBufferHeight));
	m_camera2D.SetPosition({ 0.0f, 0.0f, -1.0f });
	m_camera2D.SetTarget({ 0.0f, 0.0f, 0.0f });

	m_camera3D.SetPosition({ 0.0f, 50.0f, 200.0f });
	m_camera3D.SetTarget({ 0.0f, 50.0f, 0.0f });

	//DirectXTK�p�̃O���t�B�b�N�������Ǘ��N���X�̃C���X�^���X���쐬����
	m_directXTKGfxMemroy = std::make_unique<DirectX::GraphicsMemory>(m_pDevice);

	return true;
}

//=============================================================================
// DXGI�t�@�N�g���[���擾
//=============================================================================
IDXGIFactory4* GraphicsEngine::CreateDXGIFactory()
{
	uint32_t dxgiFactoryFlags = 0;
#ifdef _DEBUG
	//�f�o�b�O�R���g���[���[������΁A�f�o�b�O���C���[������DXGI���쐬����
	ID3D12Debug* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();

		// Enable additional debug layers.
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		debugController->Release();
	}
#endif
	IDXGIFactory4* factory;
	CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	return factory;
}

//=============================================================================
// �f�o�C�X���쐬
//=============================================================================
bool GraphicsEngine::CreateD3DDevice(IDXGIFactory4* dxgiFactory)
{
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,	//Direct3D 12.1�̋@�\���g��
		D3D_FEATURE_LEVEL_12_0	//Direct3D 12.0�̋@�\���g��
	};
	IDXGIAdapter* adapterTmp = nullptr;
	IDXGIAdapter* adapterVender[Num_GPUVender] = { nullptr };	//�e�x���_�[�̃A�_�v�^�[
	IDXGIAdapter* adapterMaxVideoMemory = nullptr;				//�ő�r�f�I�������̃A�_�v�^
	IDXGIAdapter* useAdapter = nullptr;							//�ŏI�I�Ɏg�p����A�_�v�^
	SIZE_T videoMemorySize = 0;
	
	for (uint32_t i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC desc;
		adapterTmp->GetDesc(&desc);

		if (desc.DedicatedVideoMemory > videoMemorySize) {
			//������̃r�f�I�������̕��������̂ŁA��������g��
			if (adapterMaxVideoMemory != nullptr) {
				adapterMaxVideoMemory->Release();
			}
			adapterMaxVideoMemory = adapterTmp;
			adapterMaxVideoMemory->AddRef();
			videoMemorySize = desc.DedicatedVideoMemory;
		}
		if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {
			//NVIDIA��
			if (adapterVender[GPU_VenderNvidia]) {
				adapterVender[GPU_VenderNvidia]->Release();
			}
			adapterVender[GPU_VenderNvidia] = adapterTmp;
			adapterVender[GPU_VenderNvidia]->AddRef();
		}
		else if (wcsstr(desc.Description, L"AMD") != nullptr) {
			//AMD��
			if (adapterVender[GPU_VenderAMD]) {
				adapterVender[GPU_VenderAMD]->Release();
			}
			adapterVender[GPU_VenderAMD] = adapterTmp;
			adapterVender[GPU_VenderAMD]->AddRef();
		}
		else if (wcsstr(desc.Description, L"Intel") != nullptr) {
			//Intel��
			if (adapterVender[GPU_VenderIntel]) {
				adapterVender[GPU_VenderIntel]->Release();
			}
			adapterVender[GPU_VenderIntel] = adapterTmp;
			adapterVender[GPU_VenderIntel]->AddRef();
		}
		adapterTmp->Release();
	}
	
	//�g�p����A�_�v�^�[�����߂�
	if (adapterVender[GPU_VenderNvidia] != nullptr) {
		//NVIDIA�����ŗD��
		useAdapter = adapterVender[GPU_VenderNvidia];
	}
	else if (adapterVender[GPU_VenderAMD] != nullptr) {
		//����AMD���D��
		useAdapter = adapterVender[GPU_VenderAMD];
	}
	else {
		//NVIDIA��AMD��GPU���Ȃ���΃r�f�I����������ԑ�������g��
		useAdapter = adapterMaxVideoMemory;
	}
	for (auto featureLevel : featureLevels) {
		auto hr = D3D12CreateDevice(
			useAdapter,
			featureLevel,
			IID_PPV_ARGS(&m_pDevice)
		);
		if (SUCCEEDED(hr)) {
			//D3D�f�o�C�X�̍쐬�ɐ�������
			break;
		}
	}
	for (auto& adapter : adapterVender) {
		if (adapter) {
			adapter->Release();
		}
	}
	if (adapterMaxVideoMemory) {
		adapterMaxVideoMemory->Release();
	}
	return m_pDevice != nullptr;
}

//=============================================================================
// �R�}���h�L���[���쐬
//=============================================================================
bool GraphicsEngine::CreateCommandQueue(){
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	auto hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
	if (FAILED(hr)) {
		//�R�}���h�L���[�̍쐬�Ɏ��s����
		return false;
	}
	return true;
}

//=============================================================================
// �X���b�v�`�F�C�����쐬
//=============================================================================
bool GraphicsEngine::CreateSwapChain(
	HWND hwnd,
	uint32_t frameBufferWidth,
	uint32_t frameBufferHeight,
	IDXGIFactory4* dxgiFactory
){
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
	swapChainDesc.Width = frameBufferWidth;
	swapChainDesc.Height = frameBufferHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	IDXGISwapChain1* swapChain;
	dxgiFactory->CreateSwapChainForHwnd(
		m_pCommandQueue,
		hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	);
	
	//IDXGISwapChain3�̃C���^�[�t�F�[�X���擾
	swapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	swapChain->Release();
	
	//IDXGISwapChain3�̃C���^�[�t�F�[�X���擾
	m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	return true;
}

//=============================================================================
// �t���[���o�b�t�@��RTV�p�̃f�B�X�N���v�^�q�[�v���쐬
//=============================================================================
bool GraphicsEngine::CreateDescriptorHeapForFrameBuffer(){
	//RTV�p�̃f�B�X�N���v�^�q�[�v���쐬����
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FRAME_BUFFER_COUNT;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRTVHeap));
	if (FAILED(hr)) {
		//RTV�p�̃f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s����
		return false;
	}
	
	//�f�B�X�N���v�^�̃T�C�Y���擾
	m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//DSV�p�̃f�B�X�N���v�^�q�[�v���쐬����
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDSVHeap));
	if (FAILED(hr)) {
		//DSV�p�̃f�B�X�N���v�^�q�[�v�̍쐬�Ɏ��s����
		return false;
	}
	//�f�B�X�N���v�^�̃T�C�Y���擾
	m_dsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	return true;
}

//=============================================================================
// �t���[���o�b�t�@�p��RTV���쐬
//=============================================================================
bool GraphicsEngine::CreateRTVForFameBuffer(){
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();

	//�t�����g�o�b�t�@�ƃo�b�N�o�b�t�@�p��RTV���쐬
	for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; i++) {
		m_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_pRenderTargets[i]));
		m_pDevice->CreateRenderTargetView(
			m_pRenderTargets[i], nullptr, rtvHandle
		);
		rtvHandle.ptr += m_rtvDescriptorSize;
	}
	return true;
}

//=============================================================================
// �t���[���o�b�t�@�p��DSV���쐬
//=============================================================================
bool GraphicsEngine::CreateDSVForFrameBuffer(uint32_t frameBufferWidth, uint32_t frameBufferHeight){
	D3D12_CLEAR_VALUE dsvClearValue;
	dsvClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsvClearValue.DepthStencil.Depth = 1.0f;
	dsvClearValue.DepthStencil.Stencil = 0;

	CD3DX12_RESOURCE_DESC desc(
		D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		0,
		frameBufferWidth,
		frameBufferHeight,
		1,
		1,
		DXGI_FORMAT_D32_FLOAT,
		1,
		0,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE);

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto hr = m_pDevice->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsvClearValue,
		IID_PPV_ARGS(&m_pDepthStencilBuffer)
	);
	if (FAILED(hr)) {
		//�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s
		return false;
	}
	//�f�B�X�N���v�^���쐬
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer, nullptr, dsvHandle
	);

	return true;
}

//=============================================================================
// �R�}���h���X�g���쐬
//=============================================================================
bool GraphicsEngine::CreateCommandList(){
	//�R�}���h���X�g�̍쐬
	m_pDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)
	);
	if (!m_pCommandList) {
		return false;
	}

	//�R�}���h���X�g�͊J����Ă����Ԃō쐬�����̂ŁA�����������
	m_pCommandList->Close();

	return true;
}

//=============================================================================
// GPU�Ƃ̓����I�u�W�F�N�g���쐬
//=============================================================================
bool GraphicsEngine::CreateSynchronizationWithGPUObject(){
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	if (!m_pFence) {
		//�t�F���X�̍쐬�Ɏ��s����
		return false;
	}
	m_fenceValue = 1;
	
	//�������s���Ƃ��̃C�x���g�n���h�����쐬����
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		return false;
	}
	return true;
}

//=============================================================================
// �`��J�n
//=============================================================================
void GraphicsEngine::BeginRender(){
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//�J�������X�V����
	m_camera2D.Update();
	m_camera3D.Update();

	//�R�}���h�A���P�[�^�������Z�b�g
	m_pCommandAllocator->Reset();
	
	//�����_�����O�R���e�L�X�g�����Z�b�g
	m_renderContext.Reset(m_pCommandAllocator, m_pPipelineState);
	
	//�r���[�|�[�g��ݒ�
	m_renderContext.SetViewportAndScissor(m_viewport);

	m_currentFrameBufferRTVHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	m_currentFrameBufferRTVHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
	
	//�[�x�X�e���V���o�b�t�@�̃f�B�X�N���v�^�q�[�v�̊J�n�A�h���X���擾
	m_currentFrameBufferDSVHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	
	//�o�b�N�o�b�t�@�������_�����O�^�[�Q�b�g�Ƃ��Đݒ�\�ɂȂ�܂ő҂�
	m_renderContext.WaitUntilToPossibleSetRenderTarget(m_pRenderTargets[m_frameIndex]);

	//�����_�����O�^�[�Q�b�g��ݒ�
	m_renderContext.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);

	const float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_renderContext.ClearRenderTargetView(m_currentFrameBufferRTVHandle, clearColor);
	m_renderContext.ClearDepthStencilView(m_currentFrameBufferDSVHandle, 1.0f);

}

//=============================================================================
// �t���[���o�b�t�@�p�̃����_�[�^�[�Q�b�g���쐬
//=============================================================================
void GraphicsEngine::ChangeRenderTargetToFrameBuffer(RenderContext& rc){
	rc.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);
}

//=============================================================================
// �`�抮��
//=============================================================================
void GraphicsEngine::EndRender(){
	// �����_�����O�^�[�Q�b�g�ւ̕`�����݊����҂�
	m_renderContext.WaitUntilFinishDrawingToRenderTarget(m_pRenderTargets[m_frameIndex]);

	m_directXTKGfxMemroy->Commit(m_pCommandQueue);

	//�����_�����O�R���e�L�X�g�����
	m_renderContext.Close();

	//�R�}���h�����s
	ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// �o�b�N�o�b�t�@�Ɠ���ւ���
	m_pSwapChain->Present(1, 0);
	m_directXTKGfxMemroy->GarbageCollect();

	//�`�抮���҂�
	WaitDraw();
}