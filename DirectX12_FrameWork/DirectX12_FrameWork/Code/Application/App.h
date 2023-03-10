/*****************************************************************//**
 * \file   App.h
 * \brief  アプリケーションクラス
 * 
 * \author Hiroto Kuge
 * \date   November 2022
 *********************************************************************/
#pragma once

//=============================================================================
// Includes
//=============================================================================
#include <Windows.h>
#include <stdio.h>
#include <cinttypes>
#include <wrl/client.h>
#include <memory>
#include "Window.h"


class Application {
public:
	static const TCHAR* WINDOW_TITLE;			// ウィンドウのタイトル
	static const TCHAR* WINDOW_CLASS_NAME;		// = "win32app";

	static const uint32_t		WINDOW_STYLE_WINDOWED;			// = (WS_VISIBLE | WS_CAPTION | WS_SYSMENU);
	static const uint32_t		WINDOW_EX_STYLE_WINDOWED;		// = (0);
	static const uint32_t		WINDOW_STYLE_FULL_SCREEN;		// = (WS_VISIBLE | WS_POPUP);
	static const uint32_t		WINDOW_EX_STYLE_FULL_SCREEN;	// = (0);

	static const uint32_t		CLIENT_WIDTH;	// クライアント領域 幅
	static const uint32_t		CLIENT_HEIGHT;	// クライアント領域 高さ

	static uint32_t				SYSTEM_WIDTH;	// = 0;
	static uint32_t				SYSTEM_HEIGHT;	// = 0;

	static const float			FPS;// = 60;

	virtual ~Application();		// デストラクタ
	static void InitSystemWH();	// システム幅高さ初期化

	bool Init(HINSTANCE hInstance);		// システム有効化
	void Dispose();						// システム無効化
	unsigned long MainLoop();			// メインループ

	// メンバ取得関数
	static Application* Instance();		// インスタンス取得
	HWND				GetHWnd();		// ウィンドウハンドル
	HINSTANCE			GetHInst();		// インスタンスハンドル

private:
	HWND		mHWnd;				// Windowハンドル
	HINSTANCE	mHInst;				// インスタンスハンドル
	uint32_t	mSystemCounter;		// システムカウンタ

	FILE* mfp;		// デバッグ用コンソール

	Window* m_pWindow;
	
	Application();									// コンストラクタ
	Application(const Application&);				// コピー
	Application& operator = (const Application&) {}	// =
		
};
