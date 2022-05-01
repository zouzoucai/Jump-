/*==============================================================================

	ゲームの制限時間管理　[Game_Timer.cpp]
													Author : Ryuichi Tsunoda
													Date   : 2021 / 01 / 28
==============================================================================*/
#include <stdio.h>
#include <d3dx9.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Game.h"
#include "Game_Timer.h"
#include "StageSelect.h"
#include "my_font.h"

/*------------------------------------------------------------------------------
	構造体宣言
------------------------------------------------------------------------------*/
// 各ステージ基準時間管理構造体
typedef struct TimerRank_tag {
	float time_max;						// 残り時間
	float time3_2;						// ★★★（一番早い、ベストクリア）と★★☆の境界
	float time2_1;						// ★★☆と★☆☆の境界
	// ☆☆☆（一番遅い）と★☆☆の境界はどのステージも必ず0.0f
}TimerRank;

#define TEXT_WIDTH		(120.0f)
#define TEXT_HEIGHT		(30.0f)
#define TEXT_POS_X		(SCREEN_WIDTH - TEXT_WIDTH - 50.0f)
#define TEXT_POS_Y		(TEXT_HEIGHT / 2.0f)

#define TIME_WIDTH		(20.0f)
#define TIME_HEIGHT		(30.0f)
#define TIME_POS_Y		(TIME_HEIGHT / 2.0f)
#define FADE_TIME		(0.5f)		// 最初のフェードの時間
#define RESET_TIME		(3.0f)		// 時間切れ表示の時間

/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureIdText = TEXTURE_INVALID_ID;		// 「残り時間」の文字

// 各ステージの時間
static TimerRank g_TimeData[] = {
	{ 60.0f,  30.0f,  15.0f},	// チュートリアルステージ
	{ 60.0f,  30.0f,  15.0f},	// ステージ1
	{ 90.0f,  50.0f,  30.0f},	// ステージ2
	{100.0f,  50.0f,  25.0f},	// ステージ3
	{120.0f,  60.0f,  30.0f},	// ステージ4
	{180.0f, 100.0f,  60.0f},	// ステージ5
	{ 60.0f,  30.0f,  15.0f},	// ステージ6
	{ 60.0f,  30.0f,  15.0f},	// ステージ7
	{ 60.0f,  30.0f,  15.0f},	// ステージ8
	{ 60.0f,  30.0f,  15.0f},	// ステージ9
	{ 60.0f,  30.0f,  15.0f},	// ステージ10
	{ 60.0f,  30.0f,  15.0f},	// ステージ11
	{ 60.0f,  30.0f,  15.0f},	// ステージ12
	{ 60.0f,  30.0f,  15.0f},	// ステージ13
	{ 60.0f,  30.0f,  15.0f},	// ステージ14
	{ 60.0f,  30.0f,  15.0f},	// ステージ15
	{ 60.0f,  30.0f,  15.0f},	// ステージ16
	{ 60.0f,  30.0f,  15.0f},	// ステージ17
	{ 60.0f,  30.0f,  15.0f},	// ステージ18
	{ 60.0f,  30.0f,  15.0f},	// ステージ19
	{ 60.0f,  30.0f,  15.0f},	// ステージ20
};

// 各ステージのランクを格納する配列
static Rank g_Rank[STAGE_MAX] = {
	RANK_OPEN,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
	RANK_NONE,
};

// ランクをバイナリファイルから読み込む時、Rank型で直接読み込めないので一旦int型配列に格納するためのもの
static int g_SaveRank[STAGE_MAX];

// 今回プレイしているステージのランクを一時的に格納する変数
static Rank g_NowRank = RANK_OPEN;

static float g_TimeRemaining = 0.0f;
static int g_FrameCounter = 0;

static float g_TimePositionX = 0.0f;
static int g_TimeDigits = 0;

char g_Buf[4];

static PHASE g_Phase;

static int g_NowStage = 0;	// 現在のステージ


