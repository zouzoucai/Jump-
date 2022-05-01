/*==============================================================================

	ステージ選択処理　[StageSelect.h]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 14
==============================================================================*/
#ifndef STAGESELECT_H_
#define STAGESELECT_H_

#define STAGE_MAX	(21)	// ステージ数（チュートリアルステージも含めたステージ番号配列の最大要素番号）
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