/*==============================================================================

	ゲームのプレイヤー関係の処理　[Game_Player.cpp]

													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 27
==============================================================================*/
#include <d3dx9.h>
#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Keylogger.h"
#include "Scene.h"
#include "Fade.h"
#include "Game_Player.h"
#include "Game_Judgment.h"
#include "Game_Scaffold.h"
#include "Controller.h"
#include "StageSelect.h"
#include "sound.h"

#define DIRECTION_WIDTH		(40.0f)
#define DIRECTION_HEIGHT	(20.0f)

#define GRAVITY_WEAK_FRAME	(30)	// 回転ジャンプ直後の勢いを上げるため重力を弱めるフレーム数

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// プレイヤー初期位置管理構造体
typedef struct PlayerSpawn_tag {
	int row;						// 行
	int column;						// 列
	// 状態入れてもいいかもね
}PlayerSpawn;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureIdPlayer = TEXTURE_INVALID_ID;		// 歩行中
static int g_TextureIdPlayer2 = TEXTURE_INVALID_ID;		// ジャンプ中
static int g_TextureIdPlayer3 = TEXTURE_INVALID_ID;		// 回転中
static int g_TextureIdDriver = TEXTURE_INVALID_ID;
static int g_TextureIdDirection = TEXTURE_INVALID_ID;

static Player g_Player;

static int g_NowStage = 0;	// 現在のステージ

// 各ステージ毎のプレイヤーの初期位置（行と列で指定）
static PlayerSpawn g_InitialPosition[] = {
	{22, 2},	// チュートリアルステージ
	{12, 5},	// ステージ1
	{7, 39},	// ステージ2
	{20, 1},	// ステージ3
	{22, 3},	// ステージ4
	{22, 7},	// ステージ5
	{22, 3},	// ステージ6
	{6, 5},	// ステージ7
	{18,31},	// ステージ8
	{1, 16},	// ステージ9
	{14,34},	// ステージ10
	{18,36},	// ステージ11
	{19,22},	// ステージ12
	{17, 8},	// ステージ13
	{ 9, 3},	// ステージ14
	{ 6, 6},	// ステージ15
	{22, 3},	// ステージ16
	{16, 23},	// ステージ17
	{3, 3},	// ステージ18
	{22, 21},	// ステージ19
	{10, 12},	// ステージ20
};

static D3DXVECTOR2 g_DriverPosition = D3DXVECTOR2(0.0f, 0.0f);
static CircleCollision g_DriverCollision;

static D3DXVECTOR2 g_DirectionPosition = D3DXVECTOR2(0.0f, 0.0f);
static float g_DirectionAngle = 0.0f;

static float g_gravity = 0.4f;

static int g_ScrewIndex = -1;	// くっついているネジのインデックス、ネジにくっついていない場合は-1

static float g_WalkFrame = 0;	// 歩行中のアニメーション用変数

static float g_rad = 0.0f;		// ジャンプ中の回転モーション用変数

static int g_FrameCounter = 0;

void GamePlayer_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureIdPlayer = Texture_SetTextureLoadFile("asset/texture/キャラアニメー歩く3_3(縮小改01).png");
	g_TextureIdPlayer2 = Texture_SetTextureLoadFile("asset/texture/キャラ飛ぶ01.png");
	g_TextureIdPlayer3 = Texture_SetTextureLoadFile("asset/texture/キャラハング改1（ドライバなし）.png");
	g_TextureIdDriver = Texture_SetTextureLoadFile("asset/texture/cude.png");
	g_TextureIdDirection = Texture_SetTextureLoadFile("asset/texture/player_direction.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	// ステージ番号をセット
	g_NowStage = StageSelect_GetStageNumber();

	GamePlayer_Reset();
}

void GamePlayer_Finalize(void)
{
	Texture_Release(&g_TextureIdPlayer, 1);
	Texture_Release(&g_TextureIdPlayer2, 1);
	Texture_Release(&g_TextureIdPlayer3, 1);
	Texture_Release(&g_TextureIdDriver, 1);
	Texture_Release(&g_TextureIdDirection, 1);
}

