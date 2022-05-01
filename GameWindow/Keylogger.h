/*==============================================================================

	キー入力＆キー入力の記録処理　[Keylogger.h]

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

// 押してたら
bool Keylogger_Press(KeyKind kl);

// 押した瞬間
bool Keylogger_Trigger(KeyKind kl);

// 離した瞬間
bool Keylogger_Release(KeyKind kl);

void Keylogger_RecordStart(int frame_max);
void Keylogger_RecordEnd(void);
void Keylogger_RecordLoad(void);
void Keylogger_RecordPlay(void);
bool Keylogger_IsRecordPlay(void);

#endif // KEYLOGGER_H_