void GameTimer_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureIdText = Texture_SetTextureLoadFile("asset/texture/timer_text.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	// ステージ番号をセット
	g_NowStage = StageSelect_GetStageNumber();

	// 時間をセット、PHASEの切り替えはFrameCounterで行う
	g_TimeRemaining = g_TimeData[g_NowStage].time_max;
	g_FrameCounter = 0;

	sprintf(g_Buf, "%d", (int)g_TimeRemaining);

	g_TimeDigits = (int)strlen(g_Buf);

	g_TimePositionX = SCREEN_WIDTH - (TIME_WIDTH * g_TimeDigits + 100.0f) / 2.0f - 10.0f;

	g_Phase = PHASE_READY;
}

void GameTimer_Finalize(void)
{
	Texture_Release(&g_TextureIdText, 1);
}

void GameTimer_Update(void)
{
	g_FrameCounter++;

	switch (g_Phase)
	{
	case PHASE_READY:
		if (g_FrameCounter > START_FRAME) {
			g_Phase = PHASE_GAME;
		}
		break;
	case PHASE_GAME:
		g_TimeRemaining -= 1.0f / 60.0f;

		if (g_TimeRemaining < 0.0f) {
			g_TimeRemaining = 0.0f;
			sprintf(g_Buf, "%d", 0);
			g_Phase = PHASE_TIMEOVER;
			break;
		}

		// 最後の+1は小数点以下切り上げの処理
		sprintf(g_Buf, "%d", (int)(g_TimeRemaining) + 1);

		// 桁数をg_Bufの長さで判定
		g_TimeDigits = (int)strlen(g_Buf);

		g_TimePositionX = SCREEN_WIDTH - (TIME_WIDTH * g_TimeDigits + 100.0f) / 2.0f - 10.0f;

		break;
	case PHASE_RESET:
		if (g_FrameCounter > RESET_FRAME) {
			g_Phase = PHASE_GAME;
		}
		break;
	case PHASE_TIMEOVER:
		break;
	case PHASE_CLEAR:
		break;
	}
}

void GameTimer_Draw(void)
{
	switch (g_Phase)
	{
	case PHASE_GAME:
		Sprite_SetColor(D3DCOLOR_RGBA(0, 0, 0, 255));
		Sprite_Draw(g_TextureIdText, TEXT_POS_X, TEXT_POS_Y, TEXT_WIDTH, TEXT_HEIGHT, 110, 0, 220, 55);
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
		MyFont_Draw(g_TimePositionX, TIME_POS_Y, TIME_WIDTH, TIME_HEIGHT, g_Buf, D3DCOLOR_RGBA(0, 0, 0, 255));
		break;
	case PHASE_TIMEOVER:
		Sprite_SetColor(D3DCOLOR_RGBA(0, 0, 0, 255));
		Sprite_Draw(g_TextureIdText, TEXT_POS_X, TEXT_POS_Y, TEXT_WIDTH, TEXT_HEIGHT, 110, 0, 220, 55);
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
		MyFont_Draw(g_TimePositionX, TIME_POS_Y, TIME_WIDTH, TIME_HEIGHT, g_Buf, D3DCOLOR_RGBA(0, 0, 0, 255));
		break;
	case PHASE_CLEAR:
		Sprite_SetColor(D3DCOLOR_RGBA(0, 0, 0, 255));
		Sprite_Draw(g_TextureIdText, TEXT_POS_X, TEXT_POS_Y, TEXT_WIDTH, TEXT_HEIGHT, 110, 0, 220, 55);
		Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
		MyFont_Draw(g_TimePositionX, TIME_POS_Y, TIME_WIDTH, TIME_HEIGHT, g_Buf, D3DCOLOR_RGBA(0, 0, 0, 255));
		break;
	default:
		break;
	}
}