void GamePlayer_Update(void)
{
	g_Player.position_old = g_Player.position;

	switch (g_Player.state)
	{
	case STATE_WALK:
		// ジャンプ
		if (Controller_Decision() && g_Player.velocity.y == 0.0f) {
			g_Player.velocity.y = PLAYER_JUMP_SPEED;
			g_Player.state = STATE_JUMP;
			PlaySound(SOUND_LABEL_SE_JUMP);
			break;
		}

		// 地上での移動（歩行）
		GamePlayer_Walk();

		// ドライバー突き刺し
		if (GameJudgment_ScrewToDriver() >= 0) {
			PlaySound(SOUND_LABEL_SE_CATCH);
			GamePlayer_CatchScrew();
			break;
		}

		// アニメーション用コマ更新処理
		if (fabsf(g_Player.velocity.x) > 0.1f) {
			g_WalkFrame += 1.0f / 2.0f * fabsf(g_Player.velocity.x) / PLAYER_WALK_SPEED_MAX;
		}

		break;
	case STATE_JUMP:
		// ドライバー突き刺し
		if (GameJudgment_ScrewToDriver() >= 0 && (g_FrameCounter - g_Player.turn_jump_frame > 15)) {
			PlaySound(SOUND_LABEL_SE_CATCH);
			GamePlayer_CatchScrew();
			break;
		}

		g_rad += 0.3f;

		// 空中での移動
		GamePlayer_Move();

		break;
	case STATE_TURN:
		// コントローラーで入力した方向をベクトルで取得
		D3DXVECTOR2 jump_direction =
		{ (Controller_GetLeftStickX() / 65535.0f) - 0.5f, (Controller_GetLeftStickY() / 65535.0f) - 0.5f };

		switch(Controller_GetDirectionButton())
		{
		case BUTTON_UP:
			jump_direction = { 0.0f, -0.5f };
			break;
		case BUTTON_UPRIGHT:
			jump_direction = { 0.25f, -0.25f };
			break;
		case BUTTON_RIGHT:
			jump_direction = { 0.5f,  0.0f };
			break;
		case BUTTON_DOWNRIGHT:
			jump_direction = { 0.25f, 0.25f };
			break;
		case BUTTON_DOWN:
			jump_direction = { 0.0f,  0.5f };
			break;
		case BUTTON_DOWNLEFT:
			jump_direction = {-0.25f,  0.25f };
			break;
		case BUTTON_LEFT:
			jump_direction = {-0.5f,   0.0f };
			break;
		case BUTTON_UPLEFT:
			jump_direction = {-0.25f, -0.25f };
			break;
		default:
			break;
		}

		// 回転ジャンプ、ネジから離れる
		if (Controller_Decision()) {
			GamePlayer_ReleaseScrew(jump_direction);
			break;
		}

		// くっつき中の回転
		GamePlayer_Turn();

		// 矢印の方向を決める
		if ((fabsf(jump_direction.x) >= 0.1f || fabsf(jump_direction.y) >= 0.1f) && Controller_isGamePad()) {
			g_DirectionAngle = atan2f(jump_direction.y, jump_direction.x) - D3DXToRadian(90);
		}
		else {
			g_DirectionAngle = g_Player.angle;
		}

		// くっついてるScrewの音のみ鳴らす
		Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
		if (((int)pScrew->tightness) % 3 == 0) {
			PlaySound(SOUND_LABEL_SE_SCREW_TURN_02);
		}

		break;
	}

	// 画面外に出ないようにする処理
	if (g_Player.position.x < 0.0f) {
		g_Player.position.x = 0.0f;
	}
	else if (g_Player.position.x > SCREEN_WIDTH - PLAYER_WIDTH) {
		g_Player.position.x = SCREEN_WIDTH - PLAYER_WIDTH;
	}

	g_FrameCounter++;
}

