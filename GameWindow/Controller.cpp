/*==============================================================================

	コントローラー入力の処理　[Controller.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#include "Controller.h"
#include "Keylogger.h"
//#include <mmsystem.h>

#define BUTTON_MAX	(16)

// スティックが作動する値の最小値（しきい値）
#define STICK_THRESHOLD_LEFT	(0x6fff)
#define STICK_THRESHOLD_RIGHT	(0x8fff)
#define STICK_THRESHOLD_UP		(0x6fff)
#define STICK_THRESHOLD_DOWN	(0x8fff)

// 連続入力時に用いる定数
#define CONTINUITY_TRUE_FRAME			(30)		// 連続入力発生フレーム
#define CONTINUITY_SPEEDUP_FRAME		(90)		// 連続入力加速開始フレーム
#define CONTINUITY_INTERVAL_FRAME1		(10)		// 連続入力フレーム間隔1
#define CONTINUITY_INTERVAL_FRAME2		(5)			// 連続入力フレーム間隔2

typedef enum Controll_Mode_tag {
	MODE_KEYBOARD,		// キーボード
	MODE_XINPUT,		// Xbox系統（ロジクールXInputモード）
	MODE_DIRECTINPUT,	// PS4系統 （ロジクールDirectInputモード、ただしロジクールの場合トリガーの強弱判定無し）
}Controll_Mode;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
JOYINFOEX g_JoyInfoEx;
JOYINFOEX g_JoyInfoExOld;

// 操作モード
static Controll_Mode g_mode;

static int g_stick_up_frame = 0;
static int g_stick_down_frame = 0;
static int g_stick_left_frame = 0;
static int g_stick_right_frame = 0;

static int g_button_up_frame = 0;
static int g_button_down_frame = 0;
static int g_button_left_frame = 0;
static int g_button_right_frame = 0;

void Controller_Initialize(void)
{
	g_JoyInfoEx.dwSize = sizeof(JOYINFOEX);
	g_JoyInfoEx.dwFlags = JOY_RETURNALL;

	if (JOYERR_NOERROR != joyGetPosEx(0, &g_JoyInfoEx)) { //0番のジョイスティックの情報を見る
		g_mode = MODE_KEYBOARD;
		return;
	}
	
	// XInputかDirectInputかを初期値で判断
	if (g_JoyInfoEx.dwUpos == 0x7fff && g_JoyInfoEx.dwVpos == 0x0) {
		g_mode = MODE_XINPUT;
	}
	else {
		g_mode = MODE_DIRECTINPUT;
	}

	g_stick_up_frame = 0;
	g_stick_down_frame = 0;
	g_stick_left_frame = 0;
	g_stick_right_frame = 0;

	g_button_up_frame = 0;
	g_button_down_frame = 0;
	g_button_left_frame = 0;
	g_button_right_frame = 0;
}

void Controller_Finalize(void)
{
	
}

void Controller_Update(void)
{
	if (g_mode != MODE_KEYBOARD) {
		g_JoyInfoExOld = g_JoyInfoEx;
	}
	else {
		return;
	}

	if (JOYERR_NOERROR != joyGetPosEx(0, &g_JoyInfoEx)) { //0番のジョイスティックの情報を見る
		g_mode = MODE_KEYBOARD;
	}
}

// ボタンを押してたら
bool Controller_ButtonPress(int button) 
{
	return g_JoyInfoEx.dwButtons & button;
}

// ボタンを押した瞬間
bool Controller_ButtonTrigger(int button)
{
	return ((g_JoyInfoExOld.dwButtons ^ g_JoyInfoEx.dwButtons) & g_JoyInfoEx.dwButtons) & button;
}

// ボタンを離した瞬間
bool Controller_ButtonRelease(int button)
{
	return ((g_JoyInfoExOld.dwButtons ^ g_JoyInfoEx.dwButtons) & g_JoyInfoExOld.dwButtons) & button;
}

// XInputのトリガーの状態
DWORD Controller_XInputGetTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// DUALSHOCK4の左トリガーの状態()
DWORD Controller_DS4GetLeftTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// DUALSHOCK4の右トリガーの状態
DWORD Controller_DS4GetRightTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// 左スティックの状態(x方向)
DWORD Controller_GetLeftStickX(void)
{
	return g_JoyInfoEx.dwXpos;
}

// 左スティックの状態(y方向)
DWORD Controller_GetLeftStickY(void)
{
	return g_JoyInfoEx.dwYpos;
}

// 右スティックの状態
DWORD Controller_GetRightStick(void)
{
	return g_JoyInfoEx.dwUpos;
}

// 方向ボタンを押してたら
bool Controller_DirectionButtonPress(DWORD button)
{
	return g_JoyInfoEx.dwPOV == button;
}

// 方向ボタンを押した瞬間
bool Controller_DirectionButtonTrigger(DWORD button)
{
	return g_JoyInfoExOld.dwPOV != button && g_JoyInfoEx.dwPOV == button;
}

// 方向ボタンを離した瞬間
bool Controller_DirectionButtonRelease(DWORD button)
{
	return g_JoyInfoExOld.dwPOV == button && g_JoyInfoEx.dwPOV != button;
}

// 方向ボタンの状態を返す
DWORD Controller_GetDirectionButton(void)
{
	return g_JoyInfoEx.dwPOV;
}

/*------------------------------------------------------------------------------
	以下は実際の操作で使う関数
------------------------------------------------------------------------------*/

