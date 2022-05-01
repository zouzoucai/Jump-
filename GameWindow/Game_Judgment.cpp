/*==============================================================================

	ゲームの各種判定処理　[Game_Judgment.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2020 / 12 / 28
									
==============================================================================*/
#include "Game_Judgment.h"
#include "Game_Player.h"

void GameJudgment_Initialize(void)
{
	
}

void GameJudgment_Update(void)
{
	GameJudgment_ScrewToDriver();
}

// ネジとドライバーとの当たり判定
int GameJudgment_ScrewToDriver(void) 
{
	// ネジとドライバーとの当たり判定が必要なのはプレイヤーがジャンプ状態の時のみであるためジャンプ状態以外ならreturn
	//if (GamePlayer_GetPlayer()->state != STATE_JUMP) {
	//	// 当たってないとみなして-1を返す
	//	return -1;
	//}

	// 歩行状態の時もネジにくっつくようにしたパターン
	// この場合ネジとドライバーとの当たり判定が必要ないのはプレイヤーが回転状態の時のみであるため回転状態ならreturn
	if (GamePlayer_GetPlayer()->state == STATE_TURN) {
		// 当たってないとみなして-1を返す
		return -1;
	}

	for (int i = 0; i < GameScrew_GetCount(); i++) {
		// お互いが有効か調べる(今のところ全部有効だけどいずれ一時的に無効になったり状態変わったりするかもしれんからこれは残す)
		if (!GameScrew_isEnable(i)) {
			continue;
		}

		// お互いが有効だったら当たり判定
		if (!CollisionCircleAndCircleHit(&GamePlayer_GetDriverCollision(), &GameScrew_GetCollision(i))) {
			continue;
		}

		// 当たってたらネジのインデックスを返す
		return i;
	}

	// 当たってなかったら-1を返す
	return -1;
}