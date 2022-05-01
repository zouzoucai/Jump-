/*==============================================================================

	�X�e�[�W�I�������@[StageSelect.h]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 14
==============================================================================*/
#ifndef STAGESELECT_H_
#define STAGESELECT_H_

#define STAGE_MAX	(21)	// �X�e�[�W���i�`���[�g���A���X�e�[�W���܂߂��X�e�[�W�ԍ��z��̍ő�v�f�ԍ��j
#include"Game_Timer.h"
void StageSelect_Initialize(void);
void StageSelect_Finalize(void);
void StageSelect_Update(void);
void StageSelect_Draw(void);

int StageSelect_GetStageNumber(void);

void StageSelect_ToNext(void);
void StageSelect_StageClear(int StageNumber);
void StageSelect_SetRank(Rank* rank);
#endif // STAGESELECT_H_