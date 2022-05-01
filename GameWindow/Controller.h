/*==============================================================================

	コントローラー入力の処理　[Controller.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <windows.h>

// XInput（Xbox）の場合の各種ボタンの値
typedef enum XInput_Buttons_tag
{
	XINPUT_BUTTON_A = 0x1,
	XINPUT_BUTTON_B = 0x2,
	XINPUT_BUTTON_X = 0x4,
	XINPUT_BUTTON_Y = 0x8,
	XINPUT_BUTTON_LB = 0x10,
	XINPUT_BUTTON_RB = 0x20,
	XINPUT_BUTTON_BACK = 0x40,
	XINPUT_BUTTON_START = 0x80,
	XINPUT_BUTTON_L_STICK = 0x100,
	XINPUT_BUTTON_R_STICK = 0x200,
}XInput_Buttons;

// XInputの場合のトリガーが作動する値の境界（しきい値）
#define XINPUT_TRIGGER_THRESHOLD_LEFT		(0x8fff)
#define XINPUT_TRIGGER_THRESHOLD_RIGHT		(0x6fff)
#define XINPUT_TRIGGER_THRESHOLD			(0x1000)

// DierctInput（DUALSHOCK4）の場合の各種ボタンの値
typedef enum DS4_Buttons_tag
{
	DS4_BUTTON_SHIKAKU = 0x1,
	DS4_BUTTON_BATSU = 0x2,
	DS4_BUTTON_MARU = 0x4,
	DS4_BUTTON_SANKAKU = 0x8,
	DS4_BUTTON_L1 = 0x10,
	DS4_BUTTON_R1 = 0x20,
	DS4_BUTTON_L2 = 0x40,
	DS4_BUTTON_R2 = 0x80,
	DS4_BUTTON_SHARE = 0x100,
	DS4_BUTTON_OPTIONS = 0x200,
	DS4_BUTTON_L_STICK = 0x400,
	DS4_BUTTON_R_STICK = 0x800,
	DS4_BUTTON_PS = 0x1000,
	DS4_BUTTON_PAD = 0x2000,
}DS4_Buttons;

// 方向ボタンの値
typedef enum Direction_Buttons_tag
{
	BUTTON_UP = 0x0,
	BUTTON_UPRIGHT = 0x1194,
	BUTTON_RIGHT = 0x2328,
	BUTTON_DOWNRIGHT = 0x34bc,
	BUTTON_DOWN = 0x4650,
	BUTTON_DOWNLEFT = 0x57e4,
	BUTTON_LEFT = 0x6978,
	BUTTON_UPLEFT = 0x7b0c,
	BUTTON_NONE = 0xffff,
}Direction_Buttons;

void Controller_Initialize(void);
void Controller_Finalize(void);
void Controller_Update(void);

// ボタンを押してたら
bool Controller_ButtonPress(int button);

// ボタンを押した瞬間
bool Controller_ButtonTrigger(int button);

// ボタンを離した瞬間
bool Controller_ButtonRelease(int button);

// XInputのトリガーの状態
DWORD Controller_XInputGetTrigger(void);

// 左トリガーの状態
DWORD Controller_DS4GetLeftTrigger(void);

// 右トリガーの状態
DWORD Controller_DS4GetRightTrigger(void);

// 左スティックの状態(x方向)
DWORD Controller_GetLeftStickX(void);

// 左スティックの状態(y方向)
DWORD Controller_GetLeftStickY(void);

// 右スティックの状態
DWORD Controller_GetRightStick(void);

// 方向ボタンを押してたら
bool Controller_DirectionButtonPress(DWORD button);

// 方向ボタンを押した瞬間
bool Controller_DirectionButtonTrigger(DWORD button);

// 方向ボタンを離した瞬間
bool Controller_DirectionButtonRelease(DWORD button);

// 方向ボタンの状態を返す
DWORD Controller_GetDirectionButton(void);

// 決定orジャンプ操作
bool Controller_Decision(void);

// キャンセル
bool Controller_Cancel(void);

// 左移動
bool Controller_MoveLeft(void);

// 右移動
bool Controller_MoveRight(void);

// 左回転
bool Controller_TurnLeft(void);

// 右回転
bool Controller_TurnRight(void);

// メニュー
bool Controller_Menu(void);

// 上にカーソル移動
bool Controller_Up(void);

// 下にカーソル移動
bool Controller_Down(void);

// 左にカーソル移動
bool Controller_Left(void);

// 右にカーソル移動
bool Controller_Right(void);

// ワンボタンリトライ
bool Controller_Retry(void);

// コントローラ使ってるかどうか
bool Controller_isGamePad(void);

#endif // CONTROLLER_H_