/*==============================================================================

	ステージ選択処理　[StageSelect.cpp]

													Author : Sai Hakusen
													Date   : 2021 / 01 / 14
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
#define STAGE_TEXTURESIZE_WIDTH		  (150)
#define STAGE_TEXTURESIZE_HEIGHT	  (150)
#define CURSOR_TEXTURESIZE_WIDTH	  (100)
#define CURSOR_TEXTURESIZE_HEIGHT	  (100)
#define ClEARMARK_TEXTURESIZE_WIDTH   (60)
#define CLEARMARK_TEXTURESIZE_HEIGHT  (60)
#define TOMENUMARK_TEXTURESIZE_WIDTH  (350)
#define TOMENUMARK_TEXTURESIZE_HEIGHT (100)
#define STAR_TEXTURESIZE_WIDTH    (50)
#define STAR_TEXTURESIZE_HEIGHT	  (50)
typedef enum StageMode_tag
{
	STAGE_SELECT_FADE_IN,
	STAGE_SELECT_SHOW,
	STAGE_SELECT_FADE_OUT,
}StageMode;
typedef enum StageState_tag
{
	STAGE_OPEN,
	STAGE_UNOPEN,
	STAGE_CLEAR,
}StageState;
typedef struct StageInformation_tag
{
	StageState StageState;
	D3DXVECTOR2 StageTexturePosition;
	D3DXVECTOR2 CursorPosition;
	D3DXVECTOR2 ClearMarkPosition;
	D3DXVECTOR2 star1Position;
	D3DXVECTOR2 star2Position;
	D3DXVECTOR2 star3Position;
}StageInformation;

// テクスチャファイル名を保存
// 最初のダミーは「g_StageTextureId[0]にステージ○○.pngを設定するとフェードが無効になる」というバグを防ぐためのもの
static const char* TextureFileNames[] = {
	"asset/texture/fade.png",			// ダミー
	"asset/texture/ステージ01.png",
	"asset/texture/ステージ02.png",
	"asset/texture/ステージ03.png",
	"asset/texture/ステージ04.png",
	"asset/texture/ステージ05.png",
	"asset/texture/ステージ06.png",
	"asset/texture/ステージ07.png",
	"asset/texture/ステージ08.png",
	"asset/texture/ステージ09.png",
	"asset/texture/ステージ10.png",
	"asset/texture/ステージ11.png",
	"asset/texture/ステージ12.png",
	"asset/texture/ステージ13.png",
	"asset/texture/ステージ14.png",
	"asset/texture/ステージ15.png",
	"asset/texture/ステージ16.png",
	"asset/texture/ステージ17.png",
	"asset/texture/ステージ18.png",
	"asset/texture/ステージ19.png",
	"asset/texture/ステージ20.png",

};
/*------------------------------------------------------------------------------
	グローバル変数宣言
------------------------------------------------------------------------------*/
// 現在のステージ番号（今は0～20の21個）
static int g_NowStage = 0;
static int g_CursorTextureId = TEXTURE_INVALID_ID;
static int g_UnOpenMarkTextureId = TEXTURE_INVALID_ID;
static int g_OpenMarkTextureId = TEXTURE_INVALID_ID;
static int g_ClearMarkTextureId = TEXTURE_INVALID_ID;
static int g_ToMenuTextureId = TEXTURE_INVALID_ID;
static int g_BgTextureId = TEXTURE_INVALID_ID;
static int g_karahoshiTextureId = TEXTURE_INVALID_ID;
static int g_mitiruhoshiTextureId = TEXTURE_INVALID_ID;

