/*==============================================================================

	�L�[���́��L�[���͂̋L�^�����@[Keylogger.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 12 / 25
==============================================================================*/
#ifndef KEYLOGGER_H_
#define KEYLOGGER_H_

typedef enum KeyKind_tag
{
	KL_UP,
	KL_DOWN,
	KL_LEFT,
	KL_RIGHT,
	KL_DECISION,
	KL_CANCEL,
	KL_ENTER,
	KL_MENU,
	KL_ESCAPE,
	KL_MAX
}KeyKind;

void Keylogger_Initialize(void);
void Keylogger_Finalize(void);
void Keylogger_Update(void);

// �����Ă���
bool Keylogger_Press(KeyKind kl);

// �������u��
bool Keylogger_Trigger(KeyKind kl);

// �������u��
bool Keylogger_Release(KeyKind kl);

void Keylogger_RecordStart(int frame_max);
void Keylogger_RecordEnd(void);
void Keylogger_RecordLoad(void);
void Keylogger_RecordPlay(void);
bool Keylogger_IsRecordPlay(void);

#endif // KEYLOGGER_H_