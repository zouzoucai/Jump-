/*==============================================================================

	�R���g���[���[���͂̏����@[Controller.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#include "Controller.h"
#include "Keylogger.h"
//#include <mmsystem.h>

#define BUTTON_MAX	(16)

// �X�e�B�b�N���쓮����l�̍ŏ��l�i�������l�j
#define STICK_THRESHOLD_LEFT	(0x6fff)
#define STICK_THRESHOLD_RIGHT	(0x8fff)
#define STICK_THRESHOLD_UP		(0x6fff)
#define STICK_THRESHOLD_DOWN	(0x8fff)

// �A�����͎��ɗp����萔
#define CONTINUITY_TRUE_FRAME			(30)		// �A�����͔����t���[��
#define CONTINUITY_SPEEDUP_FRAME		(90)		// �A�����͉����J�n�t���[��
#define CONTINUITY_INTERVAL_FRAME1		(10)		// �A�����̓t���[���Ԋu1
#define CONTINUITY_INTERVAL_FRAME2		(5)			// �A�����̓t���[���Ԋu2

typedef enum Controll_Mode_tag {
	MODE_KEYBOARD,		// �L�[�{�[�h
	MODE_XINPUT,		// Xbox�n���i���W�N�[��XInput���[�h�j
	MODE_DIRECTINPUT,	// PS4�n�� �i���W�N�[��DirectInput���[�h�A���������W�N�[���̏ꍇ�g���K�[�̋��㔻�薳���j
}Controll_Mode;

/*------------------------------------------------------------------------------
	�O���[�o���ϐ��錾
------------------------------------------------------------------------------*/
JOYINFOEX g_JoyInfoEx;
JOYINFOEX g_JoyInfoExOld;

// ���샂�[�h
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

	if (JOYERR_NOERROR != joyGetPosEx(0, &g_JoyInfoEx)) { //0�Ԃ̃W���C�X�e�B�b�N�̏�������
		g_mode = MODE_KEYBOARD;
		return;
	}
	
	// XInput��DirectInput���������l�Ŕ��f
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

	if (JOYERR_NOERROR != joyGetPosEx(0, &g_JoyInfoEx)) { //0�Ԃ̃W���C�X�e�B�b�N�̏�������
		g_mode = MODE_KEYBOARD;
	}
}

// �{�^���������Ă���
bool Controller_ButtonPress(int button) 
{
	return g_JoyInfoEx.dwButtons & button;
}

// �{�^�����������u��
bool Controller_ButtonTrigger(int button)
{
	return ((g_JoyInfoExOld.dwButtons ^ g_JoyInfoEx.dwButtons) & g_JoyInfoEx.dwButtons) & button;
}

// �{�^���𗣂����u��
bool Controller_ButtonRelease(int button)
{
	return ((g_JoyInfoExOld.dwButtons ^ g_JoyInfoEx.dwButtons) & g_JoyInfoExOld.dwButtons) & button;
}

// XInput�̃g���K�[�̏��
DWORD Controller_XInputGetTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// DUALSHOCK4�̍��g���K�[�̏��()
DWORD Controller_DS4GetLeftTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// DUALSHOCK4�̉E�g���K�[�̏��
DWORD Controller_DS4GetRightTrigger(void)
{
	return g_JoyInfoEx.dwZpos;
}

// ���X�e�B�b�N�̏��(x����)
DWORD Controller_GetLeftStickX(void)
{
	return g_JoyInfoEx.dwXpos;
}

// ���X�e�B�b�N�̏��(y����)
DWORD Controller_GetLeftStickY(void)
{
	return g_JoyInfoEx.dwYpos;
}

// �E�X�e�B�b�N�̏��
DWORD Controller_GetRightStick(void)
{
	return g_JoyInfoEx.dwUpos;
}

// �����{�^���������Ă���
bool Controller_DirectionButtonPress(DWORD button)
{
	return g_JoyInfoEx.dwPOV == button;
}

// �����{�^�����������u��
bool Controller_DirectionButtonTrigger(DWORD button)
{
	return g_JoyInfoExOld.dwPOV != button && g_JoyInfoEx.dwPOV == button;
}

// �����{�^���𗣂����u��
bool Controller_DirectionButtonRelease(DWORD button)
{
	return g_JoyInfoExOld.dwPOV == button && g_JoyInfoEx.dwPOV != button;
}

// �����{�^���̏�Ԃ�Ԃ�
DWORD Controller_GetDirectionButton(void)
{
	return g_JoyInfoEx.dwPOV;
}

/*------------------------------------------------------------------------------
	�ȉ��͎��ۂ̑���Ŏg���֐�
------------------------------------------------------------------------------*/

// ����or�W�����v����
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

// �L�����Z��
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

// ���ړ�
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

// �E�ړ�
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

// ����]
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

// �E��]
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

// ���j���[�{�^��
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

// ��ɃJ�[�\���ړ�
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

// ���ɃJ�[�\���ړ�
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

// ���ɃJ�[�\���ړ�
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

// �E�ɃJ�[�\���ړ�
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

// �����{�^�����g���C
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