void GamePlayer_Draw(void)
{
	switch (g_Player.state)
	{
	case STATE_WALK:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT,
				145 * ((int)g_WalkFrame % 4), 0, 145, 280);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT,
				(145 * ((int)g_WalkFrame % 4)) + 145, 0, -145, 280);
		}
		break;
	case STATE_JUMP:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer2, g_Player.position.x, g_Player.position.y, PLAYER_SIZE, PLAYER_SIZE, 0, 0, 1600, 1600,
				PLAYER_SIZE / 2, PLAYER_SIZE / 2, g_rad);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer2, g_Player.position.x, g_Player.position.y, PLAYER_SIZE, PLAYER_SIZE, 1600, 0, -1600, 1600,
				PLAYER_SIZE / 2, PLAYER_SIZE / 2, -g_rad);
		}
		break;
	case STATE_TURN:
		if (g_Player.direction == RIGHT) {
			Sprite_Draw(g_TextureIdPlayer3, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, 1400, 2800,
				PLAYER_WIDTH / 2, -DRIVER_SIZE / 2, g_Player.angle);
		}
		else {
			Sprite_Draw(g_TextureIdPlayer3, g_Player.position.x, g_Player.position.y, PLAYER_WIDTH, PLAYER_HEIGHT, 1400, 0, -1400, 2800,
				PLAYER_WIDTH / 2, -DRIVER_SIZE / 2, g_Player.angle);
		}

		Sprite_Draw(g_TextureIdDriver, g_DriverPosition.x, g_DriverPosition.y, DRIVER_SIZE, DRIVER_SIZE, 0, 0, 505, 478,
			DRIVER_SIZE / 2, DRIVER_SIZE / 2, g_Player.angle);
		
		// 矢印表示
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 0, 255));
		Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
			DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT), g_DirectionAngle);

		// 角速度で表示するDirectionの数、色を変える
		if (fabsf(g_Player.ang_velocity) > PLAYER_TURN_SPEED_MAX * 0.95f) {
			Sprite_SetColor(D3DCOLOR_RGBA(255, 110, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT), g_DirectionAngle);
			Sprite_SetColor(D3DCOLOR_RGBA(255, 0, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT * 2, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT * 2), g_DirectionAngle);
		}
		else if (fabsf(g_Player.ang_velocity) > PLAYER_TURN_SPEED_MAX * 0.75f) {
			Sprite_SetColor(D3DCOLOR_RGBA(255, 110, 0, 255));
			Sprite_Draw(g_TextureIdDirection, g_DirectionPosition.x, g_DirectionPosition.y + DIRECTION_HEIGHT, DIRECTION_WIDTH, DIRECTION_HEIGHT, 0, 0, 80, 59,
				DIRECTION_WIDTH / 2, -(DRIVER_SIZE / 2 + PLAYER_HEIGHT + DIRECTION_HEIGHT), g_DirectionAngle);
		}
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
		break;
	}
}

