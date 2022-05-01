/*==============================================================================

	ゲームのプレイヤー関係の処理　[Game_Player.h]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 21
==============================================================================*/
#ifndef GAME_PLAYER_H_
#define GAME_PLAYER_H_

#include "Collision.h"
#include "Game_Screw.h"

#define PLAYER_WIDTH				(SCREW_SIZE)			// 地上もしくは回転中のプレイヤーの横幅
#define PLAYER_HEIGHT				(PLAYER_WIDTH * 2)		// 地上もしくは回転中のプレイヤーの高さ
#define PLAYER_SIZE					(PLAYER_WIDTH)			// ジャンプ中のプレイヤーのサイズ
#define DRIVER_SIZE					(SCREW_SIZE / 4 * 3)	// ドライバーのサイズ

#define PLAYER_WALK_SPEED_MAX		(5.0f)					// 地上での最高速度
#define PLAYER_WALK_ACCELERATION	(0.5f)					// 地上での加速度
#define PLAYER_MOVE_ACCELERATION	(0.2f)					// 空中での加速度
#define PLAYER_MOVE_SPEED_MAX		(5.0f)					// 空中での最高速度
#define PLAYER_JUMP_SPEED			(-10.0f)				// ジャンプの初速
#define PLAYER_TURN_SPEED_MAX		(D3DXToRadian(12.0f))	// 回転の最高速度(毎秒12°)
#define PLAYER_TURN_JUMP_SPEED_X	(40.0f)					// 回転ジャンプの初速のx方向の係数
#define PLAYER_TURN_JUMP_SPEED_Y	(30.0f)					// 回転ジャンプの初速のy方向の係数
#define CAMERA_WIDTH				(1000)
#define CAMERA_HEIGHT				(500)

typedef enum PlayerState_tag {
	STATE_WALK,
	STATE_JUMP,
	STATE_TURN,
}PlayerState;

typedef enum PlayerDirection_tag {
	RIGHT,
	LEFT,
}PlayerDirection;

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// プレイヤー構造体
typedef struct Player_tag
{
	D3DXVECTOR2 position;		// 座標
	D3DXVECTOR2 position_old;	// 直前の座標
	D3DXVECTOR2 velocity;		// 速度
	float angle;				// 角度
	float ang_velocity;			// 角速度
	int turn_jump_frame;		// 回転ジャンプした瞬間のフレーム
	PlayerState state;			// 状態
	PlayerDirection direction;	// 向き（左右）
	CircleCollision collision;	// コリジョン
}Player;

void GamePlayer_Initialize(void);
void GamePlayer_Finalize(void);
void GamePlayer_Update(void);
void GamePlayer_Draw(void);

void GamePlayer_Walk(void);
void GamePlayer_Move(void);
void GamePlayer_Turn(void);

Player* GamePlayer_GetPlayer(void);

CircleCollision GamePlayer_GetPlayerCollision(void);
CircleCollision GamePlayer_GetDriverCollision(void);

void GamePlayer_CatchScrew(void);
void GamePlayer_ReleaseScrew(D3DXVECTOR2 jump_direction);

void GamePlayer_ToWalk(void);

void GamePlayer_Reset(void);

#endif // GAME_PLAYER_H_