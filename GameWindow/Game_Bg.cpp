/*==============================================================================

	ÉQÅ[ÉÄÇÃîwåièàóùÅ@[Game_Bg.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 27
==============================================================================*/
#include <d3dx9.h>
#include <stdio.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "StageSelect.h"
#include "Game_Player.h"

#define BG_MAX	(7)
#define BOARD_MAX	(6)

/*------------------------------------------------------------------------------
	ÉOÉçÅ[ÉoÉãïœêîêÈåæ
------------------------------------------------------------------------------*/
static int g_TextureId[BG_MAX];				// îwåi1Å`4

static int g_TextureAnimationId[3];			// îwåiÉAÉjÉÅÅ[ÉVÉáÉì

static int g_TextureIdBoard[BOARD_MAX];		// ä≈î¬1Å`6

static int g_TextureIdStageNum;	// ÉXÉeÅ[ÉWî‘çÜ

static int g_NowBgId = TEXTURE_INVALID_ID;

static int g_NowStage = 0;

static PlayerState g_PlayerState = STATE_WALK;

static float g_AnimationCount = 0.0f;

void GameBg_Initialize(void)
{
	// îOÇÃÇΩÇﬂèâä˙âª
	for (int i = 0; i < BG_MAX; i++) {
		g_TextureId[i] = TEXTURE_INVALID_ID;
	}

	for (int i = 0; i < 3; i++) {
		g_TextureAnimationId[i] = TEXTURE_INVALID_ID;
	}

	for (int i = 0; i < BOARD_MAX; i++) {
		g_TextureIdBoard[i] = TEXTURE_INVALID_ID;
	}

	g_TextureIdStageNum = TEXTURE_INVALID_ID;

	g_TextureId[0] = Texture_SetTextureLoadFile("asset/texture/îwåi1.jpg");
	g_TextureId[1] = Texture_SetTextureLoadFile("asset/texture/îwåi7.png");
	g_TextureId[2] = Texture_SetTextureLoadFile("asset/texture/îwåi3.jpg");
	g_TextureId[3] = Texture_SetTextureLoadFile("asset/texture/îwåi2.jpg");
	g_TextureId[4] = Texture_SetTextureLoadFile("asset/texture/îwåi5.png");
	g_TextureId[5] = Texture_SetTextureLoadFile("asset/texture/îwåi8.png");
	g_TextureId[6] = Texture_SetTextureLoadFile("asset/texture/îwåi10.png");

	g_TextureAnimationId[0] = Texture_SetTextureLoadFile("asset/texture/îwåiÉAÉjÉÅ1.png");
	g_TextureAnimationId[1] = Texture_SetTextureLoadFile("asset/texture/îwåiÉAÉjÉÅ2.png");
	g_TextureAnimationId[2] = Texture_SetTextureLoadFile("asset/texture/îwåiÉAÉjÉÅ3.png");

	g_NowStage = StageSelect_GetStageNumber();

	if (g_NowStage == 0) {
		g_TextureIdBoard[0] = Texture_SetTextureLoadFile("asset/texture/board1.png");
		g_TextureIdBoard[1] = Texture_SetTextureLoadFile("asset/texture/board2.png");
		g_TextureIdBoard[2] = Texture_SetTextureLoadFile("asset/texture/board3.png");
		g_TextureIdBoard[3] = Texture_SetTextureLoadFile("asset/texture/board4.png");
		g_TextureIdBoard[4] = Texture_SetTextureLoadFile("asset/texture/board5.png");
		g_TextureIdBoard[5] = Texture_SetTextureLoadFile("asset/texture/board6.png");
	}

	char buf[64];

	sprintf(buf, "asset/texture/ÉXÉeÅ[ÉW%02d.png", g_NowStage);
	g_TextureIdStageNum = Texture_SetTextureLoadFile(buf);

	// ÉeÉNÉXÉ`ÉÉÇÃì«Ç›çûÇ›
	if (Texture_Load() > 0) {
		MessageBox(NULL, "ÉeÉNÉXÉ`ÉÉÇÃì«Ç›çûÇ›Ç…é∏îsÇµÇ‹ÇµÇΩÅB", "ÉGÉâÅ[", MB_OK);
		return;
	}

	g_NowBgId = g_TextureId[g_NowStage % BG_MAX];

	g_PlayerState = STATE_WALK;

	g_AnimationCount = 0.0f;
}

void GameBg_Finalize(void)
{
	for (int i = 0; i < BG_MAX; i++) {
		Texture_Release(&g_TextureId[i], 1);
	}

	for (int i = 0; i < 3; i++) {
		Texture_Release(&g_TextureAnimationId[i], 1);
	}

	if (g_NowStage == 0) {
		for (int i = 0; i < BOARD_MAX; i++) {
			Texture_Release(&g_TextureIdBoard[i], 1);
		}
	}

	Texture_Release(&g_TextureIdStageNum, 1);
}

void GameBg_Update(void)
{
	g_PlayerState = GamePlayer_GetPlayer()->state;

	g_AnimationCount += 0.1f;
}

void GameBg_Draw(void)
{
	if (g_NowStage != 1) {
		Sprite_Draw(g_NowBgId, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1920, 1080);
	}
	else {
		Sprite_Draw(g_TextureAnimationId[(int)g_AnimationCount % 3], 0.0f, 0.0f);
	}

	if (g_NowStage != 0) {
		return;
	}
	
	Sprite_Draw(g_TextureIdBoard[4], SCREEN_WIDTH - 260.0f, 60.0f, 250.0f, 172.0f, 0, 0, 1250, 859);
	Sprite_Draw(g_TextureIdBoard[5], SCREEN_WIDTH - 260.0f, SCREEN_HEIGHT - 250.0f, 211.0f, 172.0f, 0, 0, 1056, 859);

	if (g_PlayerState == STATE_TURN) {
		Sprite_Draw(g_TextureIdBoard[2],  70.0f, 60.0f, 375.0f, 258.0f, 0, 0, 1250, 859);
		Sprite_Draw(g_TextureIdBoard[3], 520.0f, 60.0f, 375.0f, 258.0f, 0, 0, 1250, 859);
	}
	else {
		Sprite_Draw(g_TextureIdBoard[0],  70.0f, 60.0f, 375.0f, 258.0f, 0, 0, 1250, 859);
		Sprite_Draw(g_TextureIdBoard[1], 520.0f, 60.0f, 375.0f, 258.0f, 0, 0, 1250, 859);
	}
}

void GameBg_DrawStageNum(void)
{
	if (g_NowStage != 0) {
		Sprite_Draw(g_TextureIdStageNum, 20.0f, 0.0f, 128.0f, 64.0f, 0, 0, 256, 128);
	}
	else {
		Sprite_Draw(g_TextureIdStageNum, 20.0f, 8.0f, 256.0f, 48.0f, 0, 128, 512, 100);
	}
}

void GameBg_Reset(void)
{
	g_PlayerState = STATE_WALK;
}