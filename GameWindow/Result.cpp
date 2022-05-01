/*==============================================================================

	リザルト処理　[Result.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 15
==============================================================================*/
#include <d3dx9.h>
#include "Config.h"
#include "Sprite.h"
#include "Texture.h"
#include "Keylogger.h"
#include "Scene.h"
#include "Fade.h"
#include "Controller.h"
#include "StageSelect.h"
#include "sound.h"
#include "Game.h"
#include "Game_Timer.h"

#define CURSOR_TEXTURESIZE_WIDTH	  (100)
#define CURSOR_TEXTURESIZE_HEIGHT	  (100)
#define STAR_TEXTURESIZE_WIDTH	  (256)
#define STAR_TEXTURESIZE_HEIGHT	  (256)
typedef enum ResultMode_tag
{
	RESULT_FADE_IN,
	RESULT_SHOW,
	RESULT_MENU,
	RESULT_RANK,
	RESULT_FADE_OUT
}ResultMode;
typedef enum FrameCounterMode_tag
{
	ZERO,
	ONE,
	TWO,
	THREE,	
}FrameCounterMode;
/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
static int g_TextureIdResultBg = TEXTURE_INVALID_ID;			//リザルトの背景
static int g_TextureIdResultMenuBg = TEXTURE_INVALID_ID;		//リザルトメニューの背景
static int g_TextureIdToMenu = TEXTURE_INVALID_ID;				//メニューに戻るボタン
static int g_TextureIdToStageSelect = TEXTURE_INVALID_ID;		//StageSelcetに戻るボタン
static int g_TextureIdToNextStage = TEXTURE_INVALID_ID;			//NextStageに行くボタン
static int g_CursorTextureId = TEXTURE_INVALID_ID;				//カーソル表示
static int g_RetryTextureId = TEXTURE_INVALID_ID;				//リトライボタン
static int g_karahoshiTextureId = TEXTURE_INVALID_ID;			//空星表示
static int g_mitiruhoshiTextureId = TEXTURE_INVALID_ID;			//満ちる星表示
static ResultMode g_ResultMode;	
static int g_SelectCount = 0;
static Rank g_NowRank = RANK_NONE;
static D3DXVECTOR2 g_ToNextStagebuttonPosition	 = { 500.0f, 300.0f };
static D3DXVECTOR2 g_RetrybuttonPosition		 = { 500.0f, 400.0f };
static D3DXVECTOR2 g_ToStageSelectbuttonPosition = { 500.0f, 500.0f };
static D3DXVECTOR2 g_ToMenubuttonPosition		 = { 500.0f, 600.0f };
static D3DXVECTOR2 g_star1Position		 = { 510.0f, 0.0f };
static D3DXVECTOR2 g_star2Position		 = { 320.0f, 80.0f };
static D3DXVECTOR2 g_star3Position		 = { 700.0f, 80.0f };
static D3DXVECTOR2 g_CursorPosition[] = { 
	{  g_ToNextStagebuttonPosition.x + 350 , g_ToNextStagebuttonPosition.y },
	{        g_RetrybuttonPosition.x + 350 , g_RetrybuttonPosition.y },
	{g_ToStageSelectbuttonPosition.x + 350 , g_ToStageSelectbuttonPosition.y },
	{       g_ToMenubuttonPosition.x + 350 , g_ToMenubuttonPosition.y },

};
static int g_FrameCounter = 0;
static FrameCounterMode g_FrameCounterMode;
void Result_Initialize(void)
{
	// テクスチャの読み込み予約
	g_TextureIdResultBg = Texture_SetTextureLoadFile("asset/texture/clear.png");
	g_TextureIdResultMenuBg = Texture_SetTextureLoadFile("asset/texture/ResultMenuBG.png");
	g_TextureIdToMenu = Texture_SetTextureLoadFile("asset/texture/ToMenu.png");
	g_TextureIdToStageSelect = Texture_SetTextureLoadFile("asset/texture/ToStageSelect.png");
	g_TextureIdToNextStage = Texture_SetTextureLoadFile("asset/texture/ToNextStage.png");
	g_CursorTextureId = Texture_SetTextureLoadFile("asset/texture/screwmark02.png");
	g_RetryTextureId = Texture_SetTextureLoadFile("asset/texture/Retry.png");
	g_karahoshiTextureId = Texture_SetTextureLoadFile("asset/texture/空星.png");
	g_mitiruhoshiTextureId = Texture_SetTextureLoadFile("asset/texture/満ちる星.png");

	g_SelectCount = 0;
	g_FrameCounter = 0;
	
	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "テクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}
	g_FrameCounterMode = ZERO;
	g_ResultMode = RESULT_FADE_IN;
	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 60);
	PlaySound(SOUND_LABEL_BGM_CLEAR1);
}