void GameTimer_Reset(void)
{
	// 時間をセット、PHASEの切り替えはFrameCounterで行う
	g_TimeRemaining = g_TimeData[g_NowStage].time_max;
	g_FrameCounter = 0;

	sprintf(g_Buf, "%d", (int)g_TimeRemaining);

	g_TimeDigits = (int)strlen(g_Buf);

	g_TimePositionX = SCREEN_WIDTH - (TIME_WIDTH * g_TimeDigits + 100.0f) / 2.0f - 10.0f;

	g_Phase = PHASE_RESET;
}

void GameTimer_Clear(void)
{
	g_Phase = PHASE_CLEAR;

	// 今回のランクを判定
	if (g_TimeRemaining > g_TimeData[g_NowStage].time3_2) {
		g_NowRank = RANK_3;
	}
	else if (g_TimeRemaining > g_TimeData[g_NowStage].time2_1) {
		g_NowRank = RANK_2;
	}
	else if (g_TimeRemaining > 0.0f) {
		g_NowRank = RANK_1;
	}
	else {
		g_NowRank = RANK_0;
	}

	// 今回のランクが記録されているランクよりも高ければg_Rankを更新
	if (g_NowRank > g_Rank[g_NowStage]) {
		g_Rank[g_NowStage] = g_NowRank;
	}

	// 2ステージでワンセット、クリアしたら次のセットを解放する処理
	if (g_NowStage % 2 == 1) {
		if (g_Rank[g_NowStage + 2] == RANK_NONE) {
			g_Rank[g_NowStage + 2] = RANK_OPEN;
		}
		if (g_Rank[g_NowStage + 3] == RANK_NONE) {
			g_Rank[g_NowStage + 3] = RANK_OPEN;
		}
	}
	else {
		if (g_Rank[g_NowStage + 1] == RANK_NONE) {
			g_Rank[g_NowStage + 1] = RANK_OPEN;
		}
		if (g_Rank[g_NowStage + 2] == RANK_NONE) {
			g_Rank[g_NowStage + 2] = RANK_OPEN;
		}
	}

}

Rank* GameTimer_GetRank(void)
{
	return g_Rank;
}

Rank GameTimer_GetNowRank(void)
{
	return g_NowRank;
}

void GameTimer_LoadRank(void)
{
	FILE* fp = fopen("rank.dat", "rb");

	// ファイルが存在しないとき、全くの未プレイ状態として初期化
	if (fp == NULL) {
		g_Rank[0] = RANK_OPEN;
		for (int i = 1; i < STAGE_MAX; i++) {
			g_Rank[i] = RANK_NONE;
		}
		return;
	}
	
	fread(g_SaveRank, sizeof(int), STAGE_MAX, fp);

	fclose(fp);

	for (int i = 0; i < STAGE_MAX; i++) {
		switch (g_SaveRank[i])
		{
		case 0:
			g_Rank[i] = RANK_NONE;
			break;
		case 1:
			g_Rank[i] = RANK_OPEN;
			break;
		case 2:
			g_Rank[i] = RANK_0;
			break;
		case 3:
			g_Rank[i] = RANK_1;
			break;
		case 4:
			g_Rank[i] = RANK_2;
			break;
		case 5:
			g_Rank[i] = RANK_3;
			break;
		}
	}
}

void GameTimer_SaveRank(void)
{
	for (int i = 0; i < STAGE_MAX; i++) {
		switch (g_Rank[i])
		{
		case RANK_NONE:
			g_SaveRank[i] = 0;
			break;
		case RANK_OPEN:
			g_SaveRank[i] = 1;
			break;
		case RANK_0:
			g_SaveRank[i] = 2;
			break;
		case RANK_1:
			g_SaveRank[i] = 3;
			break;
		case RANK_2:
			g_SaveRank[i] = 4;
			break;
		case RANK_3:
			g_SaveRank[i] = 5;
			break;
		}
	}

	FILE* fp = fopen("rank.dat", "wb");

	if (fp == NULL) {
		return;
	}

	fwrite(g_SaveRank, sizeof(int), STAGE_MAX, fp);

	fclose(fp);
}