// 地上でのプレイヤーの移動
void GamePlayer_Walk(void)
{
	if (Controller_MoveLeft()) {
		//g_Player.velocity.x = -PLAYER_WALK_SPEED;
		// 左方向に加速、最高速度に達したら加速は止まる
		g_Player.velocity.x = g_Player.velocity.x > -PLAYER_WALK_SPEED_MAX ?
			g_Player.velocity.x - PLAYER_WALK_ACCELERATION : -PLAYER_WALK_SPEED_MAX;
		g_Player.direction = LEFT;
	}

	if (Controller_MoveRight()) {
		//g_Player.velocity.x = PLAYER_WALK_SPEED;
		// 右方向に加速、最高速度に達したら加速は止まる
		g_Player.velocity.x = g_Player.velocity.x < PLAYER_WALK_SPEED_MAX ?
			g_Player.velocity.x + PLAYER_WALK_ACCELERATION : PLAYER_WALK_SPEED_MAX;
		g_Player.direction = RIGHT;
	}

	g_Player.velocity.x *= 0.9f;

	// 重力で下に落下する処理
	g_Player.velocity.y += g_gravity;

	// 速度の分だけ移動
	g_Player.position += g_Player.velocity;

	// ドライバーの座標はプレイヤーに追従する
	if (g_Player.direction == RIGHT) {
		g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	}
	else {
		g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	}

	// 足場との当たり判定
	for (int i = 0; i < GameScaffold_GetCount(); i++) {
		Scaffold* pScaffold = GameScaffold_GetScaffold(i);
		switch (pScaffold->type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BLOCK_ON:
			// 衝突していなければbreak
			if (g_Player.position.y + PLAYER_HEIGHT <= pScaffold->position.y
				|| g_Player.position.y >= pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_WIDTH <= pScaffold->position.x
				|| g_Player.position.x >= pScaffold->position.x + pScaffold->width) {
				break;
			}

			// 上左右どの方向からの衝突なのか判定
			// 各判定式の最後の数字はposition_oldがブロックにめり込んだ時でも判定取れるようにするためのもの
			// 上からの場合
			if (g_Player.position_old.y + PLAYER_HEIGHT <= pScaffold->position.y + 5.0f) {
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
				g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x, 0.0f);
			}
			// 左からの場合
			else if (g_Player.position_old.x + PLAYER_WIDTH <= pScaffold->position.x + 15.0f) {
				g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH;
			}
			// 右からの場合
			else if (g_Player.position_old.x >= pScaffold->position.x + pScaffold->width - 15.0f) {
				g_Player.position.x = pScaffold->position.x + pScaffold->width;
			}

			break;
		case TYPE_FLOOR:
			// 衝突していなければbreak
			if (g_Player.position.y + PLAYER_HEIGHT < pScaffold->position.y
				|| g_Player.position.y > pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_WIDTH < pScaffold->position.x
				|| g_Player.position.x > pScaffold->position.x + pScaffold->width) {
				break;
			}

			// 上からの場合(床は上からの判定のみ)
			if (g_Player.position_old.y + PLAYER_HEIGHT <= pScaffold->position.y) {
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
				g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x, 0.0f);
			}
			break;
		}
	}

	if (g_Player.position.y > SCREEN_HEIGHT - PLAYER_HEIGHT) {
		g_Player.position.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
		g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
		g_Player.velocity.y = 0.0f;
	}
}

