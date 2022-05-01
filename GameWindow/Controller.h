/*==============================================================================

	�R���g���[���[���͂̏����@[Controller.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <windows.h>

// XInput�iXbox�j�̏ꍇ�̊e��{�^���̒l
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

// XInput�̏ꍇ�̃g���K�[���쓮����l�̋��E�i�������l�j
#define XINPUT_TRIGGER_THRESHOLD_LEFT		(0x8fff)
#define XINPUT_TRIGGER_THRESHOLD_RIGHT		(0x6fff)
#define XINPUT_TRIGGER_THRESHOLD			(0x1000)

// DierctInput�iDUALSHOCK4�j�̏ꍇ�̊e��{�^���̒l
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

// �����{�^���̒l
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

// �{�^���������Ă���
bool Controller_ButtonPress(int button);

// �{�^�����������u��
bool Controller_ButtonTrigger(int button);

// �{�^���𗣂����u��
bool Controller_ButtonRelease(int button);

// XInput�̃g���K�[�̏��
DWORD Controller_XInputGetTrigger(void);

// ���g���K�[�̏��
DWORD Controller_DS4GetLeftTrigger(void);

// �E�g���K�[�̏��
DWORD Controller_DS4GetRightTrigger(void);

// ���X�e�B�b�N�̏��(x����)
DWORD Controller_GetLeftStickX(void);

// ���X�e�B�b�N�̏��(y����)
DWORD Controller_GetLeftStickY(void);

// �E�X�e�B�b�N�̏��
DWORD Controller_GetRightStick(void);

// �����{�^���������Ă���
bool Controller_DirectionButtonPress(DWORD button);

// �����{�^�����������u��
bool Controller_DirectionButtonTrigger(DWORD button);

// �����{�^���𗣂����u��
bool Controller_DirectionButtonRelease(DWORD button);

// �����{�^���̏�Ԃ�Ԃ�
DWORD Controller_GetDirectionButton(void);

// ����or�W�����v����
bool Controller_Decision(void);

// �L�����Z��
bool Controller_Cancel(void);

// ���ړ�
bool Controller_MoveLeft(void);

// �E�ړ�
bool Controller_MoveRight(void);

// ����]
bool Controller_TurnLeft(void);

// �E��]
bool Controller_TurnRight(void);

// ���j���[
bool Controller_Menu(void);

// ��ɃJ�[�\���ړ�
bool Controller_Up(void);

// ���ɃJ�[�\���ړ�
bool Controller_Down(void);

// ���ɃJ�[�\���ړ�
bool Controller_Left(void);

// �E�ɃJ�[�\���ړ�
bool Controller_Right(void);

// �����{�^�����g���C
bool Controller_Retry(void);

// �R���g���[���g���Ă邩�ǂ���
bool Controller_isGamePad(void);

#endif // CONTROLLER_H_