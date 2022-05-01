/*==============================================================================

	ゲームの制限時間管理　[Game_Timer.h]
													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 28
==============================================================================*/
#ifndef GAME_TIMER_H_
#define GAME_TIMER_H_

#define START_FRAME	(30)
#define RESET_FRAME	(15)

typedef enum PHASE_tag {
	PHASE_READY,
	PHASE_GAME,
	PHASE_RESET,
	PHASE_TIMEOVER,
	PHASE_CLEAR,
}PHASE;

typedef enum Rank_tag {
	RANK_NONE,					// 未開放（プレイできない）
	RANK_OPEN,					// 　ー　（未クリア）
	RANK_0,						// ☆☆☆（一番長い、残り時間）
	RANK_1,						// ★☆☆
	RANK_2,						// ★★☆
	RANK_3,						// ★★★（一番短い、ベストクリア）
}Rank;

void GameTimer_Initialize(void);
void GameTimer_Finalize(void);
void GameTimer_Update(void);
void GameTimer_Draw(void);

void GameTimer_Reset(void);
void GameTimer_Clear(void);

Rank* GameTimer_GetRank(void);
Rank GameTimer_GetNowRank(void);

void GameTimer_LoadRank(void);
void GameTimer_SaveRank(void);

#endif // GAME_TIMER_H_