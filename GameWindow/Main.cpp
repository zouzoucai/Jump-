/*==============================================================================

	���C���@[Main.cpp]

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
	�萔��`
------------------------------------------------------------------------------*/
#define CLASS_NAME		" H�ER�EC"				// �E�C���h�E�N���X�̖��O
#define WINDOW_CAPTION	" H�ER�EC"				// �E�C���h�E�̖��O



/*------------------------------------------------------------------------------
	�v���g�^�C�v�錾
------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// �Q�[���̏������֐�
// �߂�l:�������Ɏ��s�����Ƃ�false
static bool Initialize(void);
// �Q�[���̏I������
static void Finalize(void);
// �Q�[���̍X�V�֐�
static void Update(void);
// �Q�[���̕`�揈��
static void Draw(void);

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
static HWND g_hWnd = NULL;						// �E�C���h�E�n���h��

static int g_FrameCount = 0;
static int g_BaseFrame = 0;
static double g_BaseTime = 0;
static double g_FPS = 0.0;
static double g_ReserveTime = 0.0;


/*------------------------------------------------------------------------------
	���C��
------------------------------------------------------------------------------*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// �E�C���h�E�N���X�\���̂̌���
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = CLASS_NAME; 
	wc.hInstance = hInstance; 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); 

	RegisterClass(&wc);

	// �E�C���h�E�̃X�^�C��
	DWORD window_style = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX;
	// DWORD window_style = WS_OVERLAPPEDWINDOW &~ (WS_THICKFRAME | WS_MAXIMIZEBOX);

	// �s���̂����E�C���h�E�T�C�Y�̎Z�o
	RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	AdjustWindowRect(&window_rect, window_style, FALSE);

	int window_width  = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	// �\������E�C���h�E�̏ꏊ���Z�o����
	// �f�X�N�g�b�v�i�N���C�A���g�̈�j�̎擾

	// �f�o�C�X�i�V�X�e���j���烂�j�^�[�𑜓x���擾
	// �v���C�}�����j�^�[�̉�ʉ𑜓x�̎擾
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	int window_x = max((desktop_width - window_width) / 2 , 0);
	int window_y = max((desktop_height - window_height) / 2, 0);

	// �E�C���h�E�̐���
	g_hWnd = CreateWindow(
		CLASS_NAME,
		WINDOW_CAPTION,
		window_style,	// �E�C���h�E�X�^�C��(�r�b�g�t���O)

		window_x,
		window_y,
		// �s���̂����E�C���h�E�T�C�Y��ݒ�
		window_width,
		window_height,

		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (g_hWnd == NULL) {
		// �E�C���h�E�n���h�������炩�̗��R�Ő����ł��Ȃ�����
		return 0;//	�A�v���P�[�V�����̏I��
	}

	// �w��̃E�C���h�E�n���h���̃E�C���h�E���w��̕��@�ŕ\��
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	SystemTimer_Initialize();

	g_ReserveTime = g_BaseTime = SystemTimer_GetTime();
	g_FrameCount = g_BaseFrame = 0;
	g_FPS = 0.0;

	// �Q�[���̏�����
	if (!Initialize()) {
		// ���b�Z�[�W�{�b�N�X�Ŏ��s�������ߏI������|��`����
		MessageBox(NULL, "�������Ɏ��s�������߃A�v���P�[�V�������I�����܂��B", "�G���[", MB_OK);
		return 0;	// �A�v���P�[�V�����I��
	}

	// Windows�Q�[���p���C�����[�v
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			// ���b�Z�[�W������ꍇ�̓��b�Z�[�W�D��
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			// �����ŃQ�[������
			double time = SystemTimer_GetTime();
			if ((time - g_ReserveTime) < 1.0f / 60.0f) {
				Sleep(0);	// CPU���x�܂���
				continue;
			}

			g_ReserveTime = time;

			Update();
			Draw();

			// �V�[���̐؂�ւ�����
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
		if (MessageBox(hWnd, "�{���ɏI�����Ă�낵���ł����H", "�m�F", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	};

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// �Q�[���̏������֐�
// �߂�l:�������Ɏ��s�����Ƃ�false
bool Initialize(void)
{
	// �L�[�{�[�h�̏�����
	Keyboard_Initialize();

	// �L�[���K�[�̏�����
	Keylogger_Initialize();

	// �R���g���[���[�̏�����
	Controller_Initialize();

	// �T�E���h�̏�����
	InitSound(g_hWnd);

	if (!MyDirect3D_Initialize(g_hWnd)) {
		MessageBox(NULL, "Direct3D�̏������Ɏ��s���܂����B", "�G���[", MB_OK);
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
// �Q�[���̏I������
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
// �Q�[���̍X�V�֐�
void Update(void)
{
	Keylogger_Update();

	Controller_Update();

	Scene_Update();

	Fade_Update();

	// �v��
	double time = SystemTimer_GetTime();
	if (time - g_BaseTime > 1.0) {
		g_FPS = (g_FrameCount - g_BaseFrame) / (time - g_BaseTime);
		g_BaseTime = time;
		g_BaseFrame = g_FrameCount;
	}
	g_FrameCount++;
}
// �Q�[���̕`�揈��
void Draw(void)
{
	// 1���ϐ��Ɋi�[
	LPDIRECT3DDEVICE9 pDevice = MyDirect3D_GetDevice();
	// �O�̂���pDevice��NULL�łȂ����Ƃ��m�F
	if (!pDevice) {
		return;
	}

	// ��ʂ̃N���A
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(222, 184, 135, 255), 1.0f, 0);

	// �`��o�b�`���߂̊J�n
	pDevice->BeginScene();
	
	// �Q�[���̕`��
	Scene_Draw();

	Fade_Draw();

#if defined(_DEBUG) || defined(DEBUG)
	// FPS�\��
	char buf[64];
	sprintf(buf, "FPS = %.2f", g_FPS);
	DebugFont_Draw(0.0f, 0.0f, buf);
#endif // _DEBUG || DEBUG

	// �`��o�b�`���߂̏I��
	pDevice->EndScene();

	// �o�b�N�t���b�v(�^�C�~���O��D3DPRESENT_INTERVAL_DEFAULT)
	pDevice->Present(NULL, NULL, NULL, NULL);

}