static const int g_TextureCount = ARRAYSIZE(TextureFileNames);
static int g_number = g_TextureCount+1;
static int g_StageTextureId[g_TextureCount];
static int g_TutorialTextureId = TEXTURE_INVALID_ID;	// g_StageTextureId[0]のダミーに代わるテクスチャID
static StageMode g_StageMode;
static int g_SelectCount = 0;
static D3DXVECTOR2 g_ToMenuMarkPosition[] = {
	{470.0f,540.0f}
};
StageInformation g_StageInformation[] = {
	//0~5
	{  STAGE_OPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{  STAGE_OPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	//6~10									  				   			  
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	//11~15									   				   			   
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	//15~20									 				   			 
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }, { 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},
	//メニューボタン
	{STAGE_UNOPEN,{ 0.0f, 0.0f },{315.0f,540.0f},{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f },{ 0.0f, 0.0f }},

};

// 各ステージのランクを格納する配列
static Rank* g_Rank = NULL;
void StageSelect_Initialize(void)
{	
	StageSelect_SetRank(GameTimer_GetRank());

	for (int i = 0; i < g_TextureCount; i++) {
		g_StageTextureId[i] = Texture_SetTextureLoadFile(TextureFileNames[i]);
		g_StageInformation[i].StageTexturePosition = { 70.0f + i * 200.0f,230.0f };
		g_StageInformation[i].ClearMarkPosition = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - ClEARMARK_TEXTURESIZE_WIDTH) / 2,
								  g_StageInformation[i].StageTexturePosition.y - 110 };
		g_StageInformation[i].CursorPosition  = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - CURSOR_TEXTURESIZE_WIDTH) / 2,
								g_StageInformation[i].StageTexturePosition.y + 190 };
		g_StageInformation[i].star1Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - STAR_TEXTURESIZE_WIDTH),
								g_StageInformation[i].StageTexturePosition.y + (STAGE_TEXTURESIZE_HEIGHT - STAR_TEXTURESIZE_HEIGHT) };
		g_StageInformation[i].star2Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - 1.5f*STAR_TEXTURESIZE_WIDTH),
						g_StageInformation[i].StageTexturePosition.y + STAGE_TEXTURESIZE_HEIGHT  };
		g_StageInformation[i].star3Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - 0.5f*STAR_TEXTURESIZE_WIDTH),
						g_StageInformation[i].StageTexturePosition.y + STAGE_TEXTURESIZE_HEIGHT  };

		switch (g_Rank[i])
		{
		case RANK_NONE:
			g_StageInformation[i].StageState = STAGE_UNOPEN;
			break;
		case RANK_OPEN:
			g_StageInformation[i].StageState = STAGE_OPEN;
			break;
		default:
			g_StageInformation[i].StageState = STAGE_CLEAR;
			break;
		}
	}
	g_TutorialTextureId = Texture_SetTextureLoadFile("asset/texture/ステージ00.png");
	g_CursorTextureId = Texture_SetTextureLoadFile("asset/texture/MARK.png");
	g_UnOpenMarkTextureId = Texture_SetTextureLoadFile("asset/texture/screw_red.png");
	g_OpenMarkTextureId = Texture_SetTextureLoadFile("asset/texture/screw.png");
	g_ClearMarkTextureId = Texture_SetTextureLoadFile("asset/texture/screw_good.png");
	g_ToMenuTextureId = Texture_SetTextureLoadFile("asset/texture/ToMenu.png");
	g_BgTextureId= Texture_SetTextureLoadFile("asset/texture/StageSelectBG.png");
	g_karahoshiTextureId = Texture_SetTextureLoadFile("asset/texture/空星.png");
	g_mitiruhoshiTextureId = Texture_SetTextureLoadFile("asset/texture/満ちる星.png");
	// テクスチャの読み込み
	if (Texture_Load() > 0) {
		MessageBox(NULL, "ステージセレクトテクスチャの読み込みに失敗しました。", "エラー", MB_OK);
		return;
	}
	g_NowStage = 0;
	g_StageMode = STAGE_SELECT_FADE_IN;
	g_SelectCount = 0;

	//ステージ開放するかを判断(今は全てGame_Timer.cppで管理しているためここでの処理は不要)
	//for (int i = 0; i < STAGE_MAX; i++)
	//{
	//	/*if (g_StageInformation[i].StageState == STAGE_CLEAR
	//		&& g_StageInformation[i + 1].StageState != STAGE_CLEAR
	//		&& g_StageInformation[i + 2].StageState != STAGE_CLEAR) {
	//		g_StageInformation[i + 1].StageState = STAGE_OPEN;
	//		g_StageInformation[i + 2].StageState = STAGE_OPEN;
	//	}*/
	//	if (g_StageInformation[i].StageState != STAGE_CLEAR) {
	//		continue;
	//	}

	//	if (g_StageInformation[i + 1].StageState != STAGE_CLEAR) {
	//		g_StageInformation[i + 1].StageState = STAGE_OPEN;
	//	}

	//	if (g_StageInformation[i + 2].StageState != STAGE_CLEAR) {
	//		g_StageInformation[i + 2].StageState = STAGE_OPEN;
	//	}
	//}

	Fade_Start(FADE_IN, 0.0f, 0.0f, 0.0f, 30);
	PlaySound(SOUND_LABEL_BGM_MENU);
}

