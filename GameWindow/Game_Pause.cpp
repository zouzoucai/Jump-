/*==============================================================================

	ゲームのポーズ処理　[Game_Pause.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 10
==============================================================================*/
#include <d3dx9.h>

#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Controller.h"
#include "sound.h"
#include "Game_Pause.h"

#define PAUSE_MENU_SIZE		(420.0f)
#define PAUSE_MENU_POS_X	((SCREEN_WIDTH - PAUSE_MENU_SIZE) / 2)
#define PAUSE_MENU_POS_Y	((SCREEN_HEIGHT - PAUSE_MENU_SIZE) / 2 - 20.0f)

#define PAUSE_MARK_WIDTH		(80.0f)
#define PAUSE_MARK_HEIGHT		(40.0f)

static int g_TextureId_Fade = TEXTURE_INVALID_ID;			// ゲーム画面を暗くするのに使用
static int g_TextureId_PauseMenu = TEXTURE_INVALID_ID;
static int g_TextureId_Mark = TEXTURE_INVALID_ID;			//マーク

PauseSelect g_PauseSelect = PAUSE_NONE;	// 選ばれた場所

static int g_PauseCount = 0;			// マークがある場所

D3DXVECTOR2 g_MarkPosition(PAUSE_MENU_POS_X + PAUSE_MENU_SIZE, 290.0f);

void GamePause_Initialize(void)
{
	g_TextureId_Fade = Texture_SetTextureLoadFile("asset/texture/fade.png");
	g_TextureId_PauseMenu = Texture_SetTextureLoadFile("asset/texture/pause_menu改.png");
	g_TextureId_Mark = Texture_SetTextureLoadFile("asset/texture/screwmark02.png");

	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}

	GamePause_Set();
}

void GamePause_Finalize(void)
{
	Texture_Release(&g_TextureId_Fade, 1);
	Texture_Release(&g_TextureId_PauseMenu, 1);
	Texture_Release(&g_TextureId_Mark, 1);
}

void GamePause_Update(void)
{
	//↑　↓とDECISIONキーでメニューバーを選ぶ
	if (Controller_Down() && g_PauseCount < 3)
	{
		g_PauseCount++;
		g_MarkPosition.y += 72.0f;
		PlaySound(SOUND_LABEL_SE_SELECT);
	}
	if (Controller_Up() && g_PauseCount > 0)
	{
		g_PauseCount--;
		g_MarkPosition.y -= 72.0f;
		PlaySound(SOUND_LABEL_SE_SELECT);
	}

	if (Controller_Decision()) {
		switch (g_PauseCount)
		{
		case 0:
			g_PauseSelect = PAUSE_BACK;
			break;
		case 1:
			g_PauseSelect = PAUSE_RETRY;
			break;
		case 2:
			g_PauseSelect = PAUSE_TOSTAGESELECT;
			break;
		case 3:
			g_PauseSelect = PAUSE_TOMENU;
			break;
		}
		PlaySound(SOUND_LABEL_SE_DECISION);
	}

	if (Controller_Menu() || Controller_Cancel()) {
		g_PauseSelect = PAUSE_BACK;
		PlaySound(SOUND_LABEL_SE_DECISION);
	}
}

void GamePause_Draw(void)
{
	Sprite_SetColor(D3DCOLOR_RGBA(0, 0, 0, 100));

	Sprite_Draw(g_TextureId_Fade, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 8, 8);

	Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));

	Sprite_Draw(g_TextureId_PauseMenu, PAUSE_MENU_POS_X, PAUSE_MENU_POS_Y, PAUSE_MENU_SIZE, PAUSE_MENU_SIZE, 0, 0, 530, 530);

	Sprite_Draw(g_TextureId_Mark, g_MarkPosition.x, g_MarkPosition.y, PAUSE_MARK_WIDTH, PAUSE_MARK_HEIGHT, 0, 0, 570, 350);
}

void GamePause_Set(void)
{
	g_PauseSelect = PAUSE_NONE;

	g_PauseCount = 0;

	g_MarkPosition = { PAUSE_MENU_POS_X + PAUSE_MENU_SIZE, 290.0f };
}

PauseSelect GamePause_GetSelect(void)
{
	return g_PauseSelect;
}