/*==============================================================================

	ゲームのネジ関係の処理　[Game_Screw.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_SCREW_H_
#define GAME_SCREW_H_

#include "Config.h"
#include "Collision.h"

#define SCREW_SIZE				(30.0f)
#define START_X					(-20.0f)

#define TIGHTNESS_MAX			(D3DXToRadian(360.0f * 10))
#define TIGHTNESS_DECREASE		(D3DXToRadian(10.0f))	// ぜんまいネジが自動で回る時の速度

#define SCREW_MAX_X				(SCREEN_WIDTH / (int)SCREW_SIZE)
#define SCREW_MAX_Y				(SCREEN_HEIGHT / (int)SCREW_SIZE)
#define SCREW_MAX				(SCREW_MAX_X * SCREW_MAX_Y)

#define TIGHTNESS_GOOD_RANGE	(D3DXToRadian(360.0f * 3))

// ゲージの外側
#define	GAUGE_OUTSIDE_WIDTH		(SCREW_SIZE * 2.0f)
#define	GAUGE_OUTSIDE_HEIGHT	(GAUGE_OUTSIDE_WIDTH / 4.0f)
// 各Screwの座標を基準とした場合のゲージの外側の相対位置
#define	GAUGE_OUTSIDE_POS_X		(SCREW_SIZE / 2 - GAUGE_OUTSIDE_WIDTH / 2)
#define	GAUGE_OUTSIDE_POS_Y		(-GAUGE_OUTSIDE_HEIGHT * 1.5f)

// ゲージの内側
#define GAUGE_INSIDE_WIDTH_DEFAULT	(GAUGE_OUTSIDE_WIDTH / 40 * 39)
#define GAUGE_INSIDE_HEIGHT			(GAUGE_OUTSIDE_HEIGHT * 9 / 10)
// 各Screwの座標を基準とした場合のゲージの内側の相対位置
#define	GAUGE_INSUDE_POS_X_LEFT		(GAUGE_OUTSIDE_POS_X + (GAUGE_OUTSIDE_WIDTH - GAUGE_INSIDE_WIDTH_DEFAULT) / 2)
#define	GAUGE_INSUDE_POS_X_RIGHT	(GAUGE_OUTSIDE_POS_X)
#define	GAUGE_INSUDE_POS_Y			(GAUGE_OUTSIDE_POS_Y + (GAUGE_OUTSIDE_HEIGHT / 10) / 2)

// ゲージのイイ感じポイント
#define GAUGE_POINT_WIDTH			(GAUGE_OUTSIDE_WIDTH / 10)
#define GAUGE_POINT_HEIGHT			(GAUGE_OUTSIDE_HEIGHT * 1.5f)
// 各Screwの座標を基準とした場合のゲージのイイ感じポイントの相対位置
#define GAUGE_POINT_POS_X			(SCREW_SIZE / 2 - GAUGE_POINT_WIDTH / 2)
#define GAUGE_POINT_POS_Y			(GAUGE_OUTSIDE_POS_Y - GAUGE_POINT_HEIGHT / 3)

#define GAUGE_POINT_RANGE			(SCREW_SIZE / 8 * 3)
#define	GAUGE_INSUDE_TOTAL			(GAUGE_INSIDE_WIDTH_DEFAULT - GAUGE_POINT_RANGE)

// スイッチネジのランプ
#define LAMP_SIZE					(SCREW_SIZE / 2)
#define LAMP_POS_X					(SCREW_SIZE / 4)
#define LAMP_POS_Y					(SCREW_SIZE / 4 * 3)

// ネジの状態
typedef enum ScrewState_tag {
	STATE_NONE,			// 使われてない状態
	STATE_BAD,			// イイ感じでない状態
	STATE_GOOD,			// イイ感じの状態
	STATE_SPRING_ON,	// ぜんまい動いてる状態
	STATE_SPRING_WORK,	// ぜんまい移行状態（プレイヤーくっついてる状態）
	STATE_SPRING_OFF,	// ぜんまい動いてない状態
	STATE_SWITCH_ON,	// スイッチオン
	STATE_SWITCH_OFF,	// スイッチオフ
}ScrewState;

// ギミックの色（同じ色のギミックが連動）
typedef enum GimmickColor_tag {
	GIMMICK_NONE,	// ギミック無し
	GIMMICK_RED,
	GIMMICK_BLUE,
	GIMMICK_GREEN,
	GIMMICK_YELLOW,
}GimmickColor;

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// ネジ構造体
typedef struct Screw_tag
{
	D3DXVECTOR2 position;		// 座標
	ScrewState state;			// 状態
	float tightness;			// 締まり具合（0を基準に小さくなると締まり、大きくなると緩む）
	float gauge_left_width;		// 左側のゲージの横幅（右側のゲージの横幅は引き算で算出）
	CircleCollision collision;	// コリジョン
	GimmickColor gimmick_color;	// ギミックカラー
	int texture_id;				// テクスチャID（ギミックカラーによってテクスチャが異なるため）
}Screw;

void GameScrew_Initialize(void);
void GameScrew_Finalize(void);
void GameScrew_Update(void);
void GameScrew_Draw(void);

CircleCollision GameScrew_GetCollision(int i);

bool GameScrew_isEnable(int i);

Screw* GameScrew_GetScrew(int i);

int GameScrew_GetMax(void);

size_t GameScrew_GetCount(void);

int GameScrew_GetRemain(void);

float GameScrew_GetSize(void);

bool GameScrew_Turned(int index, float angle);

void GameScrew_Translation(void);

void GameScrew_AutoTurned(int index, float angle);
void GameScrew_AutoTurnedR(int index, float angle);
void GameScrew_AutoTurnedG(int index, float angle);
void GameScrew_AutoTurnedB(int index, float angle);
void GameScrew_AutoTurnedY(int index, float angle);

// 同じギミックカラーのぜんまいねじが一つでも作動していたらスイッチねじはオン、そうでなければオフ
void GameScrew_Switch(int index);

// ぜんまいねじの状態遷移関数
void GameScrew_ToSpringOn(int index);
void GameScrew_ToSpringWorkFromOn(int index);
void GameScrew_ToSpringWorkFromOff(int index);
void GameScrew_ToSpringOff(int index);
void GameScrew_ToAllSpringOff(int index);

//void GameScrew_SpringSwitch(int index);

void GameScrew_Reset(void);

#endif // GAME_SCREW_H_