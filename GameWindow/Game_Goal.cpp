/*==============================================================================

	ゲームのゴール処理　[Game_Goal.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 25
==============================================================================*/
#include <d3dx9.h>
#include <stdio.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Game_Player.h"
#include "StageSelect.h"
#include "my_font.h"

#define GOAL_WIDTH	(SCREW_SIZE)
#define GOAL_HEIGHT	(SCREW_SIZE * 2.0f)

#define NUM_SIZE	(SCREW_SIZE)	// 残りネジ数の表示サイズ

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// ゴール位置管理構造体
typedef struct GoalSpawn_tag {
	int row;						// 行
	int column;						// 列
}GoalSpawn;

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureIdGoalBad = TEXTURE_INVALID_ID;			// ゴール(クリア不可能)
static int g_TextureIdGoalGood = TEXTURE_INVALID_ID;		// ゴール(クリア可能)
static int g_TextureIdGoalOpen = TEXTURE_INVALID_ID;		// ゴール(空いている)

static bool g_isOpen = false;	// ゴールが空いているかどうか

static bool g_isClear = false;	// クリアしたかどうか

static D3DXVECTOR2 g_GoalPosition(0.0f, 0.0f);

static int g_NowStage = 0;	// 現在のステージ

// 各ステージ毎のゴールの位置（行と列で指定）
static GoalSpawn g_Position[] = {
	{22, 29},	// チュートリアルステージ
	{11, 35},	// ステージ1
	{9, 2},	// ステージ2
	{8,  39},	// ステージ3
	{2, 36},	// ステージ4
	{7,  5},	// ステージ5
	{22, 38},	// ステージ6
	{12, 41},	// ステージ7
	{ 2,  4},	// ステージ8
    {22, 21},	// ステージ9
	{3,  4},	// ステージ10
	{22, 36},	// ステージ11
	{3,  22},	// ステージ12
	{12, 37},	// ステージ13
	{ 3, 33},	// ステージ14
	{18, 40},	// ステージ15
	{22, 40},	// ステージ16
	{16, 23},	// ステージ17
	{22, 22},	// ステージ18
	{22, 21},	// ステージ19
	{22, 39},	// ステージ20
};

void GameGoal_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureIdGoalBad = Texture_SetTextureLoadFile("asset/texture/goal_bad.png");
	g_TextureIdGoalGood = Texture_SetTextureLoadFile("asset/texture/goal_good.png");
	g_TextureIdGoalOpen = Texture_SetTextureLoadFile("asset/texture/goal_open.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	// ステージ番号をセット
	g_NowStage = StageSelect_GetStageNumber();

	g_GoalPosition = D3DXVECTOR2(START_X + g_Position[g_NowStage].column * SCREW_SIZE, g_Position[g_NowStage].row * SCREW_SIZE);

	g_isOpen = false;

	g_isClear = false;
}

void GameGoal_Finalize(void)
{
	Texture_Release(&g_TextureIdGoalBad, 1);
	Texture_Release(&g_TextureIdGoalGood, 1);
	Texture_Release(&g_TextureIdGoalOpen, 1);
}

bool GameGoal_Update(void)
{
	if (!g_isOpen) {
		return false;
	}

	Player* pPlayer = GamePlayer_GetPlayer();

	// プレイヤーが歩行状態でなければreturn false
	if (pPlayer->state != STATE_WALK) {
		return false;
	}

	// ゴールに入っていなければreturn false
	if (pPlayer->position.y < g_GoalPosition.y
		|| pPlayer->position.y > g_GoalPosition.y
		|| pPlayer->position.x + PLAYER_WIDTH < g_GoalPosition.x + GOAL_WIDTH / 1.5f
		|| pPlayer->position.x > g_GoalPosition.x + GOAL_WIDTH / 3) {
		return false;
	}
	else {
		g_isClear = true;
		return true;
	}

}

void GameGoal_Draw(void)
{
	D3DCOLOR num_color = D3DCOLOR_RGBA(255, 255, 255, 255);
	int remain = GameScrew_GetRemain();

	char buf[4];
	sprintf(buf, "%d", remain);
	if (remain >= 10) {
		MyFont_Draw(g_GoalPosition.x - NUM_SIZE / 2, g_GoalPosition.y - NUM_SIZE, NUM_SIZE, NUM_SIZE, buf, num_color);
	}
	else {
		MyFont_Draw(g_GoalPosition.x, g_GoalPosition.y - NUM_SIZE, NUM_SIZE, NUM_SIZE, buf, num_color);
	}

	if (!g_isOpen) {
		Sprite_Draw(g_TextureIdGoalBad, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
	else if(g_isOpen && !g_isClear){
		Sprite_Draw(g_TextureIdGoalGood, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
	else {
		Sprite_Draw(g_TextureIdGoalOpen, g_GoalPosition.x, g_GoalPosition.y, GOAL_WIDTH, GOAL_HEIGHT, 0, 0, 180, 360);
	}
}

void GameGoal_ToEnable(void)
{
	g_isOpen = true;
}

void GameGoal_ToDisenable(void)
{
	g_isOpen = false;
}
int GameGoal_GetNowStage(void)
{
	return g_NowStage;
}