// 決定orジャンプ操作
bool Controller_Decision(void)
{
	switch (g_mode)
	{
	case MODE_KEYBOARD:
		return Keylogger_Trigger(KL_DECISION);
	case MODE_XINPUT:
		return Controller_ButtonTrigger(XINPUT_BUTTON_B) || Keylogger_Trigger(KL_DECISION);
	case MODE_DIRECTINPUT:
		return Controller_ButtonTrigger(DS4_BUTTON_MARU) || Keylogger_Trigger(KL_DECISION);
	default:
		return false;
	}
}

// キャンセル
bool Controller_Cancel(void)
{
	switch (g_mode)
	{
	case MODE_KEYBOARD:
		return Keylogger_Trigger(KL_CANCEL);
	case MODE_XINPUT:
		return Controller_ButtonTrigger(XINPUT_BUTTON_A) || Keylogger_Trigger(KL_CANCEL);
	case MODE_DIRECTINPUT:
		return Controller_ButtonTrigger(DS4_BUTTON_BATSU) || Keylogger_Trigger(KL_CANCEL);
	default:
		return false;
	}
}

// 左移動
bool Controller_MoveLeft(void)
{
	if (g_mode != MODE_KEYBOARD) {
		return Controller_GetLeftStickX() < STICK_THRESHOLD_LEFT || Controller_DirectionButtonPress(BUTTON_LEFT)
			|| Keylogger_Press(KL_LEFT);
	}
	else {
		return Keylogger_Press(KL_LEFT);
	}
}

// 右移動
bool Controller_MoveRight(void)
{
	if (g_mode != MODE_KEYBOARD) {
		return Controller_GetLeftStickX() > STICK_THRESHOLD_RIGHT || Controller_DirectionButtonPress(BUTTON_RIGHT)
			|| Keylogger_Press(KL_RIGHT);
	}
	else {
		return Keylogger_Press(KL_RIGHT);
	}
}

// 左回転
bool Controller_TurnLeft(void)
{
	switch (g_mode)
	{
	case MODE_KEYBOARD:
		return Keylogger_Press(KL_LEFT);
	case MODE_XINPUT:
		return Controller_XInputGetTrigger() > XINPUT_TRIGGER_THRESHOLD_LEFT || Keylogger_Press(KL_LEFT);
	case MODE_DIRECTINPUT:
		return Controller_ButtonPress(DS4_BUTTON_L2) || Keylogger_Press(KL_LEFT);
	default:
		return false;
	}
}

// 右回転
bool Controller_TurnRight(void)
{
	switch (g_mode)
	{
	case MODE_KEYBOARD:
		return Keylogger_Press(KL_RIGHT);
	case MODE_XINPUT:
		return Controller_XInputGetTrigger() < XINPUT_TRIGGER_THRESHOLD_RIGHT || Keylogger_Press(KL_RIGHT);
	case MODE_DIRECTINPUT:
		return Controller_ButtonPress(DS4_BUTTON_R2) || Keylogger_Press(KL_RIGHT);
	default:
		return false;
	}
}

// メニューボタン
bool Controller_Menu(void)
{
	switch (g_mode)
	{
	case MODE_KEYBOARD:
		return Keylogger_Trigger(KL_MENU);
	case MODE_XINPUT:
		return Controller_ButtonTrigger(XINPUT_BUTTON_START) || Keylogger_Trigger(KL_MENU);
	case MODE_DIRECTINPUT:
		return Controller_ButtonTrigger(DS4_BUTTON_OPTIONS) || Keylogger_Trigger(KL_MENU);
	default:
		return false;
	}
}

// 上にカーソル移動
bool Controller_Up(void)
{
	if (g_mode == MODE_KEYBOARD) {
		return Keylogger_Trigger(KL_UP);
	}

	g_stick_up_frame = g_JoyInfoEx.dwYpos < STICK_THRESHOLD_UP ? g_stick_up_frame + 1 : 0;
	g_button_up_frame = Controller_DirectionButtonPress(BUTTON_UP) ? g_button_up_frame + 1 : 0;

	bool stick_up = false;
	bool button_up = false;
	int interval = 0;

	if (g_stick_up_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_stick_up_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_stick_up_frame % interval == 0) {
			stick_up = true;
		}
	}
	else {
		stick_up = (g_JoyInfoEx.dwYpos < STICK_THRESHOLD_UP && g_JoyInfoExOld.dwYpos >= STICK_THRESHOLD_UP);
	}

	if (g_button_up_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_button_up_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_button_up_frame % interval == 0) {
			button_up = true;
		}
	}
	
	return Controller_DirectionButtonTrigger(BUTTON_UP) || Keylogger_Trigger(KL_UP) || stick_up || button_up;
}

