/*****************************************************************//**
 * \file   App.cpp
 * \brief  アプリケーションクラス
 * 
 * \author Hiroto Kuge
 * \date   November 2022
 *********************************************************************/

//=============================================================================
// Includes
//=============================================================================
#include "../stdafx.h"
#include "App.h"
#include "../Direct3D12/TrianglePolygon.h"


//-----------------------------------------------------------------------------
// スタティック　メンバー
//-----------------------------------------------------------------------------
const TCHAR* Application::WINDOW_TITLE = L"DirectX12";
const TCHAR* Application::WINDOW_CLASS_NAME = L"win32app";

const uint32_t Application::WINDOW_STYLE_WINDOWED = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
const uint32_t Application::WINDOW_EX_STYLE_WINDOWED = (0);
const uint32_t Application::WINDOW_STYLE_FULL_SCREEN = (WS_VISIBLE | WS_POPUP);
const uint32_t Application::WINDOW_EX_STYLE_FULL_SCREEN = (0);

const uint32_t Application::CLIENT_WIDTH = 1280;
const uint32_t Application::CLIENT_HEIGHT = 720;

uint32_t Application::SYSTEM_WIDTH = 0;
uint32_t Application::SYSTEM_HEIGHT = 0;

const float	Application::FPS = 60;


// 適当に置いたディレクショナルライト
struct DirectionalLight
{
	Matrix viewProjInvMatrix;
	Vector3  color;
	float pad0;         // パディング
	Vector3  direction;
	float pad1;         // パディング
	Vector3 eyePos;     // 視点
	float specPow;      // スペキュラの絞り
};

// 関数宣言
void InitRootSignature(RootSignature& rs);


/**************************************************//**
 * ￥brief　コンストラクタ
 ******************************************************/
Application::Application() :mSystemCounter(0) {}

/**************************************************//**
 * ￥brief　デストラクタ
 ******************************************************/
Application::~Application() {
	Dispose();
}

/**************************************************//**
 * ￥brief インスタンス取得(singleton)
 * \return インスタンスアドレス
 ******************************************************/
Application* Application::Instance() {
	static Application instance;
	return &instance;
}

/**************************************************//**
 * ￥brief システム領域の幅と高さの初期化
 ******************************************************/
void Application::InitSystemWH() {

}

/**************************************************//**
 * ￥brief	初期化処理
 * \param hInstance インスタンスハンドル
 * \return 成功/失敗
 ******************************************************/
bool Application::Init(HINSTANCE hInstance) {
	// メモリーリークを検出
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//ウィンドウクラスのインスタンス取得
	m_pWindow = Window::GetInstance();

	//ウィンドウクラス登録
	m_pWindow->RegisterClass(hInstance, WINDOW_CLASS_NAME, CS_OWNDC);

	//ウィンドウ情報セット
	m_pWindow->SetWindow(hInstance,
		WINDOW_STYLE_WINDOWED,
		WINDOW_EX_STYLE_WINDOWED,
		NULL,
		CLIENT_WIDTH + SYSTEM_WIDTH,
		CLIENT_HEIGHT + SYSTEM_HEIGHT,
		WINDOW_CLASS_NAME,
		WINDOW_TITLE,
		false);

	//ウィンドウハンドル取得
	mHWnd = m_pWindow->GetHandle();

	//インスタンスハンドル格納
	mHInst = hInstance;

	
	//DirectX12初期化
	GraphicsEngine::GetInstance()->Init(mHWnd,CLIENT_WIDTH,CLIENT_HEIGHT);
	
	//Test:カメラ初期化
	GraphicsEngine::GetInstance()->GetCamera3D()->SetPosition({ 0.0f, 120.0f, 300.0f });
	GraphicsEngine::GetInstance()->GetCamera3D()->SetTarget({ 0.0f, 120.0f, 0.0f });
	
	return true;
}

/**************************************************//**
 * ￥brief 終了処理
 ******************************************************/
void Application::Dispose() {
	//// 標準出力クローズ
	//fclose(m_fp);
	//// コンソール開放
	//::FreeConsole();
}

/**************************************************//**
 * ￥brief　メインループ
 * \return メッセージID
 ******************************************************/
unsigned long Application::MainLoop(){
	//メッセージID格納用
	MSG	msg;
	ZeroMemory(&msg, sizeof(msg));

	clock_t current_time = 0;	//アプリケーション開始からの現在の経過時間
	clock_t last_time = 0;		//アプリケーション開始からの前回フレームの開始時間

	//TEST 各種初期化処理
	// ルートシグネチャを作成
	RootSignature rootSignature;
	InitRootSignature(rootSignature);

	// 定数バッファを作成
	ConstantBuffer cb;
	cb.Init(sizeof(Matrix));

	// 三角形ポリゴンを定義
	TrianglePolygon triangle;
	triangle.Init(rootSignature);

	// 三角形ポリゴンにUV座標を設定
	triangle.SetUVCoord(
		0,
		0.0f,
		1.0f
	);

	triangle.SetUVCoord(
		1,
		0.5f,
		0.0f
	);

	triangle.SetUVCoord(
		2,
		1.0f,
		1.0f
	);

	// テクスチャをロード
	Texture tex;
	tex.InitFromDDSFile(L"assets/image/default.DDS");

	// ディスクリプタヒープを作成
	DescriptorHeap ds;
	ds.RegistConstantBuffer(0, cb); // ディスクリプタヒープに定数バッファを登録

	// テクスチャをディスクリプタヒープに登録
	ds.RegistShaderResource(0, tex);
		
	ds.Commit(); //ディスクリプタヒープへの登録を確定
	
	auto& renderContext = GraphicsEngine::GetInstance()->GetRenderContext();

	while (m_pWindow->ExecMessage()) {
		
		// ゲームの更新処理：ゲーム層
		//!{
				/* ここにゲームの更新処理を記述してください*/
		
		//!} 
		// 描画オブジェクトのリストを生成：インターフェース層
		//!{
				/* ここにインターフェース層の処理を記述してください*/
		//!} 

		// 描画の実際の処理を発行：レンダー層
		//!{
		GraphicsEngine::GetInstance()->BeginRender();
		
		
		// ルートシグネチャを設定
		renderContext.SetRootSignature(rootSignature);

		// ワールド行列を作成
		Matrix mWorld;

		// ワールド行列をグラフィックメモリにコピー
		cb.CopyToVRAM(mWorld);

		//ディスクリプタヒープを設定
		renderContext.SetDescriptorHeap(ds);

		//三角形をドロー
		triangle.Draw(renderContext);
		
		GraphicsEngine::GetInstance()->EndRender();

		//!} 

	}

	//ゲームの終了処理

	int a = 0;

	return m_pWindow->GetMessage();
}

/**************************************************//**
 * ￥brief	ウィンドウハンドル取得
 * \return ウィンドウハンドル
 ******************************************************/
HWND Application::GetHWnd() { return mHWnd; }

/**************************************************//**
 * ￥brief インスタンスハンドル取得
 * \return インスタンスハンドル
 ******************************************************/
HINSTANCE Application::GetHInst() { return mHInst; }



// ルートシグネチャの初期化
void InitRootSignature(RootSignature& rs){
	rs.Init(D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP);
}