void Result_Finalize(void)
{
	Texture_Release(&g_TextureIdResultBg	  , 1); 
	Texture_Release(&g_TextureIdResultMenuBg  , 1);
	Texture_Release(&g_TextureIdToMenu		  , 1); 
	Texture_Release(&g_TextureIdToStageSelect , 1); 
	Texture_Release(&g_TextureIdToNextStage	  , 1); 
	Texture_Release(&g_RetryTextureId, 1);
	Texture_Release(&g_karahoshiTextureId, 1);
	Texture_Release(&g_mitiruhoshiTextureId, 1);
}

void Result_Update(void)
{
	switch (g_ResultMode)
	{
	case RESULT_FADE_IN:
		if (!Fade_IsFade()) {
			g_ResultMode = RESULT_SHOW;
		}
		break;
	case RESULT_SHOW:
		if (Controller_Decision()) {
			g_ResultMode = RESULT_RANK;
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		break;
	case RESULT_RANK:
		g_FrameCounter++;
		if (g_FrameCounter > 200)
		{
			g_FrameCounter = 200;
		}
		switch (g_FrameCounter / 50)
		{
		case 1:			
			g_FrameCounterMode = ONE;			
			break;
		case 2:		
			g_FrameCounterMode = TWO;			
			break;
		case 3:
			g_FrameCounterMode = THREE;				
			break;
		}		
		if (Controller_Decision()) {
			g_ResultMode = RESULT_MENU;
			g_FrameCounter = 0;
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		break;
	case RESULT_MENU:
		if (Controller_Down())
		{
			g_SelectCount > 2 ? g_SelectCount = 0 : g_SelectCount++;
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Up())
		{
			g_SelectCount < 1 ? g_SelectCount = 3 : g_SelectCount--;
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Decision()) {
			g_ResultMode = RESULT_FADE_OUT;
			Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
			PlaySound(SOUND_LABEL_SE_DECISION);
		}
		break;
	case RESULT_FADE_OUT:
		if (!Fade_IsFade()) {
			// 次のシーンへ移行
			switch (g_SelectCount) {
			case 0:				
				StageSelect_ToNext();
				Scene_Change(SCENE_GAME);
				break;
			case 1:
				//Game_Retry();
				Scene_Change(SCENE_GAME);				
				break;
			case 2:
				Scene_Change(SCENE_STAGESELECT);			
				break;
			case 3:
				Scene_Change(SCENE_MENU);
				break;
			}			
		}
		break;
	}		
}

void Result_Draw(void)
{
	switch (g_ResultMode)
	{
	case RESULT_FADE_IN:
		Sprite_Draw(g_TextureIdResultBg, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1920, 1080);
		break;
	case RESULT_SHOW:
		Sprite_Draw(g_TextureIdResultBg, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1920, 1080);				
		break;
	case RESULT_RANK:
		Sprite_Draw(g_TextureIdResultBg, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1920, 1080);
		//星表記
		switch (g_NowRank)
		{
		case RANK_0:
			switch (g_FrameCounterMode) {
			case ONE:
				Sprite_Draw(g_karahoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;
			case TWO:
				Sprite_Draw(g_karahoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_karahoshiTextureId,
						g_star1Position.x, g_star1Position.y,
						STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;

			case THREE:
				Sprite_Draw(g_karahoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_karahoshiTextureId,
					g_star3Position.x, g_star3Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_karahoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;
			}
			break;
		case RANK_1:
			switch (g_FrameCounterMode) {
			case ONE:
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;
			case TWO:
				Sprite_Draw(g_karahoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;

			case THREE:
				Sprite_Draw(g_karahoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_karahoshiTextureId,
					g_star3Position.x, g_star3Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;
			}
			break;
		case RANK_2:
			switch (g_FrameCounterMode) {
			case ONE:
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				break;
			case TWO:
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				break;

			case THREE:
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_karahoshiTextureId,
					g_star3Position.x, g_star3Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT);

				break;
			}
				break;
		case RANK_3:
			switch (g_FrameCounterMode) {
			case ONE:
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				break;
			case TWO:				
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				break;
			case THREE:				
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star2Position.x, g_star2Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star3Position.x, g_star3Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				Sprite_Draw(g_mitiruhoshiTextureId,
					g_star1Position.x, g_star1Position.y,
					STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
				break;
			}
			
			
			
			break;
		}
		break;
	case RESULT_MENU:		
		Sprite_Draw(g_TextureIdResultMenuBg, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1280, 720);
		//星表記
		switch (g_NowRank)
		{
		case RANK_0:
			Sprite_Draw(g_karahoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_1:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_2:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_3:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x+50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star2Position.x+70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star3Position.x+30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		}
		Sprite_Draw(g_CursorTextureId,
			g_CursorPosition[g_SelectCount].x,
			g_CursorPosition[g_SelectCount].y,
			100, 100, 0, 0, 532, 532);
	
		Sprite_Draw(g_TextureIdToNextStage,
			g_ToNextStagebuttonPosition.x,
			g_ToNextStagebuttonPosition.y,
			250, 100, 0, 0, 350, 120);		
		Sprite_Draw(g_RetryTextureId,
			g_RetrybuttonPosition.x,
			g_RetrybuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		Sprite_Draw(g_TextureIdToStageSelect,
			g_ToStageSelectbuttonPosition.x,
			g_ToStageSelectbuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		Sprite_Draw(g_TextureIdToMenu,
			g_ToMenubuttonPosition.x,
			g_ToMenubuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		
		break;	
	case RESULT_FADE_OUT:	
		Sprite_Draw(g_TextureIdResultMenuBg, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1280, 720);
		//星表記
		switch (g_NowRank)
		{
		case RANK_0:
			Sprite_Draw(g_karahoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_1:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_2:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_karahoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		case RANK_3:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star1Position.x + 50, g_star1Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star2Position.x + 70, g_star2Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_star3Position.x + 30, g_star3Position.y,
				150, 150, 0, 0, STAR_TEXTURESIZE_WIDTH,STAR_TEXTURESIZE_HEIGHT);
			break;
		}
		Sprite_Draw(g_CursorTextureId,
			g_CursorPosition[g_SelectCount].x,
			g_CursorPosition[g_SelectCount].y,
			100, 100, 0, 0, 532, 532);

		Sprite_Draw(g_TextureIdToNextStage,
			g_ToNextStagebuttonPosition.x,
			g_ToNextStagebuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		Sprite_Draw(g_RetryTextureId,
			g_RetrybuttonPosition.x,
			g_RetrybuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		Sprite_Draw(g_TextureIdToStageSelect,
			g_ToStageSelectbuttonPosition.x,
			g_ToStageSelectbuttonPosition.y,
			250, 100, 0, 0, 350, 120);
		Sprite_Draw(g_TextureIdToMenu,
			g_ToMenubuttonPosition.x,
			g_ToMenubuttonPosition.y,
			250, 100, 0, 0, 350, 120);

		break;
	}	
}
void Result_GetRank(Rank rank)
{
	g_NowRank = rank;
}
 

 