// 下にカーソル移動
bool Controller_Down(void)
{
	if (g_mode == MODE_KEYBOARD) {
		return Keylogger_Trigger(KL_DOWN);
	}

	g_stick_down_frame = g_JoyInfoEx.dwYpos > STICK_THRESHOLD_DOWN ? g_stick_down_frame + 1 : 0;
	g_button_down_frame = Controller_DirectionButtonPress(BUTTON_DOWN) ? g_button_down_frame + 1 : 0;

	bool stick_down = false;
	bool button_down = false;
	int interval = 0;

	if (g_stick_down_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_stick_down_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_stick_down_frame % interval == 0) {
			stick_down = true;
		}
	}
	else {
		stick_down = (g_JoyInfoEx.dwYpos > STICK_THRESHOLD_DOWN && g_JoyInfoExOld.dwYpos <= STICK_THRESHOLD_DOWN);
	}

	if (g_button_down_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_button_down_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_button_down_frame % interval == 0) {
			button_down = true;
		}
	}

	return Controller_DirectionButtonTrigger(BUTTON_DOWN) || Keylogger_Trigger(KL_DOWN) || stick_down || button_down;
}

// 左にカーソル移動
bool Controller_Left(void)
{
	if (g_mode == MODE_KEYBOARD) {
		return Keylogger_Trigger(KL_LEFT);
	}

	g_stick_left_frame = g_JoyInfoEx.dwXpos < STICK_THRESHOLD_LEFT ? g_stick_left_frame + 1 : 0;
	g_button_left_frame = Controller_DirectionButtonPress(BUTTON_LEFT) ? g_button_left_frame + 1 : 0;

	bool stick_left = false;
	bool button_left = false;
	int interval = 0;

	if (g_stick_left_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_stick_left_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_stick_left_frame % interval == 0) {
			stick_left = true;
		}
	}
	else {
		stick_left = (g_JoyInfoEx.dwXpos < STICK_THRESHOLD_LEFT && g_JoyInfoExOld.dwXpos >= STICK_THRESHOLD_LEFT);
	}

	if (g_button_left_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_button_left_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_button_left_frame % interval == 0) {
			button_left = true;
		}
	}

	return Controller_DirectionButtonTrigger(BUTTON_LEFT) || Keylogger_Trigger(KL_LEFT) || stick_left || button_left;
}

// 右にカーソル移動
bool Controller_Right(void)
{
	if (g_mode == MODE_KEYBOARD) {
		return Keylogger_Trigger(KL_RIGHT);
	}

	g_stick_right_frame = g_JoyInfoEx.dwXpos > STICK_THRESHOLD_RIGHT ? g_stick_right_frame + 1 : 0;
	g_button_right_frame = Controller_DirectionButtonPress(BUTTON_RIGHT) ? g_button_right_frame + 1 : 0;

	bool stick_right = false;
	bool button_right = false;
	int interval = 0;

	if (g_stick_right_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_stick_right_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_stick_right_frame % interval == 0) {
			stick_right = true;
		}
	}
	else {
		stick_right = (g_JoyInfoEx.dwXpos > STICK_THRESHOLD_RIGHT && g_JoyInfoExOld.dwXpos <= STICK_THRESHOLD_RIGHT);
	}

	if (g_button_right_frame > CONTINUITY_TRUE_FRAME) {
		interval = g_button_right_frame < CONTINUITY_SPEEDUP_FRAME ? CONTINUITY_INTERVAL_FRAME1 : CONTINUITY_INTERVAL_FRAME2;
		if (g_button_right_frame % interval == 0) {
			button_right = true;
		}
	}

	return Controller_DirectionButtonTrigger(BUTTON_RIGHT) || Keylogger_Trigger(KL_RIGHT) || stick_right || button_right;
}

// ワンボタンリトライ
bool Controller_Retry(void)
{
	switch (g_mode)
	{
	case MODE_XINPUT:
		return Controller_ButtonTrigger(XINPUT_BUTTON_BACK);
	case MODE_DIRECTINPUT:
		return Controller_ButtonTrigger(DS4_BUTTON_PAD) || Controller_ButtonTrigger(DS4_BUTTON_SHARE);
	default:
		return false;
	}
}

bool Controller_isGamePad(void)
{
	return g_mode != MODE_KEYBOARD;
}