void StageSelect_Finalize(void)
{
	for (int i = 0; i < g_TextureCount; i++) {
		Texture_Release(&g_StageTextureId[i], 1);
	}	
	Texture_Release(&g_TutorialTextureId, 1);
	Texture_Release(&g_CursorTextureId, 1);
	Texture_Release(&g_UnOpenMarkTextureId, 1);
	Texture_Release(&g_OpenMarkTextureId, 1);
	Texture_Release(&g_ClearMarkTextureId, 1);
	Texture_Release(&g_ToMenuTextureId, 1);
	Texture_Release(&g_BgTextureId, 1);
	Texture_Release(&g_karahoshiTextureId, 1);
	Texture_Release(&g_mitiruhoshiTextureId, 1);

	StopSound(SOUND_LABEL_BGM_MENU);
}

void StageSelect_Update(void)
{
	switch (g_StageMode)
	{
	case STAGE_SELECT_FADE_IN:
		if (!Fade_IsFade()) {
			g_StageMode = STAGE_SELECT_SHOW;
		}
		break;
	case STAGE_SELECT_SHOW:
		if (Controller_Right() )
		{
			if (g_SelectCount < 20)
			{			
				if (g_SelectCount > 4) {
					for (int i = 0; i < STAGE_MAX; i++) {
						g_StageInformation[i].StageTexturePosition.x -= 200;
					}
				}
				g_SelectCount++;
			}											
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Left())
		{
			if (g_SelectCount > 0 && g_SelectCount < STAGE_MAX)
			{
				if (g_SelectCount > 5) {
					for (int i = 0; i < STAGE_MAX; i++) {
						g_StageInformation[i].StageTexturePosition.x += 200;
					}
				}			
				g_SelectCount--;
			}
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Up())
		{
			//メニューボタンからステージ選択ボタン位置に戻る用
			g_number== STAGE_MAX +1? g_number:g_SelectCount = g_number , g_number= STAGE_MAX +1;
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Down())
		{
			//メニューボタンに移動前、選択されているステージ番号を記録する
			if (g_SelectCount != STAGE_MAX) {
				g_number = g_SelectCount;
				g_SelectCount = STAGE_MAX;
			}			
			PlaySound(SOUND_LABEL_SE_SELECT);
		}
		if (Controller_Decision())
		{	
			//未開放のスデージをプレイすることできない
			if (g_StageInformation[g_SelectCount].StageState!=STAGE_UNOPEN||g_SelectCount==STAGE_MAX) {
				g_StageMode = STAGE_SELECT_FADE_OUT;
				Fade_Start(FADE_OUT, 0.0f, 0.0f, 0.0f, 60);
			}		
			PlaySound(SOUND_LABEL_SE_DECISION);
		}	
		g_SelectCount == STAGE_MAX ? g_NowStage : g_NowStage = g_SelectCount;
		
		break;
	case STAGE_SELECT_FADE_OUT:
		
		if (!Fade_IsFade()) {
			// 次のシーンへ移行			
			g_SelectCount == STAGE_MAX ? Scene_Change(SCENE_MENU) : Scene_Change(SCENE_GAME);			
		}
		break;
	}	
	//カソール位置とクリアアイコン位置、更新
	for (int i = 0; i < STAGE_MAX; i++) {
		g_StageInformation[i].CursorPosition  = { g_StageInformation[i].StageTexturePosition .x + (STAGE_TEXTURESIZE_WIDTH - CURSOR_TEXTURESIZE_WIDTH) / 2,
								g_StageInformation[i].StageTexturePosition .y + 170 };
		g_StageInformation[i].ClearMarkPosition = { g_StageInformation[i].StageTexturePosition .x + (STAGE_TEXTURESIZE_WIDTH - ClEARMARK_TEXTURESIZE_WIDTH) / 2,
								  g_StageInformation[i].StageTexturePosition .y - 110, };
		g_StageInformation[i].star1Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - STAR_TEXTURESIZE_WIDTH),
								g_StageInformation[i].StageTexturePosition.y + (STAGE_TEXTURESIZE_HEIGHT - STAR_TEXTURESIZE_HEIGHT) };
		g_StageInformation[i].star2Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - 1.5f * STAR_TEXTURESIZE_WIDTH),
						g_StageInformation[i].StageTexturePosition.y + STAGE_TEXTURESIZE_HEIGHT };
		g_StageInformation[i].star3Position = { g_StageInformation[i].StageTexturePosition.x + (STAGE_TEXTURESIZE_WIDTH - 0.5f * STAR_TEXTURESIZE_WIDTH),
						g_StageInformation[i].StageTexturePosition.y + STAGE_TEXTURESIZE_HEIGHT };


	}	
}
void StageSelect_Draw(void)
{
	//BG描画
	Sprite_Draw(g_BgTextureId, 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 1280, 720);

	//カソール描画
	Sprite_Draw(g_CursorTextureId,
		g_StageInformation[g_SelectCount].CursorPosition.x,
		g_StageInformation[g_SelectCount].CursorPosition.y,
		CURSOR_TEXTURESIZE_WIDTH, CURSOR_TEXTURESIZE_HEIGHT, 0, 0, 532, 532);
	//メニュー戻るきー描画
	Sprite_Draw(g_ToMenuTextureId,
		g_ToMenuMarkPosition[0].x,
		g_ToMenuMarkPosition[0].y,
		TOMENUMARK_TEXTURESIZE_WIDTH, TOMENUMARK_TEXTURESIZE_HEIGHT, 0, 0, 350, 120);
	//チュートリアルセレクトアイコン描画
	Sprite_Draw(g_TutorialTextureId,
		g_StageInformation[0].StageTexturePosition.x,
		g_StageInformation[0].StageTexturePosition.y,
		STAGE_TEXTURESIZE_WIDTH, STAGE_TEXTURESIZE_HEIGHT, 0, 0, 512, 512);
	for(int i=0;i< STAGE_MAX;i++){
		//各ステージセレクトアイコン描画
		if (i != 0) {
			Sprite_Draw(g_StageTextureId[i],
				g_StageInformation[i].StageTexturePosition.x,
				g_StageInformation[i].StageTexturePosition.y,
				STAGE_TEXTURESIZE_WIDTH, STAGE_TEXTURESIZE_HEIGHT, 0, 0, 256, 128);
		}
		
		switch (g_Rank[i])
		{
		case RANK_0:
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star1Position.x, g_StageInformation[i].star1Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star2Position.x, g_StageInformation[i].star2Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star3Position.x, g_StageInformation[i].star3Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			break;
		case RANK_1:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star1Position.x, g_StageInformation[i].star1Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star2Position.x, g_StageInformation[i].star2Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star3Position.x, g_StageInformation[i].star3Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			break;
		case RANK_2:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star1Position.x, g_StageInformation[i].star1Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star2Position.x, g_StageInformation[i].star2Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_karahoshiTextureId,
				g_StageInformation[i].star3Position.x, g_StageInformation[i].star3Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			break;
		case RANK_3:
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star1Position.x, g_StageInformation[i].star1Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star2Position.x, g_StageInformation[i].star2Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			Sprite_Draw(g_mitiruhoshiTextureId,
				g_StageInformation[i].star3Position.x, g_StageInformation[i].star3Position.y,
				STAR_TEXTURESIZE_WIDTH, STAR_TEXTURESIZE_HEIGHT, 0, 0, 256, 256);
			break;
		}
		//未開放アイコン描画
		if(g_StageInformation[i].StageState==STAGE_UNOPEN){
			Sprite_Draw(g_UnOpenMarkTextureId,
				g_StageInformation[i].ClearMarkPosition .x,
				g_StageInformation[i].ClearMarkPosition .y,
				ClEARMARK_TEXTURESIZE_WIDTH, CLEARMARK_TEXTURESIZE_HEIGHT, 0, 0, 886, 886);
		}
		//開放アイコン描画
		if(g_StageInformation[i].StageState == STAGE_OPEN){
			Sprite_Draw(g_OpenMarkTextureId,
				g_StageInformation[i].ClearMarkPosition.x,
				g_StageInformation[i].ClearMarkPosition.y,
				ClEARMARK_TEXTURESIZE_WIDTH, CLEARMARK_TEXTURESIZE_HEIGHT, 0, 0, 886, 886);
		}
		//クリアアイコン描画
		if(g_StageInformation[i].StageState == STAGE_CLEAR){
			Sprite_Draw(g_ClearMarkTextureId,
				g_StageInformation[i].ClearMarkPosition.x,
				g_StageInformation[i].ClearMarkPosition.y,
				ClEARMARK_TEXTURESIZE_WIDTH, CLEARMARK_TEXTURESIZE_HEIGHT, 0, 0, 886, 886);
		}	
		
	}	
}

int StageSelect_GetStageNumber(void)
{
	return g_NowStage;
}

void StageSelect_ToNext(void)
{
	// 今は最後のステージクリアしたらステージ1に戻るようにしてる
	g_NowStage = g_NowStage == STAGE_MAX ? 1 : g_NowStage + 1;
}
void StageSelect_StageClear(int StageNumber)
{
	g_StageInformation[StageNumber].StageState = STAGE_CLEAR;	
}
void StageSelect_SetRank(Rank* rank)
{
	g_Rank = rank;
}