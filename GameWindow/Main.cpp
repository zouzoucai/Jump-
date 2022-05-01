/*==============================================================================

	メイン　[Main.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 26
==============================================================================*/
#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include <stdio.h>
#include <stdlib.h>

#include "Config.h"
#include "debug_font.h"
#include "my_font.h"
#include "Mydirect3d.h"
#include "system_timer.h"
#include "Sprite.h"
#include "keyboard.h"
#include "Keylogger.h"
#include "sound.h"
#include "Scene.h"
#include "Fade.h"
#include "Controller.h"
#include "Game_Timer.h"

/*------------------------------------------------------------------------------
	定数定義
------------------------------------------------------------------------------*/
#define CLASS_NAME		" H・R・C"				// ウインドウクラスの名前
#define WINDOW_CAPTION	" H・R・C"				// ウインドウの名前



/*------------------------------------------------------------------------------
	プロトタイプ宣言
------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// ゲームの初期化関数
// 戻り値:初期化に失敗したときfalse
static bool Initialize(void);
// ゲームの終了処理
static void Finalize(void);
// ゲームの更新関数
static void Update(void);
// ゲームの描画処理
static void Draw(void);

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static HWND g_hWnd = NULL;						// ウインドウハンドル

static int g_FrameCount = 0;
static int g_BaseFrame = 0;
static double g_BaseTime = 0;
static double g_FPS = 0.0;
static double g_ReserveTime = 0.0;


/*------------------------------------------------------------------------------
	メイン
------------------------------------------------------------------------------*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ウインドウクラス構造体の決定
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME; 
	wc.hInstance = hInstance; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); 

	RegisterClass(&wc);

	// ウインドウのスタイル
	DWORD window_style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	// DWORD window_style = WS_OVERLAPPEDWINDOW &~ (WS_THICKFRAME | WS_MAXIMIZEBOX);

	// 都合のいいウインドウサイズの算出
	RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	AdjustWindowRect(&window_rect, window_style, FALSE);

	int window_width  = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	// 表示するウインドウの場所を算出する
	// デスクトップ（クライアント領域）の取得

	// デバイス（システム）からモニター解像度を取得
	// プライマリモニターの画面解像度の取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	int window_x = max((desktop_width - window_width) / 2 , 0);
	int window_y = max((desktop_height - window_height) / 2, 0);

	// ウインドウの生成
	g_hWnd = CreateWindow(
		CLASS_NAME,
		WINDOW_CAPTION,
		window_style,	// ウインドウスタイル(ビットフラグ)

		window_x,
		window_y,
		// 都合のいいウインドウサイズを設定
		window_width,
		window_height,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (g_hWnd == NULL) {
		// ウインドウハンドルが何らかの理由で生成できなかった
		return 0;//	アプリケーションの終了
	}

	// 指定のウインドウハンドルのウインドウを指定の方法で表示
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	SystemTimer_Initialize();

	g_ReserveTime = g_BaseTime = SystemTimer_GetTime();
	g_FrameCount = g_BaseFrame = 0;
	g_FPS = 0.0;

	// ゲームの初期化
	if (!Initialize()) {
		// メッセージボックスで失敗したため終了する旨を伝える
		MessageBox(NULL, "初期化に失敗したためアプリケーションを終了します。", "エラー", MB_OK);
		return 0;	// アプリケーション終了
	}

	// Windowsゲーム用メインループ
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// メッセージがある場合はメッセージ優先
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// ここでゲーム処理
			double time = SystemTimer_GetTime();
			if ((time - g_ReserveTime) < 1.0f / 60.0f) {
				Sleep(0);	// CPUを休ませる
				continue;
			}

			g_ReserveTime = time;

			Update();
			Draw();

			// シーンの切り替え処理
			Scene_ExecuteChange();
		}
	}

	Finalize();

	return (int)msg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
    case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	case WM_ACTIVATEAPP:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard_ProcessMessage(uMsg, wParam, lParam);
        break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	};

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// ゲームの初期化関数
// 戻り値:初期化に失敗したときfalse
bool Initialize(void)
{
	// キーボードの初期化
	Keyboard_Initialize();

	// キーロガーの初期化
	Keylogger_Initialize();

	// コントローラーの初期化
	Controller_Initialize();

	// サウンドの初期化
	InitSound(g_hWnd);

	if (!MyDirect3D_Initialize(g_hWnd)) {
		MessageBox(NULL, "Direct3Dの初期化に失敗しました。", "エラー", MB_OK);
		return false;
	}

	Fade_Initialize();

	Scene_Initialize();

	Sprite_Initialize();

	DebugFont_Initialize();

	MyFont_Initialize();

	GameTimer_LoadRank();

	return true;
}
// ゲームの終了処理
void Finalize(void)
{
	Scene_Finalize();

	GameTimer_SaveRank();

	Fade_Finalize();

	Sprite_Finalize();

	DebugFont_Finalize();

	MyFont_Finalize();

	MyDirect3D_Finalize();

	UninitSound();

	Controller_Finalize();

	Keylogger_Finalize();
}
// ゲームの更新関数
void Update(void)
{
	Keylogger_Update();

	Controller_Update();

	Scene_Update();

	Fade_Update();

	// 計測
	double time = SystemTimer_GetTime();
	if (time - g_BaseTime > 1.0) {
		g_FPS = (g_FrameCount - g_BaseFrame) / (time - g_BaseTime);
		g_BaseTime = time;
		g_BaseFrame = g_FrameCount;
	}
	g_FrameCount++;
}
// ゲームの描画処理
void Draw(void)
{
	// 1次変数に格納
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// 念のためpDeviceがNULLでないことを確認
	if (!pDevice) {
		return;
	}

	// 画面のクリア
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(222, 184, 135, 255), 1.0f, 0);

	// 描画バッチ命令の開始
	pDevice->BeginScene();
	
	// ゲームの描画
	Scene_Draw();

	Fade_Draw();

#if defined(_DEBUG) || defined(DEBUG)
	// FPS表示
	char buf[64];
	sprintf(buf, "FPS = %.2f", g_FPS);
	DebugFont_Draw(0.0f, 0.0f, buf);
#endif // _DEBUG || DEBUG

	// 描画バッチ命令の終了
	pDevice->EndScene();

	// バックフリップ(タイミングはD3DPRESENT_INTERVAL_DEFAULT)
	pDevice->Present(NULL, NULL, NULL, NULL);

}