// 空中でのプレイヤーの移動
void GamePlayer_Move(void)
{
	if (Controller_MoveLeft() && (g_FrameCounter - g_Player.turn_jump_frame > 10)) {
		// 左方向に加速、最高速度に達したら加速は止まる
		g_Player.velocity.x = g_Player.velocity.x > -PLAYER_MOVE_SPEED_MAX ?
			g_Player.velocity.x - PLAYER_MOVE_ACCELERATION : -PLAYER_MOVE_SPEED_MAX;
	}

	if (Controller_MoveRight() && (g_FrameCounter - g_Player.turn_jump_frame > 10)) {
		// 右方向に加速、最高速度に達したら加速は止まる
		g_Player.velocity.x = g_Player.velocity.x < PLAYER_MOVE_SPEED_MAX ?
			g_Player.velocity.x + PLAYER_MOVE_ACCELERATION : PLAYER_MOVE_SPEED_MAX;
	}

	g_Player.velocity.x *= 0.98f;

	// 重力で下に落下する処理
	if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME) {
		g_Player.velocity.y += g_gravity;
	}
	else if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME / 3 * 2) {
		g_Player.velocity.y += g_gravity * 0.75f;
	}
	else if (g_FrameCounter - g_Player.turn_jump_frame > GRAVITY_WEAK_FRAME / 3) {
		g_Player.velocity.y += g_gravity * 0.5f;
	}

	// 速度の分だけ移動
	g_Player.position += g_Player.velocity;

	// ドライバーの座標はプレイヤーに追従する
	g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 3.5f;
	g_DriverPosition.y = g_Player.position.y + 5.0f;

	// 足場との当たり判定
	for (int i = 0; i < GameScaffold_GetCount(); i++) {
		Scaffold* pScaffold = GameScaffold_GetScaffold(i);
		switch (pScaffold->type)
		{
		case TYPE_NONE:
			break;
		case TYPE_BLOCK_ON:
			// 衝突していなければbreak
			if (g_Player.position.y + PLAYER_SIZE <= pScaffold->position.y
				|| g_Player.position.y >= pScaffold->position.y + pScaffold->height
				|| g_Player.position.x + PLAYER_SIZE <= pScaffold->position.x
				|| g_Player.position.x >= pScaffold->position.x + pScaffold->width) {
				break;
			}

			// 上下左右どの方向からの衝突なのか判定
			// 直前の座標がどこだったかで衝突の位置を判定
			// 上からの場合
			if (g_Player.position_old.y + PLAYER_SIZE <= pScaffold->position.y + 5.0f && g_Player.velocity.y > 0) {
				// 左にはみ出ないようにする処理
				if (g_Player.position.x + PLAYER_WIDTH / 2 < pScaffold->position.x) {
					g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH / 2;
				}
				// 右にはみ出ないようにする処理
				else if (g_Player.position.x > pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2) {
					g_Player.position.x = pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2;
				}
				g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				GamePlayer_ToWalk();
			}
			// 下からの場合
			else if (g_Player.position_old.y >= pScaffold->position.y + pScaffold->height - 5.0f) {
				g_Player.position.y = pScaffold->position.y + pScaffold->height;
			}
			// 左からの場合
			else if (g_Player.position_old.x + PLAYER_SIZE <= pScaffold->position.x + 5.0f) {
				g_Player.position.x = pScaffold->position.x - PLAYER_SIZE;
			}
			// 右からの場合
			else if (g_Player.position_old.x >= pScaffold->position.x + pScaffold->width - 5.0f) {
				g_Player.position.x = pScaffold->position.x + pScaffold->width;
			}
			
			break;
		case TYPE_FLOOR:
			// 衝突していなければbreak
			if (g_Player.position.y + PLAYER_SIZE < pScaffold->position.y
				|| g_Player.position.y > pScaffold->position.y + FLOOR_HEIGHT
				|| g_Player.position.x + PLAYER_SIZE < pScaffold->position.x
				|| g_Player.position.x > pScaffold->position.x + pScaffold->width) {
				break;
			}

			// 上からの場合(床は上からの判定のみ)
			if (g_Player.position_old.y + PLAYER_SIZE <= pScaffold->position.y && g_Player.velocity.y > 0) {
				// 左にはみ出ないようにする処理
				if (g_Player.position.x + PLAYER_WIDTH / 2 < pScaffold->position.x) {
					g_Player.position.x = pScaffold->position.x - PLAYER_WIDTH / 2;
				}
				// 右にはみ出ないようにする処理
				else if (g_Player.position.x > pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2) {
					g_Player.position.x = pScaffold->position.x + pScaffold->width - PLAYER_WIDTH / 2;
				}
				g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
				g_Player.position.y = pScaffold->position.y - PLAYER_HEIGHT;
				GamePlayer_ToWalk();
			}

			break;
		}
	}

	// 一定より下まで落ちたら着地
	if (g_Player.position.y < 0.0f) {
		g_Player.position.y = 0.0f;
	}
	else if (g_Player.position.y > SCREEN_HEIGHT - PLAYER_SIZE) {
		g_Player.position.y = SCREEN_HEIGHT - PLAYER_HEIGHT;
		GamePlayer_ToWalk();
	}
}

