//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_SE_SCREW_TURN_01,
	SOUND_LABEL_SE_SCREW_TURN_02,
	SOUND_LABEL_SE_SCREW_TURN_03,
	SOUND_LABEL_SE_SCREW_STOP,
	SOUND_LABEL_SE_SCREW_GOOD,
	SOUND_LABEL_SE_DOOR_OPEN,
	SOUND_LABEL_SE_JUMP,
	SOUND_LABEL_SE_CATCH,
	SOUND_LABEL_SE_DOOR_IN,
	SOUND_LABEL_SE_SELECT,
	SOUND_LABEL_SE_DECISION,
	SOUND_LABEL_SE_PAUSE,
	SOUND_LABEL_BGM_CLEAR1,
	SOUND_LABEL_BGM_CLEAR2,
	SOUND_LABEL_BGM_MENU,
	SOUND_LABEL_BGM_TITLE,
	SOUND_LABEL_BGM_GAME,
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);

#endif
