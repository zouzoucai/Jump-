/*==============================================================================

	ゲームのポーズ処理　[Game_Pause.h]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 10
==============================================================================*/
#ifndef GAME_PAUSE_H_
#define GAME_PAUSE_H_

typedef enum PauseSelect_tag{
	PAUSE_NONE,
	PAUSE_BACK,
	PAUSE_RETRY,
	PAUSE_TOSTAGESELECT,
	PAUSE_TOMENU,
}PauseSelect;

void GamePause_Initialize(void);
void GamePause_Finalize(void);
void GamePause_Update(void);
void GamePause_Draw(void);

void GamePause_Set(void);

PauseSelect GamePause_GetSelect(void);

#endif // GAME_PAUSE_H_