// ドライバーをネジに刺した時のプレイヤーの回転
void GamePlayer_Turn(void)
{
	// 回転速度、前半は一気に上がり、後半はゆっくり上がる
	if (Controller_TurnLeft()) {
		// 時計回り方向に加速、最高速度に達したら加速は止まる
		if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.0f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.1f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.25f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.05f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.01f;
		}
		else if (g_Player.ang_velocity < PLAYER_TURN_SPEED_MAX) {
			g_Player.ang_velocity += PLAYER_TURN_SPEED_MAX * 0.003f;
		}
		else {
			g_Player.ang_velocity = PLAYER_TURN_SPEED_MAX;
		}
	}

	if (Controller_TurnRight()) {
		// 反時計回り方向に加速、最高速度に達したら加速は止まる
		if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.0f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.1f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.25f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.05f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.01f;
		}
		else if (g_Player.ang_velocity > -PLAYER_TURN_SPEED_MAX) {
			g_Player.ang_velocity -= PLAYER_TURN_SPEED_MAX * 0.003f;
		}
		else {
			g_Player.ang_velocity = -PLAYER_TURN_SPEED_MAX;
		}
	}

	if (!Controller_TurnLeft() && !Controller_TurnRight()) {
		if (sinf(g_Player.angle) > 0) {
			g_Player.ang_velocity -= D3DXToRadian(0.2f);
		}
		else if (sinf(g_Player.angle) < 0) {
			g_Player.ang_velocity += D3DXToRadian(0.2f);
		}
		g_Player.ang_velocity *= 0.995f;
	}

	// GameScrew_TurnedはScrewがまだ回せる場合はtrue、回せない場合はfalseを返す関数であり、
	// if文の条件式に入れることでScrewのtightnessを変化させながら回せるかどうかの判定をしている
	if (GameScrew_Turned(g_ScrewIndex, g_Player.ang_velocity)) {
		g_Player.angle += g_Player.ang_velocity;
	}
	else {
		g_Player.ang_velocity = 0.0f;
	}

	// g_Player.angleが地面と垂直な角度から少ししか変わらない場合に、角度は0にする
	// これがないとしばらくの間プレイヤーが小刻みに震えるため
	g_Player.angle = cosf(g_Player.angle) < 0.9999f ? g_Player.angle : 0.0f;
}

Player* GamePlayer_GetPlayer(void)
{
	return &g_Player;
}

CircleCollision GamePlayer_GetPlayerCollision(void)
{
	return g_Player.collision;
}

CircleCollision GamePlayer_GetDriverCollision(void)
{
	CircleCollision DriverCollision;

	DriverCollision.center.x = g_DriverPosition.x + DRIVER_SIZE / 2;
	DriverCollision.center.y = g_DriverPosition.y + DRIVER_SIZE / 2;
	DriverCollision.radius = DRIVER_SIZE / 2 * 1.5f;	// 少し大きめに当たり判定取る

	return DriverCollision;
}

void GamePlayer_CatchScrew(void)
{
	// くっつくネジのインデックスを取得
	g_ScrewIndex = GameJudgment_ScrewToDriver();
	g_Player.state = STATE_TURN;
	g_DriverPosition = GameScrew_GetScrew(g_ScrewIndex)->position +
		D3DXVECTOR2((SCREW_SIZE - DRIVER_SIZE) / 2, (SCREW_SIZE - DRIVER_SIZE) / 2);
	if (g_Player.direction == RIGHT) {
		g_Player.position = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - PLAYER_WIDTH / 2, DRIVER_SIZE);
	}
	else {
		g_Player.position = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - PLAYER_WIDTH / 2, DRIVER_SIZE);
	}
	g_DirectionPosition = g_DriverPosition + D3DXVECTOR2(DRIVER_SIZE / 2 - DIRECTION_WIDTH / 2, DRIVER_SIZE + PLAYER_HEIGHT);
	// 突き刺した瞬間の角速度はプレイヤーのx方向の速度の-10倍に比例
	g_Player.ang_velocity = D3DXToRadian(g_Player.velocity.x / PLAYER_MOVE_SPEED_MAX * -10.0f);

	// ネジの状態に合わせてネジやギミックへの処理を変える
	Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
	switch (pScrew->state)
	{
	case STATE_SPRING_OFF:
		GameScrew_ToSpringWorkFromOff(g_ScrewIndex);
		GameScaffold_Switch(pScrew->gimmick_color);
		break;
	case STATE_SPRING_ON:
		GameScrew_ToSpringWorkFromOn(g_ScrewIndex);
		break;
	case STATE_SWITCH_ON:
		GameScrew_ToAllSpringOff(g_ScrewIndex);
		break;
	}
}

