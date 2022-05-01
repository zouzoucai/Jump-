/*==============================================================================

	�Q�[���̐������ԊǗ��@[Game_Timer.h]
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
	RANK_NONE,					// ���J���i�v���C�ł��Ȃ��j
	RANK_OPEN,					// �@�[�@�i���N���A�j
	RANK_0,						// �������i��Ԓ����A�c�莞�ԁj
	RANK_1,						// ������
	RANK_2,						// ������
	RANK_3,						// �������i��ԒZ���A�x�X�g�N���A�j
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