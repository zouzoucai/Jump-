/*==============================================================================

	ゲームのエフェクト処理　[Game_Effect.h]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 10 / 20
==============================================================================*/
#ifndef GAME_EFFECT_H_
#define GAME_EFFECT_H_

void GameEffect_Initialize(void);
void GameEffect_Finalize(void);
void GameEffect_Update(void);
void GameEffect_Draw(void);

void GameEffect_Spawn(float x, float y);


#endif // GAME_EFFECT_H_