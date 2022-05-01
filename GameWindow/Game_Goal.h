/*==============================================================================

	ÉQÅ[ÉÄÇÃÉSÅ[ÉãèàóùÅ@[Game_Goal.h]

													Author : Sai Hakusen
													Date   : 2020 / 12 / 26
==============================================================================*/
#ifndef GAME_GOAL_H_
#define GAME_GOAL_H_

void GameGoal_Initialize(void);
void GameGoal_Finalize(void);
bool GameGoal_Update(void);
void GameGoal_Draw(void);

void GameGoal_ToEnable(void);
void GameGoal_ToDisenable(void);
int GameGoal_GetNowStage(void);
#endif // GAME_GOAL_H_