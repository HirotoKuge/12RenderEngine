/*****************************************************************//**
 * \file	GraphicsEngine.cpp
 * \brief	描画エンジン
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
// デバッグレイヤー有効化
//=============================================================================
void EnableDebugLayer() {
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer)))) {
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}


// インスタンスアドレス
GraphicsEngine* GraphicsEngine::pInstance = nullptr;

//=============================================================================
// インスタンス取得
//=============================================================================
GraphicsEngine* GraphicsEngine::GetInstance() {
	if (pInstance == nullptr) {
		pInstance = new GraphicsEngine();
	}

	return pInstance;
}


//=============================================================================
// デストラクタ
//=============================================================================
GraphicsEngine::~GraphicsEngine(){
	// 描画を待つ
	WaitDraw();

	//後始末
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
// 描画終了待ち
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
// 初期化処理
//=============================================================================
bool GraphicsEngine::Init(HWND hwnd, uint32_t frameBufferWidth, uint32_t frameBufferHeight){

	m_frameBufferWidth = frameBufferWidth;
	m_frameBufferHeight = frameBufferHeight;

	//デバイスにアクセスするためのインターフェースを作成
	auto dxgiFactory = CreateDXGIFactory();
	
	//D3Dデバイスの作成
	if (!CreateD3DDevice(dxgiFactory)) {
		//D3Dデバイスの作成に失敗した
		MessageBox(hwnd, TEXT("D3Dデバイスの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}
	//コマンドキューの作成
	if (!CreateCommandQueue()) {
		//コマンドキューの作成に失敗した
		MessageBox(hwnd, TEXT("コマンドキューの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}
	//スワップチェインを作成
	if (!CreateSwapChain(hwnd, frameBufferWidth, frameBufferHeight, dxgiFactory)) {
		//スワップチェインの作成に失敗
		MessageBox(hwnd, TEXT("スワップチェインの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//フレームバッファ用のディスクリプタヒープを作成する
	if (!CreateDescriptorHeapForFrameBuffer()) {
		MessageBox(hwnd, TEXT("フレームバッファ用のディスクリプタヒープの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//フレームバッファ用のRTVを作成する
	if (!CreateRTVForFameBuffer()) {
		MessageBox(hwnd, TEXT("フレームバッファ用のRTVの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;

	}

	//フレームバッファ用のDSVを作成する
	if (!CreateDSVForFrameBuffer(frameBufferWidth, frameBufferHeight)) {
		MessageBox(hwnd, TEXT("フレームバッファ用のDSVの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//コマンドアロケータの作成
	m_pDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pCommandAllocator));

	if (!m_pCommandAllocator) {
		MessageBox(hwnd, TEXT("コマンドアロケータの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//コマンドリストの作成
	if (!CreateCommandList()) {
		MessageBox(hwnd, TEXT("コマンドリストの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//GPUと同期をとるためのオブジェクトを作成する
	if (!CreateSynchronizationWithGPUObject()) {
		MessageBox(hwnd, TEXT("GPUと同期をとるためのオブジェクトの作成に失敗しました"), TEXT("エラー"), MB_OK);
		return false;
	}

	//レンダリングコンテキストの作成
	m_renderContext.Init(m_pCommandList);

	//ビューポートを初期化
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<FLOAT>(frameBufferWidth);
	m_viewport.Height = static_cast<FLOAT>(frameBufferHeight);
	m_viewport.MinDepth = D3D12_MIN_DEPTH;
	m_viewport.MaxDepth = D3D12_MAX_DEPTH;

	//シザリング矩形を初期化
	m_scissorRect.left = 0;
	m_scissorRect.top = 0;
	m_scissorRect.right = frameBufferWidth;
	m_scissorRect.bottom = frameBufferHeight;

	//CBR_SVRのディスクリプタのサイズを取得
	m_cbrSrvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	//Samplerのディスクリプタのサイズを取得
	m_samplerDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

	//初期化が終わったのでDXGIを破棄
	dxgiFactory->Release();

	//カメラを初期化する
	m_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	m_camera2D.SetWidth(static_cast<float>(m_frameBufferWidth));
	m_camera2D.SetHeight(static_cast<float>(m_frameBufferHeight));
	m_camera2D.SetPosition({ 0.0f, 0.0f, -1.0f });
	m_camera2D.SetTarget({ 0.0f, 0.0f, 0.0f });

	m_camera3D.SetPosition({ 0.0f, 50.0f, 200.0f });
	m_camera3D.SetTarget({ 0.0f, 50.0f, 0.0f });

	//DirectXTK用のグラフィックメモリ管理クラスのインスタンスを作成する
	m_directXTKGfxMemroy = std::make_unique<DirectX::GraphicsMemory>(m_pDevice);

	return true;
}

//=============================================================================
// DXGIファクトリーを取得
//=============================================================================
IDXGIFactory4* GraphicsEngine::CreateDXGIFactory()
{
	uint32_t dxgiFactoryFlags = 0;
#ifdef _DEBUG
	//デバッグコントローラーがあれば、デバッグレイヤーがあるDXGIを作成する
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
// デバイスを作成
//=============================================================================
bool GraphicsEngine::CreateD3DDevice(IDXGIFactory4* dxgiFactory)
{
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,	//Direct3D 12.1の機能を使う
		D3D_FEATURE_LEVEL_12_0	//Direct3D 12.0の機能を使う
	};
	IDXGIAdapter* adapterTmp = nullptr;
	IDXGIAdapter* adapterVender[Num_GPUVender] = { nullptr };	//各ベンダーのアダプター
	IDXGIAdapter* adapterMaxVideoMemory = nullptr;				//最大ビデオメモリのアダプタ
	IDXGIAdapter* useAdapter = nullptr;							//最終的に使用するアダプタ
	SIZE_T videoMemorySize = 0;
	
	for (uint32_t i = 0; dxgiFactory->EnumAdapters(i, &adapterTmp) != DXGI_ERROR_NOT_FOUND; i++) {
		DXGI_ADAPTER_DESC desc;
		adapterTmp->GetDesc(&desc);

		if (desc.DedicatedVideoMemory > videoMemorySize) {
			//こちらのビデオメモリの方が多いので、こちらを使う
			if (adapterMaxVideoMemory != nullptr) {
				adapterMaxVideoMemory->Release();
			}
			adapterMaxVideoMemory = adapterTmp;
			adapterMaxVideoMemory->AddRef();
			videoMemorySize = desc.DedicatedVideoMemory;
		}
		if (wcsstr(desc.Description, L"NVIDIA") != nullptr) {
			//NVIDIA製
			if (adapterVender[GPU_VenderNvidia]) {
				adapterVender[GPU_VenderNvidia]->Release();
			}
			adapterVender[GPU_VenderNvidia] = adapterTmp;
			adapterVender[GPU_VenderNvidia]->AddRef();
		}
		else if (wcsstr(desc.Description, L"AMD") != nullptr) {
			//AMD製
			if (adapterVender[GPU_VenderAMD]) {
				adapterVender[GPU_VenderAMD]->Release();
			}
			adapterVender[GPU_VenderAMD] = adapterTmp;
			adapterVender[GPU_VenderAMD]->AddRef();
		}
		else if (wcsstr(desc.Description, L"Intel") != nullptr) {
			//Intel製
			if (adapterVender[GPU_VenderIntel]) {
				adapterVender[GPU_VenderIntel]->Release();
			}
			adapterVender[GPU_VenderIntel] = adapterTmp;
			adapterVender[GPU_VenderIntel]->AddRef();
		}
		adapterTmp->Release();
	}
	
	//使用するアダプターを決める
	if (adapterVender[GPU_VenderNvidia] != nullptr) {
		//NVIDIA製が最優先
		useAdapter = adapterVender[GPU_VenderNvidia];
	}
	else if (adapterVender[GPU_VenderAMD] != nullptr) {
		//次はAMDが優先
		useAdapter = adapterVender[GPU_VenderAMD];
	}
	else {
		//NVIDIAとAMDのGPUがなければビデオメモリが一番多いやつを使う
		useAdapter = adapterMaxVideoMemory;
	}
	for (auto featureLevel : featureLevels) {
		auto hr = D3D12CreateDevice(
			useAdapter,
			featureLevel,
			IID_PPV_ARGS(&m_pDevice)
		);
		if (SUCCEEDED(hr)) {
			//D3Dデバイスの作成に成功した
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
// コマンドキューを作成
//=============================================================================
bool GraphicsEngine::CreateCommandQueue(){
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	auto hr = m_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_pCommandQueue));
	if (FAILED(hr)) {
		//コマンドキューの作成に失敗した
		return false;
	}
	return true;
}

//=============================================================================
// スワップチェインを作成
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
	
	//IDXGISwapChain3のインターフェースを取得
	swapChain->QueryInterface(IID_PPV_ARGS(&m_pSwapChain));
	swapChain->Release();
	
	//IDXGISwapChain3のインターフェースを取得
	m_currentBackBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	return true;
}

//=============================================================================
// フレームバッファのRTV用のディスクリプタヒープを作成
//=============================================================================
bool GraphicsEngine::CreateDescriptorHeapForFrameBuffer(){
	//RTV用のディスクリプタヒープを作成する
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FRAME_BUFFER_COUNT;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	auto hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pRTVHeap));
	if (FAILED(hr)) {
		//RTV用のディスクリプタヒープの作成に失敗した
		return false;
	}
	
	//ディスクリプタのサイズを取得
	m_rtvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//DSV用のディスクリプタヒープを作成する
	desc.NumDescriptors = 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hr = m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pDSVHeap));
	if (FAILED(hr)) {
		//DSV用のディスクリプタヒープの作成に失敗した
		return false;
	}
	//ディスクリプタのサイズを取得
	m_dsvDescriptorSize = m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	return true;
}

//=============================================================================
// フレームバッファ用のRTVを作成
//=============================================================================
bool GraphicsEngine::CreateRTVForFameBuffer(){
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();

	//フロントバッファとバックバッファ用のRTVを作成
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
// フレームバッファ用のDSVを作成
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
		//深度ステンシルバッファの作成に失敗
		return false;
	}
	//ディスクリプタを作成
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();

	m_pDevice->CreateDepthStencilView(
		m_pDepthStencilBuffer, nullptr, dsvHandle
	);

	return true;
}

//=============================================================================
// コマンドリストを作成
//=============================================================================
bool GraphicsEngine::CreateCommandList(){
	//コマンドリストの作成
	m_pDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pCommandAllocator, nullptr, IID_PPV_ARGS(&m_pCommandList)
	);
	if (!m_pCommandList) {
		return false;
	}

	//コマンドリストは開かれている状態で作成されるので、いったん閉じる
	m_pCommandList->Close();

	return true;
}

//=============================================================================
// GPUとの同期オブジェクトを作成
//=============================================================================
bool GraphicsEngine::CreateSynchronizationWithGPUObject(){
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pFence));
	if (!m_pFence) {
		//フェンスの作成に失敗した
		return false;
	}
	m_fenceValue = 1;
	
	//同期を行うときのイベントハンドラを作成する
	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (m_fenceEvent == nullptr) {
		return false;
	}
	return true;
}

//=============================================================================
// 描画開始
//=============================================================================
void GraphicsEngine::BeginRender(){
	m_frameIndex = m_pSwapChain->GetCurrentBackBufferIndex();

	//カメラを更新する
	m_camera2D.Update();
	m_camera3D.Update();

	//コマンドアロケータををリセット
	m_pCommandAllocator->Reset();
	
	//レンダリングコンテキストもリセット
	m_renderContext.Reset(m_pCommandAllocator, m_pPipelineState);
	
	//ビューポートを設定
	m_renderContext.SetViewportAndScissor(m_viewport);

	m_currentFrameBufferRTVHandle = m_pRTVHeap->GetCPUDescriptorHandleForHeapStart();
	m_currentFrameBufferRTVHandle.ptr += m_frameIndex * m_rtvDescriptorSize;
	
	//深度ステンシルバッファのディスクリプタヒープの開始アドレスを取得
	m_currentFrameBufferDSVHandle = m_pDSVHeap->GetCPUDescriptorHandleForHeapStart();
	
	//バックバッファがレンダリングターゲットとして設定可能になるまで待つ
	m_renderContext.WaitUntilToPossibleSetRenderTarget(m_pRenderTargets[m_frameIndex]);

	//レンダリングターゲットを設定
	m_renderContext.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);

	const float clearColor[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	m_renderContext.ClearRenderTargetView(m_currentFrameBufferRTVHandle, clearColor);
	m_renderContext.ClearDepthStencilView(m_currentFrameBufferDSVHandle, 1.0f);

}

//=============================================================================
// フレームバッファ用のレンダーターゲットを作成
//=============================================================================
void GraphicsEngine::ChangeRenderTargetToFrameBuffer(RenderContext& rc){
	rc.SetRenderTarget(m_currentFrameBufferRTVHandle, m_currentFrameBufferDSVHandle);
}

//=============================================================================
// 描画完了
//=============================================================================
void GraphicsEngine::EndRender(){
	// レンダリングターゲットへの描き込み完了待ち
	m_renderContext.WaitUntilFinishDrawingToRenderTarget(m_pRenderTargets[m_frameIndex]);

	m_directXTKGfxMemroy->Commit(m_pCommandQueue);

	//レンダリングコンテキストを閉じる
	m_renderContext.Close();

	//コマンドを実行
	ID3D12CommandList* ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// バックバッファと入れ替える
	m_pSwapChain->Present(1, 0);
	m_directXTKGfxMemroy->GarbageCollect();

	//描画完了待ち
	WaitDraw();
}