void GamePlayer_ReleaseScrew(D3DXVECTOR2 jump_direction)
{
	g_Player.state = STATE_JUMP;
	// スティックのx方向またはy方向のいずれかがしきい値以上に動いていたらスティックを倒している方向に飛ぶ
	if ((fabsf(jump_direction.x) >= 0.1f || fabsf(jump_direction.y) >= 0.1f) && Controller_isGamePad()) {
		D3DXVec2Normalize(&jump_direction, &jump_direction);
		if (fabsf(g_Player.ang_velocity) < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.velocity = D3DXVECTOR2(
				jump_direction.x * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_X,
				jump_direction.y * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_Y);
		}
		else {
			g_Player.velocity = D3DXVECTOR2(
				jump_direction.x * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_X,
				jump_direction.y * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_Y);
		}
	}
	else {
		if (fabsf(g_Player.ang_velocity) < PLAYER_TURN_SPEED_MAX * 0.5f) {
			g_Player.velocity = D3DXVECTOR2(
				cosf(D3DXToRadian(90.0f) + g_Player.angle) * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_X,
				sinf(D3DXToRadian(90.0f) + g_Player.angle) * PLAYER_TURN_SPEED_MAX * 0.5f * PLAYER_TURN_JUMP_SPEED_Y);
		}
		else {
			g_Player.velocity = D3DXVECTOR2(
				cosf(D3DXToRadian(90.0f) + g_Player.angle) * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_X,
				sinf(D3DXToRadian(90.0f) + g_Player.angle) * fabsf(g_Player.ang_velocity) * PLAYER_TURN_JUMP_SPEED_Y);
		}
	}

	g_Player.position = g_DriverPosition +
		D3DXVECTOR2(DRIVER_SIZE / 2 + cosf(D3DXToRadian(90.0f) + g_Player.angle) * DRIVER_SIZE / 2 - PLAYER_SIZE / 2,
			DRIVER_SIZE / 2 + sinf(D3DXToRadian(90.0f) + g_Player.angle) * DRIVER_SIZE / 2 - PLAYER_SIZE / 2);

	g_Player.angle = 0.0f;
	g_Player.ang_velocity = 0.0f;
	g_Player.turn_jump_frame = g_FrameCounter;
	g_DirectionAngle = 0.0f;

	// ネジの状態に合わせてネジやギミックへの処理を変える
	// 今はぜんまいネジを作動させるのみ
	Screw* pScrew = GameScrew_GetScrew(g_ScrewIndex);
	if (pScrew->state == STATE_SPRING_WORK) {
		GameScrew_ToSpringOn(g_ScrewIndex);
	}
	g_ScrewIndex = -1;
}

void GamePlayer_ToWalk(void)
{
	if (g_Player.velocity.x > 0.0f) {
		g_Player.direction = RIGHT;
	}
	else {
		g_Player.direction = LEFT;
	}

	g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;
	g_Player.velocity = D3DXVECTOR2(g_Player.velocity.x / 2, 0.0f);
	g_Player.state = STATE_WALK;
	g_WalkFrame = 0.0f;
}

void GamePlayer_Reset(void)
{
	g_Player.position = D3DXVECTOR2(START_X + g_InitialPosition[g_NowStage].column * SCREW_SIZE,
		g_InitialPosition[g_NowStage].row * SCREW_SIZE);
	g_Player.position_old = g_Player.position;
	g_Player.velocity = D3DXVECTOR2(0.0f, 0.0f);

	g_Player.state = STATE_WALK;
	g_Player.direction = RIGHT;

	g_Player.turn_jump_frame = -GRAVITY_WEAK_FRAME;	// 開始してGRAVITY_WEAK_FRAMEの間にジャンプすると高く飛び過ぎるのを防ぐため

	g_DriverPosition.x = g_Player.position.x + PLAYER_WIDTH / 2 - DRIVER_SIZE / 2;
	g_DriverPosition.y = g_Player.position.y - DRIVER_SIZE;

	g_DirectionPosition = D3DXVECTOR2(0.0f, 0.0f);
	g_DirectionAngle = 0.0f;

	g_gravity = 0.4f;

	g_ScrewIndex = -1;

	g_WalkFrame = 0.0f;

	g_FrameCounter = 0;
}